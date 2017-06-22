/*
 * intelHexParser.h
 *
 *  Created on: 22.06.2017
 *      Author: sabrinarosmann
 */

#ifndef KERNEL_SYSTEMMODULES_LOADER_INTELHEXPARSER_H_
#define KERNEL_SYSTEMMODULES_LOADER_INTELHEXPARSER_H_

#include <inttypes.h>
#include "kernel/common/hexIntParser.h"

#define DATA_RECORD                         0x00
#define END_OF_FILE_RECORD                  0x01
#define EXTENDED_SEGMENT_ADDRESS_RECORD     0x02
#define EXTENDED_LINEAR_ADDRESS_RECORD      0x04
#define START_LINEAR_ADDRESS_RECORD         0x05

typedef struct {
    uint8_t dataLength;
    uint8_t checksum;
    uint16_t dataLengthInBytes;
    uint32_t address;
    uint16_t recordType;
    uint8_t* data;
} IntelHexEntry_t;

typedef struct {
    uint16_t nrOfRecords;
    IntelHexEntry_t* entries;
} IntelHexSet_t;

IntelHexSet_t intelHexParser_parseIntelHexData(uint8_t data[], uint32_t length, IntelHexEntry_t entries[]);
uint32_t intelHexParser_getNumberOfIntelHexEntries(uint8_t* data, uint32_t length);

#endif /* KERNEL_SYSTEMMODULES_LOADER_INTELHEXPARSER_H_ */
