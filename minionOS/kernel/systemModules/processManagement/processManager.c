/*
 * processManager.c
 *
 *  Created on: 17 Apr 2017
 *      Author: Mathias
 */

#include "processManager.h"

int8_t processManager_loadProcess(uint32_t physicalStartAddress, uint32_t nrOfNeededBytes){
    PCB_t* pPcb = scheduler_startProcess(VIRTUAL_PROCESS_START_ADDRESS, VIRTUAL_PROCESS_START_ADDRESS + 0x10000, 0x60000110);
    return mmu_initProcess(physicalStartAddress, VIRTUAL_PROCESS_START_ADDRESS, nrOfNeededBytes, pPcb);
}

void processManager_killProcess(ProcessId_t processId) {
    mmu_killProcess(processId);
    scheduler_stopProcess(processId);
}

uint8_t processManager_getNextProcessId(void) {
    return scheduler_getNextProcessId();
}
