/*
 * fileSystem.c
 *
 *  Created on: 20.05.2017
 *      Author: benimurza
 */

#include "fileSystem.h"
#include "abstractStorage.h"

#include <string.h>

#define PATH_DELIMITER '/'

// FAT16 constraints
#define MAX_CHAR_FILE_NAME 8
#define MAX_CHAR_EXTENSION 3

// File types
#define FAT16_DIRECTORY_ENTRY 0x10
#define FAT16_ARCHIVE_ENTRY   0x20

// Cluster defines
#define INVALID_CLUSTER 0 // Cluster 0 and 1 are invalid

#define STORAGE_SECTOR_SIZE 512
#define PARTITION_TABLE_OFFSET 0x1BE

#define PARTITION_SECTOR_SIGNATURE 0xAA55 // inverted byte order

// Must not be greater than 255
#define MAX_NUMBER_FILE_DESCRIPTORS 16

typedef struct{
    uint32_t bytesRemainingInFile;
    uint32_t beginningOfFileAsClusterNumber;

    uint32_t fileSize;

    // Use uint16 instead of uint8 because of memory alignment issues
    uint16_t isSlotTaken;
}__attribute((packed)) FileDescriptor_t;

// A struct that contains data for accessing the file system.
typedef struct{
    uint32_t fatTableAddress;
    uint32_t rootDirectoryAddress;
    uint32_t numberOfSectorsPerFatTable;
    uint16_t maximumNumberOfEntriesInRoot;
    uint16_t currentClusterBeingRead;
    uint32_t bytesRemainingInFile;
    uint32_t bytesRemainingInCluster;
    uint32_t sectorsPerCluster;

    // Array containing the file descriptors
    FileDescriptor_t fileDescriptors[MAX_NUMBER_FILE_DESCRIPTORS];

    // This is the directory currently being read. After initialization, this directory is the root directory by default.
    uint32_t currentDirectoryAddressInBytes;
}__attribute((packed)) FileSystemState_t;

static FileSystemState_t fileSystemState;

// Function declarations
static uint16_t getNextClusterToRead(uint16_t currentCluster);
static uint8_t compareFileNames(uint8_t* file1, uint8_t* ext1, uint8_t* file2, uint8_t* ext2);
static uint32_t getClusterAdressInBytes(uint32_t clusterNumber);
static uint32_t readBootSector(void);
static uint32_t readPartitionTable(PartitionTable_t * partitionTable);
static int16_t getNextFreeFileDescriptorSlot(void);
static uint32_t getNextDirectory(uint8_t * dirName, uint32_t currentDirectory);
static int16_t openFileEntry(uint8_t * fileName, uint8_t* extension, uint32_t addressOfCurrentDir);
static uint8_t* removeWhiteSpacesFromUint8Array(uint8_t* input);

/*
 * Returns next free FD slot, or -1 if no free FD slot
 */
int16_t getNextFreeFileDescriptorSlot(void){
    volatile uint8_t i = 0;
    for(i = 0; i < MAX_NUMBER_FILE_DESCRIPTORS; i++){
        if(fileSystemState.fileDescriptors[i].isSlotTaken == 0){
            // slot not taken, return 0
            return i;
        }
    }
    return -1;
}

/*
 * Internal helper function to compare two file names (with extensions).
 * Filenames must be 8 chars long (padding with space), extensions must be 3 characters long.
 * Returns 1 if files are equal, 0 otherwise.
 */
uint8_t compareFileNames(uint8_t* file1, uint8_t* ext1, uint8_t* file2, uint8_t* ext2){
    volatile uint8_t i = 0;
    for(i = 0; i < MAX_CHAR_FILE_NAME; i++){
        if(file1[i]!=file2[i]){
            return 0;
        }
    }
    for(i = 0; i < MAX_CHAR_EXTENSION; i++){
        if(ext1[i]!=ext2[i]){
            return 0;
        }
    }
    return 1;
}

/*
 * Returns the cluster address (in bytes) for a cluster number
 */
uint32_t getClusterAdressInBytes(uint32_t clusterNumber){
    // Calculate address of data area
    uint32_t dataAreaStartAddress = fileSystemState.rootDirectoryAddress + (fileSystemState.maximumNumberOfEntriesInRoot * sizeof(Fat16Entry_t));
    return dataAreaStartAddress + ((clusterNumber-2) * fileSystemState.sectorsPerCluster * STORAGE_SECTOR_SIZE); // clusterNumber-2 because of FAT16 anomaly
}

