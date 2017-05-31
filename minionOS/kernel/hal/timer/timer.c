/*
 * Timer.c
 *
 *  Created on: 5 Apr 2017
 *      Author: Mathias
 */
#include <kernel/hal/clock/clock.h>
#include <kernel/hal/interrupts/interrupts.h>
#include <kernel/hal/timer/timer.h>
#include <stdio.h>
#include "global/types.h"

static void init_timer(Timer_t * timer);
static int8_t getFreeTimerIndex(void);
static void isr_handler(uint32_t source, PCB_t * currentPcb);
static uint32_t getClockRateFromInterval(uint32_t interval_us);
static float getSecondsFromMicroseconds(uint32_t microseconds);
static uint32_t calcLoadValue(uint32_t clockRate, uint32_t interval_us);

static Timer_t g_timer[AMOUNT_OF_TIMERS];

Timer_t * timer_create(TimerMode_t mode, ReloadType_t reloadType,
                       uint32_t interval_us, TickCallback_t callback)
{
    int8_t freeTimerIndex = getFreeTimerIndex();

    if (freeTimerIndex < 0)
    {
        return NULL;
    }

    TimerNumber_t freeTimerNr = (TimerNumber_t) freeTimerIndex;

    Timer_t newTimer = { .timerNr = freeTimerNr, .timerMode = mode,
                         .reloadType = reloadType, .interval_us = interval_us,
                         .callback = callback };

    g_timer[freeTimerIndex] = newTimer;

    init_timer(&g_timer[freeTimerIndex]);

    g_timer[freeTimerIndex].initialized = TRUE;
    return &g_timer[freeTimerIndex];
}

static int8_t getFreeTimerIndex(void)
{
    int i = 0;
    while (i < AMOUNT_OF_TIMERS && g_timer[i].initialized == TRUE)
    {
        i++;
    }

    if (i == AMOUNT_OF_TIMERS)
    {
        return -1;
    }
    else
    {
        return i;
    }
}

static void init_timer(Timer_t * timer)
{
    uint32_t timerBaseAddress = timer_getTimerAddress(timer->timerNr);
    uint32_t clockRate = getClockRateFromInterval(timer->interval_us);
    uint32_t loadValue = calcLoadValue(clockRate, timer->interval_us);

    timer_setTimerLoadValue(timer->timerNr, loadValue);
    clock_setTimerClockSource(timer->timerNr, KHZ_32_CLOCK);
    timer_setTimerInterruptEnabled(timer->timerNr, timer->timerMode);
    timer_clearInterruptFlag(timer);

    // Register handler and enable interrupt source
    uint32_t irq_nr = timer_getIrqNumber(timer->timerNr);
    interrupts_registerHandler(&isr_handler, irq_nr);
}

static void isr_handler(uint32_t source, PCB_t * currentPcb)
{
    TimerNumber_t timerNumber = timer_getTimerNumberFromIrqSource(source);
    g_timer[timerNumber].callback(currentPcb);
}

void timer_start(Timer_t * timer)
{
    omapTimer_start(timer->timerNr, timer->reloadType);
}

void timer_stop(Timer_t * timer)
{
    omapTimer_stop(timer->timerNr);
}

void timer_clearInterruptFlag(Timer_t * timer)
{
    omapTimer_clearInterruptFlag(timer->timerNr);
}


static uint32_t calcLoadValue(uint32_t clockRate, uint32_t interval_us)
{
    // page 2614, modified
    uint32_t loadValue = (UINT32_MAX + 1
            - getSecondsFromMicroseconds(interval_us) * clockRate);

    if (loadValue > MAX_TIMER_LOAD_VAL)
    {
        loadValue = MAX_TIMER_LOAD_VAL;
    }

    //Info: if we want to use exactly 1ms timer, then check page 2608; use timer 1 as 1ms timer
    return loadValue;
}

static uint32_t getClockRateFromInterval(uint32_t interval_us)
{
    if (interval_us < 65)
    { // < 65us
      // use system clock
        return CLK_32KHZ; //TODO not working this way!
    }
    else if (interval_us >= 1000)
    {
        return CLK_32KHZ;
    }
    return 0;
}

static float getSecondsFromMicroseconds(uint32_t microseconds)
{
    return microseconds / 1000000.0;
}
