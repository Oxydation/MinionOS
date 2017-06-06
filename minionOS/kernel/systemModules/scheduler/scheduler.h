/*
 * scheduler.h
 *
 *  Created on: 13 Apr 2017
 *      Author: Mathias
 */

#ifndef KERNEL_SYSTEMMODULES_SCHEDULER_SCHEDULER_H_
#define KERNEL_SYSTEMMODULES_SCHEDULER_SCHEDULER_H_

#include "kernel/systemModules/processManagement/contextSwitch.h"

#define MAX_ALLOWED_PROCESSES 16

void scheduler_init(void);
void scheduler_start(void);
void scheduler_stop(void);

PCB_t * scheduler_getCurrentProcess(void);

void scheduler_startProcess(uint32_t startAddress, uint32_t stackPointer, uint32_t cpsr);
void scheduler_stopProcess(ProcessId_t processId);

void scheduler_blockProcess(ProcessId_t processId);
void scheduler_unblockProcess(ProcessId_t processId);

ProcessId_t scheduler_getNexProcessId(void);

#endif /* KERNEL_SYSTEMMODULES_SCHEDULER_SCHEDULER_H_ */
