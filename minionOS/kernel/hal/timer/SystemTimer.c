/*
 * SystemTimer.c
 *
 *  Created on: 5 Apr 2017
 *      Author: Mathias
 */

#include "SystemTimer.h"
#include <stdio.h>
#include <math.h>

#define MAX_CALLBACKS 30

typedef struct
{
    uint32_t lastCallbackTime;
    uint32_t interval_ms;
    TickCallback_t callback;
} TimerCallbackSubscription_t;

static TimerCallbackSubscription_t g_registeredCallbacks[MAX_CALLBACKS] = { 0 };
static Timer_t * g_systemTimer;

uint32_t g_current_ms = 0;

void systemtimer_handler(void)
{
    g_current_ms++;
    int i = 0;
    for(i = 0; i < MAX_CALLBACKS; i++){
        if(g_registeredCallbacks[i].callback != NULL && abs(g_current_ms - g_registeredCallbacks[i].lastCallbackTime) >= g_registeredCallbacks[i].interval_ms){
            g_registeredCallbacks[i].lastCallbackTime = g_current_ms;
            g_registeredCallbacks[i].callback();
        }
    }

    clear_interrupt_flag(g_systemTimer);
}

void init_system_timer(uint32_t interval_us)
{
    g_systemTimer = create_timer(OVERFLOW, AUTORELOAD, interval_us,
                                 &systemtimer_handler);
}

void start_system_timer()
{
    start_timer(g_systemTimer);
}

void stop_system_timer()
{
    stop_timer(g_systemTimer);
}

void subscribe_systimer_callback(
        uint32_t interval_ms, TickCallback_t callback)
{
    int i = 0;
    while (i < MAX_CALLBACKS && g_registeredCallbacks[i].callback != NULL)
    {
        i++;
    }

    if (i >= MAX_CALLBACKS)
    {
        // we are full
    }
    else
    {
        TimerCallbackSubscription_t newRegisteredCallback = {
                .interval_ms = interval_ms, .callback = callback };

        g_registeredCallbacks[i] = newRegisteredCallback;
    }
}

//void desubscribe_systimer_callback(TimerCallbackSubscription_t * subscription)
//{
//    //TODO: not sure if needed
//}

