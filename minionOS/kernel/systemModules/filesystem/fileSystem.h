/*
 * fileSystem.h
 *
 *  Created on: 20.05.2017
 *      Author: benimurza
 */

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <inttypes.h>

#define FAT_16_LBA_PARTITION_TYPE 14


// Partition table
typedef struct{
    uint8_t isBootable;
	uint8_t partitionStartCHS[3];
	uint8_t partitionType;
	uint8_t partitionEndCHS[3];
	uint32_t relativeOffsetPartitionSectors;
	uint32_t partitionSizeInSectors;
}__attribute((packed)) PartitionTable_t;


// FAT16 boot sector
typedef struct{
    uint8_t jumpInstruction80x86[3];
    uint8_t oemName[8];
    uint16_t sectorSizeInBytes;
    uint8_t numberOfSectorsPerCluster;
    uint16_t reservedSectors;
    uint8_t numberOfFATs;
    uint8_t numberOfEntriesInRoot[2]; // Because of alignment errors
    uint16_t numSectorsOnDiskForLTE32MB;
    uint8_t mediaDescriptor;
    uint16_t numSectorsPerFATTable;
    uint16_t numSectorsPerTrackCHS;
    uint16_t numHeadsCHS;
    uint32_t numHiddenSectorsBeforeBoot;
    uint32_t numSectorsOnDiskForGT32MB;

    /* FAT 32 missing, only FAT16 supported */

    uint8_t driveNumber;
    uint8_t currentHead;
    uint8_t bootSignature;
    uint32_t volumeId;
    uint8_t volumeLabel[11];
    uint8_t fileSystemType[8];
    uint8_t bootCode[448];
    uint16_t bootSectorSignature; // 0xAA55 instead of 0x55AA because of byte ordering
} __attribute((packed)) FAT16BootSector_t;

// File structure
typedef struct {
    uint8_t filename[8];
    uint8_t ext[3];
    uint8_t attributes;
    uint8_t reserved[10];
    uint16_t modify_time;
    uint16_t modify_date;
    uint16_t starting_cluster;
    uint32_t file_size;
} __attribute((packed)) Fat16Entry_t;

// File system types
typedef enum{
    UNUSED = 0,
    FAT12 = 1,
    FAT16_LTE_32_MB = 4,
    EXTENDED_PARTITION = 5,
    FAT16_GT_32_MB = 6,
    FAT32_LTE_2_GB = 11,
    FAT32_LTE_2_GB_LBA = 12,
    FAT16_GT_32_MB_LBA = 14,
    EXTENDED_PARTITION_LBA = 15
} FileSystemTypes_t;

/*
 * Initialize file system - read most important information about the file system in a module-global struct.
 */
uint32_t filesystem_Initialize(void);

/*
 * Open file (could also be a directory!). File name MUST be 8 characters in length. Unused characters must be padded with space.
 * Extension must be 3 characters in length. Unused characters must be padded with space. In case of a directory, extension should contain 3 spaces.
 * Returns: size of file if file found, 0 otherwise (or if a directory was opened).
 */
uint32_t fileSystem_openFile(uint8_t * fileName, uint8_t * extension);

/*
 * In case user is currently in a subdirectory, and wants to go back to root.
 */
void fileSystem_openRootDir(void);

/*
 * Read amount of bytes from previously opened file. Buffer needs to be as large as the number of bytes to read.
 * Returns number of bytes read on success, or 0 otherwise. It is possible that the amount of bytes read is smaller
 * that the amount specified in the input parameter - this happens if the end of file has been reached.
 */
uint32_t fileSystem_readBytes(uint32_t bytesToRead, uint8_t * buffer);


#endif
