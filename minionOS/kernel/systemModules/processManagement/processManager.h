/*
 * processManager.h
 *
 *  Created on: 17 Apr 2017
 *      Author: Mathias
 */

#ifndef KERNEL_SYSTEMMODULES_PROCESSMANAGEMENT_PROCESSMANAGER_H_
#define KERNEL_SYSTEMMODULES_PROCESSMANAGEMENT_PROCESSMANAGER_H_

#include "../scheduler/scheduler.h"
#include "../mmu/mmu.h"

#define STACK_SIZE  0x40000     /* 256 KB */

int8_t processManager_loadProcess(uint32_t physicalStartAddress, uint32_t nrOfNeededBytes);
void processManager_killProcess(ProcessId_t processId);
void processManager_terminateCurrentProcess(PCB_t* pcb);

#endif /* KERNEL_SYSTEMMODULES_PROCESSMANAGEMENT_PROCESSMANAGER_H_ */
