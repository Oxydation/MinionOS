/*
 * scheduler.c
 *
 *  Created on: 13 Apr 2017
 *      Author: Mathias
 */
#include <stdio.h>
#include "kernel/systemModules/scheduler/scheduler.h"
#include "kernel/hal/timer/systemTimer.h"
#include "global/queue/queue.h"
#include "kernel/systemModules/scheduler/pcbQueue/pcbQueue.h"

#define SCHEDULER_INTERVAL_MS 50

PCB_t g_processes[MAX_ALLOWED_PROCESSES];
ProcessId_t nextProcessId = 1;
PCB_t * g_currentProcess;
Queue_t g_queueBlocked;
Queue_t g_queueReady;

SubscriptionId_t g_systemTimerId;

static void handleSchedulerTick(PCB_t * currentPcb);
static void initSchedulerTimer(uint32_t interval_ms);
static void addReadyProcess(PCB_t * process);
static PCB_t * removeReadyProcess();
static void addBlockedProcess(PCB_t * process);
static PCB_t * removeBlockedProcess();
static PCB_t * getNextProcess(void);

void scheduler_init(void)
{
    initSchedulerTimer(SCHEDULER_INTERVAL_MS);
    g_queueBlocked = queue_create();
    g_queueReady = queue_create();
}

void scheduler_start(void)
{
    systemTimer_enableSubscription(g_systemTimerId);
}

void scheduler_stop(void)
{
    systemTimer_disableSubscription(g_systemTimerId);
}

void scheduler_startProcess(uint32_t startAddress, uint32_t stackPointer,
                            uint32_t cpsr)
{
    // 1. Step: Create PCB
    PCB_t newProcessPcb = { .lr = startAddress, .processId = nextProcessId,
                            .registers.R13 = stackPointer, .cpsr = cpsr };
    // 2. Step store into pcb array
    g_processes[nextProcessId] = newProcessPcb;

    // 3. Load process into ready queue
    addReadyProcess(&g_processes[nextProcessId++]);
}

static void handleSchedulerTick(PCB_t * currentPcb)
{
    if (g_queueReady.size > 0 && g_currentProcess == NULL)
    {
        g_currentProcess = getNextProcess();
        copyPcb(g_currentProcess, currentPcb);
        currentPcb->processId = g_currentProcess->processId;
    }
    else if (g_queueReady.size > 0 && g_currentProcess != NULL)
    {
        // Save old process and add to ready queue
        copyPcb(currentPcb, g_currentProcess);
        addReadyProcess(g_currentProcess);

        // Load new process
        g_currentProcess = getNextProcess();
        copyPcb(g_currentProcess, currentPcb);
        currentPcb->processId = g_currentProcess->processId;
    }
    else if (g_queueReady.size == 0)
    {
        // do nothing
    }
}

static PCB_t * getNextProcess()
{
    return removeReadyProcess();
}

static void initSchedulerTimer(uint32_t interval_ms)
{
    g_systemTimerId = systemTimer_subscribeCallback(interval_ms,
                                                    &handleSchedulerTick);
    systemTimer_disableSubscription(g_systemTimerId);
}

/** Queue functions **/
static void addReadyProcess(PCB_t * process)
{
    queue_insert(&g_queueReady, pcbQueue_createPcbNode(process));
}

static PCB_t * removeReadyProcess()
{
    PCB_t * removedProcess = ((PcbNode_t*) queue_front(&g_queueReady))->data;
    queue_remove(&g_queueReady);
    return removedProcess;
}

static void addBlockedProcess(PCB_t * process)
{
    queue_insert(&g_queueBlocked, pcbQueue_createPcbNode(process));
}

static PCB_t * removeBlockedProcess()
{
    PCB_t * result = ((PcbNode_t*) queue_front(&g_queueBlocked))->data;
    queue_remove(&g_queueBlocked);
    return result;
}
