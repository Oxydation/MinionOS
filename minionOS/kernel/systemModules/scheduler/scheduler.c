/*
 * scheduler.c
 *
 *  Created on: 13 Apr 2017
 *      Author: Mathias
 */

#include "kernel/systemModules/scheduler/scheduler.h"

#define SCHEDULER_INTERVAL_MS 50       //50

PCB_t g_processes[MAX_ALLOWED_PROCESSES + 1];
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
static ProcessId_t scheduler_getNextProcessId(void);
static void scheduler_terminateCurrentProcess(void);

void (*fPointer) (void);

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

PCB_t* scheduler_startProcess(uint32_t startAddress, uint32_t stackPointer, uint32_t cpsr)
{
    ProcessId_t processId = scheduler_getNextProcessId();

    fPointer = &scheduler_terminateCurrentProcess;

    // 1. Step: Create PCB
    PCB_t newProcessPcb = { .lr = ((uint32_t)startAddress + 0x4), .processId = processId,
                            .registers.R13 = stackPointer, .registers.R14 = (uint32_t)fPointer, //.registers.R14 = startAddress, //registers.R14 = (uint32_t*)fPointer,
                            .cpsr = cpsr, .status = WAITING };
    // 2. Step store into pcb array
    g_processes[processId] = newProcessPcb;

    // 3. Load process into ready queue
    addReadyProcess(&g_processes[processId]);

    return &g_processes[processId];
}

static void scheduler_terminateCurrentProcess(void) {
    g_currentProcess->status = DEAD;
    mmu_killProcess(g_currentProcess->processId);

    while (1) {

    }
}

void scheduler_stopProcess(ProcessId_t processId) {

    PCB_t* process = &g_processes[processId];

    if (g_currentProcess->processId == processId)
    {
        // Load other process
        if (g_queueReady.size != 0) {
            g_currentProcess = getNextProcess();
            g_currentProcess->status = RUNNING;
            mmu_switchProcess(g_currentProcess);
            asm_loadContext(g_currentProcess);
        }
    }
    else
    {
        pcbQueue_removePcbNode(&g_queueReady, process);
        pcbQueue_removePcbNode(&g_queueBlocked, process);
    }

    g_processes[processId].processId = 0;
}

static void handleSchedulerTick(PCB_t * currentPcb)
{
    if (g_queueReady.size > 0 && g_currentProcess == NULL)
    {
        g_currentProcess = getNextProcess();
        g_currentProcess->status = RUNNING;
        copyPcb(g_currentProcess, currentPcb);
        currentPcb->processId = g_currentProcess->processId;
        mmu_switchProcess(currentPcb);
    }
    else if (g_queueReady.size > 0 && g_currentProcess != NULL)
    {
        if (g_currentProcess->status != DEAD) {
            // Save old process and add to ready queue
            g_currentProcess->status = WAITING;
            copyPcb(currentPcb, g_currentProcess);
            addReadyProcess(g_currentProcess);
        } else {
            g_currentProcess->processId = 0;
        }

        // Load new process
        g_currentProcess = getNextProcess();
        g_currentProcess->status = RUNNING;
        copyPcb(g_currentProcess, currentPcb);
        currentPcb->processId = g_currentProcess->processId;
        mmu_switchProcess(currentPcb);
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

static PCB_t * removeReadyProcess(void)
{
    PCB_t * removedProcess = ((PcbNode_t*) queue_front(&g_queueReady))->data;
    queue_remove(&g_queueReady);
    return removedProcess;
}

static void addBlockedProcess(PCB_t * process)
{
    queue_insert(&g_queueBlocked, pcbQueue_createPcbNode(process));
}

static PCB_t * removeBlockedProcess(void)
{
    PCB_t * result = ((PcbNode_t*) queue_front(&g_queueBlocked))->data;
    queue_remove(&g_queueBlocked);
    return result;
}

ProcessId_t scheduler_getNextProcessId(void) {
    int i;
    for (i = 1; i <= MAX_ALLOWED_PROCESSES; i++) {
       if (g_processes[i].processId == 0) {
           return i;
       }
    }
    return -1;
}
