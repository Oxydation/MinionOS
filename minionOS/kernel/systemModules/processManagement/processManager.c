/*
 * processManager.c
 *
 *  Created on: 17 Apr 2017
 *      Author: Mathias
 */

#include "processManager.h"

PCB_t* processManager_loadProcess(uint32_t startAddress, uint32_t stackPointer){
    return scheduler_startProcess(startAddress, stackPointer, 0x60000110);
}

void processManager_killProcess(ProcessId_t processId) {
    mmu_killProcess(processId);
    scheduler_stopProcess(processId);
}

uint8_t processManager_getNextProcessId(void) {
    return scheduler_getNextProcessId();
}