uint32_t readPartitionTable(PartitionTable_t * partitionTable){
    if(partitionTable==0){
        return 1;
    }

    uint8_t buffer[STORAGE_SECTOR_SIZE];
    if (readSector(buffer, 0) != STORAGE_SECTOR_SIZE){
        // Did not read expected 512 bytes. Make sure storage is initialized.
        return 2;
    }

    // TODO: maybe analyze all four partitions instead of just the first?
    memcpy(partitionTable, buffer + PARTITION_TABLE_OFFSET, sizeof(PartitionTable_t));

    return 0;
}

/*
 * Gets the next cluster to be read from the FAT table. As input, the current cluster should be provided.
 */
uint16_t getNextClusterToRead(uint16_t currentCluster){
    // The FAT table may spread across multiple sectors. Check which sector should be read.
    uint32_t fatSectorToRead = currentCluster/256; // 256, because a cluster number is 16 bit (FAT16) large

    if(fatSectorToRead>fileSystemState.numberOfSectorsPerFatTable){
        // Out of FAT bounds
        return 0xFFFF;
    }

    uint16_t fatTable[256];
    if(readSector((uint8_t*)fatTable, (fileSystemState.fatTableAddress + (STORAGE_SECTOR_SIZE * fatSectorToRead)))!=STORAGE_SECTOR_SIZE){
        return 0xFFFF;
    }

    return fatTable[currentCluster];
}

/*
 * Reading the boot sector will cause the fileSystemState struct to be updated/written with information.
 */
uint32_t readBootSector(void){
    PartitionTable_t partitionTable;
    if(readPartitionTable(&partitionTable)){
        // Error reading partition table
        return 2;
    }

    if(partitionTable.partitionType != FAT16_GT_32_MB_LBA){
        // Unknown FAT type - currently only supporting type 14
        return 3;
    }

    FAT16BootSector_t bootSector;

    // Calculate address of boot sector
    if(readSector((uint8_t*)&bootSector, STORAGE_SECTOR_SIZE*partitionTable.relativeOffsetPartitionSectors) != STORAGE_SECTOR_SIZE){
        return 4;
    }

    // Write the fileSystemState struct:

    // Write FAT table address in the global variable
    fileSystemState.fatTableAddress = (bootSector.reservedSectors + partitionTable.relativeOffsetPartitionSectors)*STORAGE_SECTOR_SIZE;

    // Write root directory address in the global variable
    fileSystemState.rootDirectoryAddress = ((bootSector.reservedSectors+bootSector.numSectorsPerFATTable*bootSector.numberOfFATs)*STORAGE_SECTOR_SIZE)
                            +STORAGE_SECTOR_SIZE*partitionTable.relativeOffsetPartitionSectors;

    // Initial directory to read is the root directory
    fileSystemState.currentDirectoryAddressInBytes = fileSystemState.rootDirectoryAddress;

    // Write number of sectors per FAT table in root directory
    fileSystemState.numberOfSectorsPerFatTable = bootSector.numSectorsPerFATTable;

    // Maximum number of entries in root
    fileSystemState.maximumNumberOfEntriesInRoot = ((uint16_t)bootSector.numberOfEntriesInRoot[1]<<8) | (bootSector.numberOfEntriesInRoot[0]);

    fileSystemState.sectorsPerCluster = bootSector.numberOfSectorsPerCluster;

    return 0;
}

/*
 * Reads the boot sectors and saves the most important information in a module-global struct.
 */
uint32_t filesystem_Initialize(void){
    return readBootSector();
}

/*
 * Returns address of next directory, or 0 if not found/not a directory.
 */
