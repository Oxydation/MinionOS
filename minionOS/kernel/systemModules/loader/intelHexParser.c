/*
 * intelHexParser.c
 *
 *  Created on: 22.06.2017
 *      Author: sabrinarosmann
 */

#include "kernel/systemModules/loader/intelHexParser.h"

static IntelHexEntry_t intelHexParser_parseIntelHexEntry(uint8_t data[], uint32_t length);

IntelHexSet_t intelHexParser_parseIntelHexData(uint8_t data[], uint32_t length, IntelHexEntry_t entries[])
{
    IntelHexSet_t set;
    uint32_t currentByte = 0;
    uint32_t startIndex = 0;
    uint8_t valid = 1;

    uint32_t nrOfEntries = intelHexParser_getNumberOfIntelHexEntries(data, length);
    uint32_t currentEntry = 0;

    while (currentByte < length)
    {
        if (data[currentByte] == ':')
        {
            startIndex = currentByte;
        }
        else if ((data[currentByte] == '\x0a' || data[currentByte] == '\x0d') && valid)
        {
            uint32_t entryLength = currentByte - startIndex;
            uint8_t* entryData = &data[startIndex];
            entries[currentEntry++] = intelHexParser_parseIntelHexEntry(entryData, entryLength);
            valid = 0;
        }
        else if (valid == 0)
        {
            valid = 1;
        }
        currentByte++;
    }

    set.nrOfRecords = nrOfEntries;
    set.entries = entries;
    return set;
}

static IntelHexEntry_t intelHexParser_parseIntelHexEntry(uint8_t* data, uint32_t length)
{
    IntelHexEntry_t entry;
    entry.dataLength = hexParser_parseHex8ToInt(data + 1);
    entry.dataLengthInBytes = entry.dataLength * 2;         // one data byte in intel hex is equal to two bytes
    entry.address = hexParser_parseHex16ToInt(data + 3);
    entry.recordType = hexParser_parseHex8ToInt(data + 7);
    entry.data = data + 9;
    entry.checksum = hexParser_parseHex8ToInt(data + 9 + entry.dataLengthInBytes);
    return entry;
}

uint32_t intelHexParser_getNumberOfIntelHexEntries(uint8_t* data, uint32_t length)
{
    uint32_t currentByte = 0;
    uint32_t nrOfEntries = 0;

    while (currentByte < length)
    {
        if (data[currentByte] == ':')
        {
            nrOfEntries++;
        }
        currentByte++;
    }
    return nrOfEntries;
}

