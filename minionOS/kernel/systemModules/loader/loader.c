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


