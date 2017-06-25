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
Queue_t g_queueReady;

SubscriptionId_t g_systemTimerId;

static void handleSchedulerTick(PCB_t * currentPcb);
static void initSchedulerTimer(uint32_t interval_ms);
static void initIdleProcess(void);
static void addReadyProcess(PCB_t * process);
static PCB_t * removeReadyProcess();
static PCB_t * getNextProcess(void);
static ProcessId_t scheduler_getNextProcessId(void);
static PCB_t* getIdleProcess(void);
static void idleProcess(void);

uint32_t counter = 0;

void scheduler_init(void)
{
    initSchedulerTimer(SCHEDULER_INTERVAL_MS);
    g_queueReady = queue_create();
    initIdleProcess();
}

void scheduler_start(void)
{
    systemTimer_enableSubscription(g_systemTimerId);
}

void scheduler_stop(void)
{
    systemTimer_disableSubscription(g_systemTimerId);
}

static void initIdleProcess(void) {

    PCB_t idleProcessPcb = { .lr = ((uint32_t)(&idleProcess) + 0x4), .processId = 0,
                             .registers.R14 = (uint32_t)&idleProcess, .status = WAITING };

    g_processes[0] = idleProcessPcb;
}

PCB_t* scheduler_startProcess(uint32_t startAddress, uint32_t stackPointer, uint32_t cpsr)
{
    ProcessId_t processId = scheduler_getNextProcessId();

    // 1. Step: Create PCB
    PCB_t newProcessPcb = { .lr = ((uint32_t)startAddress + 0x4), .processId = processId,
                            .registers.R13 = stackPointer, .registers.R14 = NULLPOINTER,
                            .cpsr = cpsr, .status = WAITING };
    // 2. Step store into pcb array
    g_processes[processId] = newProcessPcb;

    // 3. Load process into ready queue
    addReadyProcess(&g_processes[processId]);

    return &g_processes[processId];
}

void scheduler_terminateCurrentProcess(PCB_t* pcb) {
    g_currentProcess->status = DEAD;
    mmu_killProcess(g_currentProcess->processId);
    PCB_t* idleProcess = getIdleProcess();
    pcb->cpsr = idleProcess->cpsr;
    pcb->lr = idleProcess->lr;
    pcb->processId = idleProcess->processId;
    pcb->registers = idleProcess->registers;
    pcb->status = idleProcess->status;
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
    }

    g_processes[processId].processId = 0;
}

PCB_t * scheduler_getCurrentProcess(void) {
    return g_currentProcess;
}

void scheduler_prepareSwitchToIdleProcess() {
    _call_swi(SWITCH_TO_IDLE_SWI_NUMBER);
}

void scheduler_switchToIdleProcess(PCB_t* pcb) {
    PCB_t* idleProcess = getIdleProcess();
    pcb->cpsr = idleProcess->cpsr;
    pcb->lr = idleProcess->lr;
    pcb->processId = idleProcess->processId;
    pcb->registers = idleProcess->registers;
    pcb->status = idleProcess->status;
}

void scheduler_blockProcess(ProcessId_t processId) {
    g_processes[processId].status = BLOCKED;
}

void scheduler_unblockProcess(ProcessId_t processId) {
    g_processes[processId].status = WAITING;
    addReadyProcess(&g_processes[processId]);
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
        if (g_currentProcess->status != DEAD && g_currentProcess->processId > 0) {
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
    else if (g_currentProcess == NULL || g_currentProcess->status == DEAD)
    {
        if (g_currentProcess != NULL) {
            g_currentProcess->processId = 0;
        }

        g_currentProcess = getIdleProcess();
        g_currentProcess->status = RUNNING;
        copyPcb(g_currentProcess, currentPcb);
        currentPcb->processId = g_currentProcess->processId;
    }
    else
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

static ProcessId_t scheduler_getNextProcessId(void) {
    int i;
    for (i = 1; i <= MAX_ALLOWED_PROCESSES; i++) {
       if (g_processes[i].processId == 0) {
           return i;
       }
    }
    return 0;
}

static PCB_t* getIdleProcess(void) {
    return &g_processes[0];
}

static void idleProcess(void) {
    while (1) {

    }
}
