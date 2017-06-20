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
    uint8_t readBuffer1[1024];
    uint8_t readBuffer2[1024];
    uint8_t readBuffer3[1024];
    uint32_t nrOfBytesInFile = 0;
    int32_t nrOfBytesRead;
    uint32_t* pBuffer = (uint32_t*)&buffer;

    int i = 0;
    do {
        if (i % 3 == 0) {
            nrOfBytesRead = sysCalls_readFile(fileHandle, readBuffer1, 1024);
            if(nrOfBytesRead > 0) {
                memcpy(pBuffer, readBuffer1, nrOfBytesRead);
            }
        } else if (i % 3 == 1) {
            nrOfBytesRead = sysCalls_readFile(fileHandle, readBuffer2, 1024);
            if (nrOfBytesRead > 0) {
                memcpy(pBuffer, readBuffer2, nrOfBytesRead);
            }
        } else if (i % 3 == 2) {
            nrOfBytesRead = sysCalls_readFile(fileHandle, readBuffer3, 1024);
            if (nrOfBytesRead > 1024) {
                nrOfBytesRead= 1024;
            }
            if (nrOfBytesRead > 0) {
                memcpy(pBuffer, readBuffer3, nrOfBytesRead);
            }
        }
        nrOfBytesInFile += nrOfBytesRead;
        i++;
        pBuffer = (uint32_t*)((uint32_t)pBuffer + i*1024);
    } while (i < BUFFER_SIZE && nrOfBytesRead > 0);

    return LOAD_PROCESS_OK;
}