uint32_t getNextDirectory(uint8_t * dirName, uint32_t currentDirectory){
    // Local buffer
    uint8_t buffer[STORAGE_SECTOR_SIZE];

    uint32_t sizeOfFatEntry = sizeof(Fat16Entry_t);

    // Local variable to store the currently read entry
    Fat16Entry_t currentEntry;
    volatile uint16_t i = 0;

    uint8_t * dirExtension = "   ";


    for(i=0; i < fileSystemState.maximumNumberOfEntriesInRoot; i+=sizeOfFatEntry){

        if(i%STORAGE_SECTOR_SIZE==0){
            // Read current directory
            readSector(buffer, currentDirectory+i);
        }

        memcpy((void*)&currentEntry, buffer+i, sizeof(currentEntry));

        if(compareFileNames(currentEntry.filename, currentEntry.ext, dirName, dirExtension)){
            // File found. Check if it is a directory. If yes, change the current directory global variable.
            if(currentEntry.attributes == FAT16_DIRECTORY_ENTRY){
                // Entry is a directory
                return getClusterAdressInBytes(currentEntry.starting_cluster);
            }
            else {
                return 0;
            }
        }
    }

    return 0;
}

/*
 * Opens a file entry. If fileName points to a directory name, -2 (error) is returned.
 */
int16_t openFileEntry(uint8_t * fileName, uint8_t* extension, uint32_t addressOfCurrentDir){
    // Calculate size of a FAT entry
    uint32_t sizeOfFatEntry = sizeof(Fat16Entry_t);
    // Iterate through entries in the root directory, until file is found

    uint8_t buffer[STORAGE_SECTOR_SIZE];

    // Local variable to store the currently read entry
    Fat16Entry_t currentEntry;
    volatile uint32_t i = 0;
    for(i=0; i < fileSystemState.maximumNumberOfEntriesInRoot; i+=sizeOfFatEntry){

        if(i%STORAGE_SECTOR_SIZE==0){
            readSector(buffer, addressOfCurrentDir+i);
        }

        memcpy((void*)&currentEntry, buffer+i, sizeof(currentEntry));
        if(compareFileNames(currentEntry.filename, currentEntry.ext, fileName, extension)){
            // File found. Check if it is a directory. If yes, change the current directory global variable.
            if(currentEntry.attributes == FAT16_DIRECTORY_ENTRY){
                // Entry is a directory
                return -2;
            }

            int16_t fileDescriptor = getNextFreeFileDescriptorSlot();

            // Get a file descriptor (if available)
            if(fileDescriptor==-1){
                // Too many files are opened, and no FD available
                return -1;
            }

            fileSystemState.fileDescriptors[fileDescriptor].beginningOfFileAsClusterNumber = currentEntry.starting_cluster;
            fileSystemState.fileDescriptors[fileDescriptor].bytesRemainingInFile = currentEntry.file_size;
            fileSystemState.fileDescriptors[fileDescriptor].fileSize = currentEntry.file_size;
            // Slot taken
            fileSystemState.fileDescriptors[fileDescriptor].isSlotTaken = 1;

            return fileDescriptor;
        }
    }

    // File not found
    return -1;
}


int16_t fileSystem_openFile(uint8_t * fileName){
    if(fileName==NULL){
        return -3;
    }

    if (*fileName != '/') {
        return -4;
    } else {
        fileName += 1;
    }

    // Split fileName according to delimiter. Use strnchr in order not to change the original string
    uint8_t * currentName = (uint8_t*) strchr((char*)fileName, PATH_DELIMITER);

    // Initialize dir name with spaces
    uint8_t currentDirName[8];
    memset(currentDirName,' ', 8);

    uint8_t fileToOpen[8];
    memset(fileToOpen,' ', 8);

    uint8_t fileToOpenExtension[3];
    memset(fileToOpenExtension,' ', 3);

    uint32_t addressOfNextDirectoryToOpen = fileSystemState.rootDirectoryAddress;
    uint32_t lastPosition = 0;

    // As long as currentPath is not null, navigate through directories
    while(currentName!=0){
        // Copy current directory name to local buffer
        strncpy((char*)currentDirName, (char*)(fileName + lastPosition), currentName - fileName - lastPosition);

        // currentDirName now contains the name of the directory to open
        addressOfNextDirectoryToOpen = getNextDirectory(currentDirName, addressOfNextDirectoryToOpen);

        if(addressOfNextDirectoryToOpen==0){
            // Not a directory, or does not exist
            return -3;
        }

        // Update last position
        lastPosition = currentName - fileName + 1;

        currentName = (uint8_t*)strchr((char*)(currentName+1), PATH_DELIMITER);

        // Reinitialize current name with spaces
        memset(currentDirName,' ', 8);
    }
    // currentPath has reached 0, meaning all directories have been accessed and only the filename + extension remains
    // Split filename and extension
    uint8_t * firstOccurenceOfDot = (uint8_t*)strchr((char*)(fileName + lastPosition), '.');

    // Copy name of file to open, without extension, to local buffer
    strncpy((char*)fileToOpen, (char*)(fileName + lastPosition) , firstOccurenceOfDot - (fileName + lastPosition));

    // Copy extension
    strncpy((char*)fileToOpenExtension, (char*)(firstOccurenceOfDot + 1), 3);

    return openFileEntry(fileToOpen, fileToOpenExtension, addressOfNextDirectoryToOpen);
}

