/*
 * processManager.h
 *
 *  Created on: 17 Apr 2017
 *      Author: Mathias
 */

#ifndef KERNEL_SYSTEMMODULES_PROCESSMANAGEMENT_PROCESSMANAGER_H_
#define KERNEL_SYSTEMMODULES_PROCESSMANAGEMENT_PROCESSMANAGER_H_

#include "../scheduler/scheduler.h"

PCB_t processManager_loadProcess(uint32_t startAddress, uint32_t stackPointer);
void processManager_startFirstProcess(void);
uint8_t processManager_getNextProcessId(void);

#endif /* KERNEL_SYSTEMMODULES_PROCESSMANAGEMENT_PROCESSMANAGER_H_ */
