/*
 * pcbQueue.h
 *
 *  Created on: 17 Apr 2017
 *      Author: Mathias
 */

#ifndef KERNEL_SYSTEMMODULES_SCHEDULER_PCBQUEUE_PCBQUEUE_H_
#define KERNEL_SYSTEMMODULES_SCHEDULER_PCBQUEUE_PCBQUEUE_H_

#include "global/queue/queue.h"
#include "kernel/systemModules/processManagement/contextSwitch.h"

typedef struct PcbNode
{
    PCB_t * data;
    struct PcbNode * pNext;
} PcbNode_t;

PcbNode_t* pcbQueue_createPcbNode(PCB_t * pcb);

#endif /* KERNEL_SYSTEMMODULES_SCHEDULER_PCBQUEUE_PCBQUEUE_H_ */
