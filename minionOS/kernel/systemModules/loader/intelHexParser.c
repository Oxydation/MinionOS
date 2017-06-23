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

    uint32_t entryLength;
    uint8_t* entryData;

    while (currentByte < length)
    {
        if (data[currentByte] == ':')
        {
            if (currentByte > 0) {
                entryLength = currentByte - startIndex - 2;
                entryData = &data[startIndex];
                entries[currentEntry++] = intelHexParser_parseIntelHexEntry(entryData, entryLength);
            }
            startIndex = currentByte;
        }
        /*
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
        */
        currentByte++;
    }

    entryLength = currentByte - startIndex - 2;
    entryData = &data[startIndex];
    entries[currentEntry++] = intelHexParser_parseIntelHexEntry(entryData, entryLength);

    set.nrOfRecords = nrOfEntries;
    set.entries = entries;
    return set;
}

static IntelHexEntry_t intelHexParser_parseIntelHexEntry(uint8_t* data, uint32_t length)
{
    IntelHexEntry_t entry;
    //entry.dataLength = hexParser_parseHex8ToInt(data + 1);
    //entry.dataLengthInBytes = entry.dataLength * 2;         // one data byte in intel hex is equal to two bytes
    //entry.address = hexParser_parseHex16ToInt(data + 3);
    //entry.recordType = hexParser_parseHex8ToInt(data + 7);
    //entry.data = data + 9;
    //entry.checksum = hexParser_parseHex8ToInt(data + 9 + entry.dataLengthInBytes);
    entry.dataLength = hexIntParser_parseIntToHex8(data + 1);
    entry.dataLengthInBytes = entry.dataLength * 2;
    entry.address = hexIntParser_parseIntToHex16(data + 3);
    entry.recordType = hexIntParser_parseIntToHex8(data + 7);
    entry.data = data + 9;
    entry.checksum = hexIntParser_parseIntToHex8(data + 9 + entry.dataLengthInBytes);
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

void intelHexParser_parseFileToHex(uint8_t* buffer, uint32_t fileLength)
{
    int32_t counter = 0;

    while (counter < fileLength)
    {
        buffer[counter] = hexIntParser_parseHex4ToInt(buffer[counter]);
        counter++;
    }
}

void intelHexParser_parseIntelHexDataToInt32(uint8_t* sourceBuffer, uint32_t* targetBuffer, uint32_t dataLength)
{
    uint32_t sourceBufferCounter = 0;
    uint32_t targetBufferCounter = 0;

    while (sourceBufferCounter < dataLength)
    {
        if (sourceBufferCounter + 8 <= dataLength)
        {
            targetBuffer[targetBufferCounter] = hexIntParser_parseIntToHex32(sourceBuffer + sourceBufferCounter);
        }
        else
        {
            uint32_t remainingBytes = dataLength - sourceBufferCounter;
            uint32_t result = 0;
            if (remainingBytes == 6)
            {
                result = hexIntParser_parseIntToHex24(sourceBuffer + sourceBufferCounter);
            }
            else if (remainingBytes == 4)
            {
                result = hexIntParser_parseIntToHex16(sourceBuffer + sourceBufferCounter);
            }
            else if (remainingBytes == 2)
            {
                result = hexIntParser_parseIntToHex8(sourceBuffer + sourceBufferCounter);
            }

            result = result << (32 - remainingBytes*4);
            targetBuffer[targetBufferCounter] = result;
        }
        sourceBufferCounter += 8;
        targetBufferCounter++;
    }
}
