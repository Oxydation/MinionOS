/*
 * loader.c
 *
 *  Created on: 19.06.2017
 *      Author: sabrinarosmann
 */

#include "kernel/systemModules/loader/loader.h"

int8_t loader_loadProcess(const char* fileName) {

    int32_t fileHandle = sysCalls_openFile(fileName);

    if (fileHandle < 0) {
        return NOT_ABLE_TO_LOAD_FILE;
    }

    uint8_t buffer[BUFFER_SIZE];
    uint8_t readBuffer[1024];
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
        pBuffer = (uint32_t*)((uint32_t)pBuffer + i*1024);
    } while (i < BUFFER_SIZE && nrOfBytesRead > 0);

    // get physical memory for executable and copy it there
    uint32_t* pAddress = processManager_getPhysicalMemoryForProcess(nrOfBytesInFile);
    memcpy(pAddress, buffer, nrOfBytesInFile);

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
            uint32_t address = hexParser_parseHex16ToInt(entry->data);
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
            uint32_t address = hexParser_parseHex16ToInt(entry->data);
            baseAddress = address << 16;

            if (baseAddress >= 0x40000000)
            {
                break;
            }
        }
        else if (entry->recordType == DATA_RECORD)
        {
            uint32_t address = baseAddress + entry->address;
            uint32_t addressToCopy = pAddress + address - VIRTUAL_PROCESS_START_ADDRESS;
            memcpy((uint32_t*)addressToCopy, entry->data, entry->dataLengthInBytes);
        }
        counter++;
    }
}

