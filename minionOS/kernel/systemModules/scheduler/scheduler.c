/*
 * scheduler.c
 *
 *  Created on: 13 Apr 2017
 *      Author: Mathias
 */
#include "kernel/systemModules/scheduler/scheduler.h"
#include "kernel/hal/timer/systemTimer.h"
#include "kernel/systemModules/processManagement/contextSwitch.h"
#define SCHEDULER_INTERVAL_MS 50

PCB_t g_processes[MAX_ALLOWED_PROCESSES];

PCB_t * g_currentProcess;
PCB_t * g_sleepingProcess;
PCB_t * g_blockedProcess;
PCB_t * g_readyProcess;

SubscriptionId_t g_systemTimerId;

static void handleSchedulerTick(void);
static void initSchedulerTimer(uint32_t interval_ms);

void scheduler_init(void)
{
    initSchedulerTimer(SCHEDULER_INTERVAL_MS);
}

void scheduler_start(void)
{
    systemTimer_enableSubscription(g_systemTimerId);
}

void scheduler_stop(void)
{
    systemTimer_disableSubscription(g_systemTimerId);
}

static void handleSchedulerTick(void)
{
    // set current process sleeping
    // select new process (round robin)
}

static void initSchedulerTimer(uint32_t interval_ms)
{
    g_systemTimerId = systemTimer_subscribeCallback(interval_ms,
                                                    &handleSchedulerTick);
    systemTimer_disableSubscription(g_systemTimerId);
}
