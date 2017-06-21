/*
 * scheduler.h
 *
 *  Created on: 13 Apr 2017
 *      Author: Mathias
 */

#ifndef KERNEL_SYSTEMMODULES_SCHEDULER_SCHEDULER_H_
#define KERNEL_SYSTEMMODULES_SCHEDULER_SCHEDULER_H_

#define MAX_ALLOWED_PROCESSES 16

#include <stdio.h>
#include "kernel/systemModules/processManagement/contextSwitch.h"
#include "kernel/hal/timer/systemTimer.h"
#include "global/queue/queue.h"
#include "kernel/systemModules/scheduler/pcbQueue/pcbQueue.h"
#include "kernel/systemModules/mmu/mmu.h"

void (*idleProcessPointer) (void);

void scheduler_init(void);
void scheduler_start(void);
void scheduler_stop(void);

PCB_t * scheduler_getCurrentProcess(void);

PCB_t* scheduler_startProcess(uint32_t startAddress, uint32_t stackPointer, uint32_t cpsr);
void scheduler_stopProcess(ProcessId_t processId);

void scheduler_blockProcess(ProcessId_t processId);
void scheduler_unblockProcess(ProcessId_t processId);

#endif /* KERNEL_SYSTEMMODULES_SCHEDULER_SCHEDULER_H_ */
