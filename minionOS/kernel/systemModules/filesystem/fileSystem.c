/*
 * fileSystem.c
 *
 *  Created on: 20.05.2017
 *      Author: benimurza
 */

#include "fileSystem.h"
#include "abstractStorage.h"

// File types
#define FAT16_DIRECTORY_ENTRY 0x10

// Cluster defines
#define INVALID_CLUSTER 0 // Cluster 0 and 1 are invalid

#define STORAGE_SECTOR_SIZE 512
#define PARTITION_TABLE_OFFSET 0x1BE

#define PARTITION_SECTOR_SIGNATURE 0xAA55 // inverted byte order


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

/*
 * Internal helper function to compare two file names (with extensions).
 * Filenames must be 8 chars long (padding with space), extensions must be 3 characters long.
 * Returns 1 if files are equal, 0 otherwise.
 */
uint8_t compareFileNames(uint8_t* file1, uint8_t* ext1, uint8_t* file2, uint8_t* ext2){
    volatile uint8_t i = 0;
    for(i = 0; i < 8; i++){
        if(file1[i]!=file2[i]){
            return 0;
        }
    }
    for(i = 0; i < 3; i++){
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

uint32_t fileSystem_openFile(uint8_t * fileName, uint8_t * extension){
    // Calculate size of a FAT entry
    uint32_t sizeOfFatEntry = sizeof(Fat16Entry_t);
    // Iterate through entries in the root directory, until file is found

    uint8_t buffer[STORAGE_SECTOR_SIZE];
    readSector(buffer, fileSystemState.currentDirectoryAddressInBytes);
    // Local variable to store the currently read entry
    Fat16Entry_t currentEntry;
    volatile uint32_t i = 0;
    for(i=0; i < fileSystemState.maximumNumberOfEntriesInRoot; i+=sizeOfFatEntry){
        memcpy((void*)&currentEntry, buffer+i, sizeof(currentEntry));
        if(compareFileNames(currentEntry.filename, currentEntry.ext, fileName, extension)){
            // File found. Check if it is a directory. If yes, change the current directory global variable.
            if(currentEntry.attributes == FAT16_DIRECTORY_ENTRY){
                // Entry is a directory
                fileSystemState.currentDirectoryAddressInBytes = getClusterAdressInBytes(currentEntry.starting_cluster);
            }

            // File found. Note cluster and return file size.
            fileSystemState.currentClusterBeingRead = currentEntry.starting_cluster;
            fileSystemState.bytesRemainingInFile = currentEntry.file_size;
            return currentEntry.file_size;
        }
    }

    // Nothing found. Return 0.
    return 0;
}

uint32_t fileSystem_readBytes(uint32_t bytesToRead, uint8_t * buffer){
    if(fileSystemState.currentClusterBeingRead==INVALID_CLUSTER){
        // User probably forgot to open any file.
        return 0;
    }

    // Get current cluster
    uint16_t currentCluster = fileSystemState.currentClusterBeingRead;

    // Tracks how many clusters have been read
    uint16_t clustersRead = 0;

    uint32_t clusterSizeInBytes = fileSystemState.sectorsPerCluster*STORAGE_SECTOR_SIZE;

    // Local buffer in which each sector is stored
    uint8_t localBuffer[STORAGE_SECTOR_SIZE];

    volatile uint32_t i = 0;
    for(i = 0; i < bytesToRead; i++){
        // if sector beginning, read sector
        if(i%STORAGE_SECTOR_SIZE==0){
            // Check if next cluster needs to be read
            if(i/(clusterSizeInBytes + (clusterSizeInBytes*clustersRead))>=1){
                currentCluster = getNextClusterToRead(currentCluster);
                clustersRead++;
            }
            // Read a sector
            readSector(localBuffer, getClusterAdressInBytes(currentCluster)+(i%clusterSizeInBytes));
        }

        // Local buffer is only 512 bytes, so cycle through it using %.
        buffer[i] = localBuffer[i%STORAGE_SECTOR_SIZE];
    }

    return 0;
}

void fileSystem_openRootDir(void){
    fileSystemState.currentDirectoryAddressInBytes = fileSystemState.rootDirectoryAddress;
    // Set to invalid cluster
    fileSystemState.currentClusterBeingRead = INVALID_CLUSTER;
}