void fileSystem_closeFile(uint8_t fileDescriptor){
    if(fileDescriptor < MAX_NUMBER_FILE_DESCRIPTORS){
        // Free slot
        fileSystemState.fileDescriptors[fileDescriptor].isSlotTaken = 0;
    }
}

uint32_t fileSystem_readBytes(uint8_t fileDescriptor, uint8_t * buffer, uint32_t bufferSize){
    if(buffer==NULL){
        return 0;
    }

    if(fileDescriptor>=MAX_NUMBER_FILE_DESCRIPTORS){
        // Invalid file descriptor
        return 0;
    }

    if(fileSystemState.fileDescriptors[fileDescriptor].isSlotTaken==0){
        // Slot not taken => no file opened for this descriptor
        return 0;
    }

    uint32_t bytesRemainingInFile = fileSystemState.fileDescriptors[fileDescriptor].bytesRemainingInFile;

    uint32_t fileSize = fileSystemState.fileDescriptors[fileDescriptor].fileSize;

    if(bytesRemainingInFile==0){
        return 0;
    }

    // Get current cluster
    uint16_t currentCluster = fileSystemState.fileDescriptors[fileDescriptor].beginningOfFileAsClusterNumber;

    uint32_t clusterSizeInBytes = fileSystemState.sectorsPerCluster*STORAGE_SECTOR_SIZE;

    // Local buffer in which each sector is stored
    uint8_t localBuffer[STORAGE_SECTOR_SIZE];

    // Position where file read was left off
    uint32_t resumePosition = fileSystemState.fileDescriptors[fileDescriptor].fileSize - bytesRemainingInFile;

    // Tracks how many clusters have been read
    uint16_t clustersRead = resumePosition/STORAGE_SECTOR_SIZE;

    if(resumePosition%STORAGE_SECTOR_SIZE!=0){
        // Read first sector
        if(readSector(localBuffer, getClusterAdressInBytes(currentCluster)+((resumePosition/STORAGE_SECTOR_SIZE)*STORAGE_SECTOR_SIZE)) != STORAGE_SECTOR_SIZE){
            // Some unexpected error occurred.
            return 0;
        }
    }

    // Keep track of the passed buffer position
    volatile uint32_t bufferPosition = 0;

    volatile uint32_t i = 0;
    for(i = resumePosition; i < bufferSize+resumePosition; i++){
        // if sector beginning, read sector
        if(i%STORAGE_SECTOR_SIZE==0){
            // Check if next cluster needs to be read. A cluster is made up of multiple sectors
            if(i/(clusterSizeInBytes + (clusterSizeInBytes*clustersRead))>=1){
                currentCluster = getNextClusterToRead(currentCluster);
                clustersRead++;
            }

            // Read a sector
            if(readSector(localBuffer, getClusterAdressInBytes(currentCluster)+(i%clusterSizeInBytes)) != STORAGE_SECTOR_SIZE){
                return i;
            }
        }

        if(i>=fileSize){
            fileSystemState.fileDescriptors[fileDescriptor].bytesRemainingInFile = 0;
            // EOF reached
            return i;
        }

        // Local buffer is only 512 bytes, so cycle through it using %.
        buffer[bufferPosition] = localBuffer[i%STORAGE_SECTOR_SIZE];
        bufferPosition++;
    }

    // Update how many bytes have been read
    fileSystemState.fileDescriptors[fileDescriptor].bytesRemainingInFile -= bufferSize;

    return bufferSize;
}

