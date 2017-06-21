/*
 * processManager.c
 *
 *  Created on: 17 Apr 2017
 *      Author: Mathias
 */

#include "processManager.h"

static uint32_t calcStackPointer(uint32_t virtualStartAddress, uint32_t nrOfNeededBytes);

int8_t processManager_loadProcess(uint32_t physicalStartAddress, uint32_t nrOfNeededBytes){
    uint32_t stackPointer = calcStackPointer(VIRTUAL_PROCESS_START_ADDRESS, nrOfNeededBytes);
    PCB_t* pPcb = scheduler_startProcess(VIRTUAL_PROCESS_START_ADDRESS, stackPointer, 0x60000110);
    uint32_t nrOfNeededBytesWithStack = stackPointer - VIRTUAL_PROCESS_START_ADDRESS;
    return mmu_initProcess(physicalStartAddress, VIRTUAL_PROCESS_START_ADDRESS, nrOfNeededBytesWithStack, pPcb);
}

uint32_t* processManager_getPhysicalMemoryForProcess(uint32_t nrOfNeededBytes) {
    uint32_t stackPointer = calcStackPointer(VIRTUAL_PROCESS_START_ADDRESS, nrOfNeededBytes);
    uint32_t nrOfNeededBytesWithStack = stackPointer - VIRTUAL_PROCESS_START_ADDRESS;
    uint32_t* pAddress = mmu_getPhysicalMemoryForProcess(nrOfNeededBytesWithStack);
    mmu_mapRegionDirectly((uint32_t)pAddress, nrOfNeededBytesWithStack, SECTION);
    return pAddress;
}

void processManager_killProcess(ProcessId_t processId) {
    mmu_killProcess(processId);
    scheduler_stopProcess(processId);
}

void processManager_terminateCurrentProcess(PCB_t* pcb) {
    scheduler_terminateCurrentProcess(pcb);
}

static uint32_t calcStackPointer(uint32_t virtualStartAddress, uint32_t nrOfNeededBytes) {
    return virtualStartAddress + nrOfNeededBytes + STACK_SIZE;
}
