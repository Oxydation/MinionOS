/*
 * loader.c
 *
 *  Created on: 19.06.2017
 *      Author: sabrinarosmann
 */

#include "kernel/systemModules/loader/loader.h"

static void copyFileToMemory(uint32_t pAddress, IntelHexSet_t* set);
static uint32_t getNrOfBytesNecessary(IntelHexSet_t* intelHexSet);

int8_t loader_loadProcess(const char* fileName, FileType_t fileType) {

    int32_t fileHandle = sysCalls_openFile(fileName);

    if (fileHandle < 0) {
        return NOT_ABLE_TO_LOAD_FILE;
    }

    uint8_t buffer[BUFFER_SIZE] = {0};
    uint8_t readBuffer[1024] = {0};
    uint32_t nrOfBytesInFile = 0;
    int32_t nrOfBytesRead;
    uint32_t* pBuffer = (uint32_t*)&buffer;

    int i = 0;
    do {
        nrOfBytesRead = sysCalls_readFile(fileHandle, readBuffer, 1024);
        if (nrOfBytesRead > 0) {
            memcpy(pBuffer, readBuffer, nrOfBytesRead);
        }
        nrOfBytesInFile += nrOfBytesRead;
        i++;
        pBuffer = (uint32_t*)((uint8_t*)pBuffer + 1024);
    } while (i < BUFFER_SIZE && nrOfBytesRead > 0);

    uint32_t* pAddress;

    if (fileType == INTEL_HEX)
    {
        intelHexParser_parseFileToHex(buffer, nrOfBytesInFile);

        uint32_t nrOfEntries = intelHexParser_getNumberOfIntelHexEntries(buffer, nrOfBytesInFile);
        IntelHexEntry_t entries[nrOfEntries];

        IntelHexSet_t data = intelHexParser_parseIntelHexData(buffer, nrOfBytesInFile, entries);

        uint32_t nrOfBytesNeeded = getNrOfBytesNecessary(&data);

        // get physical memory for executable and copy it there
        pAddress = processManager_getPhysicalMemoryForProcess(nrOfBytesNeeded);
        copyFileToMemory((uint32_t)pAddress, &data);
    }
    else if (fileType == ELF)
    {
        elfParser_loadElfFile(buffer);
    }

    processManager_loadProcess((uint32_t)pAddress, nrOfBytesInFile);

    return LOAD_PROCESS_OK;
}


static uint32_t getNrOfBytesNecessary(IntelHexSet_t* intelHexSet)
{
    uint32_t startAddress = 0xFFFFFFFF;
    uint32_t endAddress = 0x0000000;
    uint32_t counter = 0;
    uint32_t baseAddress = 0;
    uint16_t nrOfBytesNeededAfterEndAddress = 0;

    while (counter < intelHexSet->nrOfRecords)
    {
        IntelHexEntry_t* entry = &intelHexSet->entries[counter];
        if (entry->recordType == EXTENDED_LINEAR_ADDRESS_RECORD)
        {
            uint32_t address = hexIntParser_parseHex16ToInt(entry->data);
            baseAddress = address << 16;

            if (baseAddress >= 0x40000000)
            {
                break;
            }
        }
        else if (entry->recordType == DATA_RECORD)
        {
            uint32_t address = baseAddress + entry->address;

            if (address < startAddress)
            {
                startAddress = address;
            }

            if (address > endAddress)
            {
                endAddress = address;
                nrOfBytesNeededAfterEndAddress = entry->dataLengthInBytes;
            }
        }
        counter++;
    }

    uint32_t nrOfBytesNeeded = endAddress - startAddress + nrOfBytesNeededAfterEndAddress;
    return nrOfBytesNeeded;
}

static void copyFileToMemory(uint32_t pAddress, IntelHexSet_t* set)
{
    uint32_t counter = 0;
    uint32_t baseAddress = 0;

    while (counter < set->nrOfRecords)
    {
        IntelHexEntry_t* entry = &set->entries[counter];
        if (entry->recordType == EXTENDED_LINEAR_ADDRESS_RECORD)
        {
            uint32_t address = hexIntParser_parseHex16ToInt(entry->data);
            baseAddress = address << 16;

            if (baseAddress >= 0x40000000)
            {
                break;
            }
        }
        else if (entry->recordType == DATA_RECORD)
        {
            uint32_t address = baseAddress + entry->address;
            uint32_t addressToCopy = pAddress + address - VIRTUAL_MEMORY_START_ADDRESS;
            uint32_t lengthOfDataArray = entry->dataLengthInBytes/8;
            uint32_t remainder = entry->dataLengthInBytes % 8;
            if (remainder > 0)
            {
                lengthOfDataArray++;
            }
            //uint32_t data[lengthOfDataArray];
            //intelHexParser_parseIntelHexDataToInt32(entry->data, data, entry->dataLengthInBytes);
            memcpy((uint32_t*)addressToCopy, entry->data, entry->dataLengthInBytes);
        }
        counter++;
    }
}