uint8_t* removeWhiteSpacesFromUint8Array(uint8_t* input)
{
    uint32_t i,j;
    uint8_t *output=input;
    for (i = 0, j = 0; i<strlen((char*)input); i++,j++)
    {
        if (input[i]!=' ')
            output[j]=input[i];
        else
            j--;
    }
    output[j]=0;
    return output;
}

uint8_t * fileSystem_getNextEntryInDirectory(uint8_t * dirName){
    if(dirName==NULL){
        return 0;
    }

    // Skip leading slash
    if (*dirName != '/') {
        return 0;
    } else {
        dirName += 1;
    }

    static const uint8_t * lastDirName;
    static uint32_t indexOfLastReadEntry = 0;

    if (lastDirName == NULL || lastDirName != dirName || strcmp((char*)lastDirName, (char*)dirName) != 0) {
        lastDirName = dirName;
        // Directory has changed, reset index of last entry
        indexOfLastReadEntry = 0;
    }

    // Split fileName according to delimiter. Use strnchr in order not to change the original string
    uint8_t * currentName = (uint8_t*) strchr((char*)dirName, PATH_DELIMITER);

    // Initialize dir name with spaces
    uint8_t currentDirName[8];
    memset(currentDirName,' ', 8);

    uint32_t addressOfNextDirectoryToOpen = fileSystemState.rootDirectoryAddress;
    uint32_t lastPosition = 0;

    // As long as currentPath is not null, navigate through directories
    while(currentName!=0){
        // Copy current directory name to local buffer
        strncpy((char*)currentDirName, (char*)(dirName + lastPosition), currentName - dirName - lastPosition);

        // currentDirName now contains the name of the directory to open
        addressOfNextDirectoryToOpen = getNextDirectory(currentDirName, addressOfNextDirectoryToOpen);

        if(addressOfNextDirectoryToOpen==0){
            // Not a directory, or does not exist
            return 0;
        }

        // Update last position
        lastPosition = currentName - dirName + 1;

        currentName = (uint8_t*)strchr((char*)(currentName+1), PATH_DELIMITER);

        // Reinitialize current name with spaces
        memset(currentDirName,' ', MAX_CHAR_FILE_NAME);
    }

    // Current dir name is available. Read current dir.
    // Create local buffer
    uint8_t buf[STORAGE_SECTOR_SIZE];

    uint32_t fileEntriesToSkip = indexOfLastReadEntry;

    Fat16Entry_t currentEntry;

    // Max size is: size of filename + size of dot + size of extension + size of null termination + 3 chars for special filenames (starting with / and ending with ~X)
    static uint8_t fileNameToReturn[MAX_CHAR_FILE_NAME + MAX_CHAR_EXTENSION + 5];

    uint32_t i = 0;
    for(i = 0; i < fileSystemState.maximumNumberOfEntriesInRoot; i+=sizeof(Fat16Entry_t)){
        if(i%STORAGE_SECTOR_SIZE == 0){
            readSector(buf, addressOfNextDirectoryToOpen);
        }

        // Copy current position to a FAT16 entry
        memcpy((void*)&currentEntry, buf+i, sizeof(currentEntry));

        if((currentEntry.attributes == FAT16_DIRECTORY_ENTRY) || (currentEntry.attributes == FAT16_ARCHIVE_ENTRY)){
            if(fileEntriesToSkip == 0){
                // File name to be returned must be reinitialized, in case there are some leftover chars
                memset(fileNameToReturn, ' ', MAX_CHAR_FILE_NAME + MAX_CHAR_EXTENSION + 5);

                strncpy((char*)fileNameToReturn, (char*)currentEntry.filename, MAX_CHAR_FILE_NAME);
                if(currentEntry.attributes == FAT16_ARCHIVE_ENTRY){
                    // Add '.' and extension
                    fileNameToReturn[MAX_CHAR_FILE_NAME] = '.';
                    strncpy((char*)(fileNameToReturn+MAX_CHAR_FILE_NAME+1), (char*)currentEntry.ext, MAX_CHAR_EXTENSION);
                }
                indexOfLastReadEntry++;
                fileNameToReturn[MAX_CHAR_FILE_NAME + MAX_CHAR_EXTENSION + 4] = 0;
                return removeWhiteSpacesFromUint8Array(fileNameToReturn);
            } else {
                fileEntriesToSkip--;
            }
        }
    }

    // Nothing found
    return 0;
}
