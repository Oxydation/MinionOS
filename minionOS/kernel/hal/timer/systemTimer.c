/*
 * SystemTimer.c
 *
 *  Created on: 5 Apr 2017
 *      Author: Mathias
 */

#include <kernel/hal/timer/systemTimer.h>
#include <stdio.h>
#include <math.h>
#include "global/types.h"

#define MAX_CALLBACKS 30

static void systemtimer_handler(PCB_t * currentPcb);

typedef struct
{
    uint32_t lastCallbackTime;
    uint32_t interval_ms;
    TickCallback_t callback;
    SubscriptionId_t subscriptionId;
    uint8_t enabled;
} TimerCallbackSubscription_t;

static TimerCallbackSubscription_t g_registeredCallbacks[MAX_CALLBACKS] = { 0 };
static Timer_t * g_systemTimer;

uint32_t g_current_ms = 0;


void systemTimer_init(uint32_t interval_us)
{
    g_systemTimer = timer_create(OVERFLOW, AUTORELOAD, interval_us,
                                 &systemtimer_handler);
}

void systemTimer_start()
{
    timer_start(g_systemTimer);
}

void systemTimer_stop()
{
    timer_stop(g_systemTimer);
}

SubscriptionId_t systemTimer_subscribeCallback(uint32_t interval_ms,
                                   TickCallback_t callback)
{
    int i = 0;
    while (i < MAX_CALLBACKS && g_registeredCallbacks[i].callback != NULL)
    {
        i++;
    }

    if(i < MAX_CALLBACKS)
    {
        TimerCallbackSubscription_t newRegisteredCallback = {
                .interval_ms = interval_ms, .callback = callback, .subscriptionId = i };

        g_registeredCallbacks[i] = newRegisteredCallback;
    }

    return i;
}

void systemTimer_enableSubscription(SubscriptionId_t subscriptionId){
    g_registeredCallbacks[subscriptionId].enabled = TRUE;
}

void systemTimer_disableSubscription(SubscriptionId_t subscriptionId){
    g_registeredCallbacks[subscriptionId].enabled = FALSE;
}

static void systemtimer_handler(PCB_t * currentPcb)
{
    g_current_ms++;
    int i = 0;
    for (i = 0; i < MAX_CALLBACKS; i++)
    {
        if (g_registeredCallbacks[i].callback != NULL && g_registeredCallbacks[i].enabled
                && abs(g_current_ms - g_registeredCallbacks[i].lastCallbackTime)
                        >= g_registeredCallbacks[i].interval_ms)
        {
            g_registeredCallbacks[i].lastCallbackTime = g_current_ms;
            g_registeredCallbacks[i].callback(currentPcb);
        }
    }

    timer_clearInterruptFlag(g_systemTimer);
}
