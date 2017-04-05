/*
 * Timer.c
 *
 *  Created on: 5 Apr 2017
 *      Author: Mathias
 */
#include "kernel/hal/timer/Timer.h"
#include "kernel/devices/omap3530/includes/Timer.h"
#include "kernel/hal/clock/Clock.h"
#include <stdio.h>
#include "kernel/hal/interrupts/Interrupts.h"

static Timer_t g_timer[AMOUNT_OF_TIMERS];

Timer_t * create_timer(TimerMode mode, ReloadType reloadType,
                       uint32_t interval_us, TickCallback_t callback)
{
    int8_t freeTimerIndex = get_free_timer_index();

    if(freeTimerIndex < 0){
        return NULL;
    }

    TimerNumber freeTimerNr = (TimerNumber) freeTimerIndex;

    Timer_t newTimer = { .timerNr = freeTimerNr, .timerMode = mode,
                         .reloadType = reloadType, .interval_us = interval_us,
                         .callback = callback };

    g_timer[freeTimerIndex] = newTimer;

    init_timer(&g_timer[freeTimerIndex]);

    g_timer[freeTimerIndex].initialized = TRUE;
    return &g_timer[freeTimerIndex];
}

static int8_t get_free_timer_index(void)
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

void timer_handler(uint32_t source){

    TimerNumber timerNumber = get_timer_number_from_irq_source(source);
    g_timer[timerNumber].callback();
}

static void init_timer(Timer_t * timer)
{
    uint32_t timerBaseAddress = get_timer_address(timer->timerNr);
    uint32_t clockRate = get_clock_rate(timer->interval_us);
    uint32_t loadValue = calc_load_value(clockRate, timer->interval_us);

    // Clear timer load value and set to a given load value
    // Check p.2614 for example values for the load value
    set_32(timerBaseAddress + GPTIMER_TLDR, loadValue);
    set_32(timerBaseAddress + GPTIMER_TCRR, loadValue);

    set_timer_clock(timer->timerNr, KHZ_32_CLOCK);

    uint32_t tier = 0;
    switch (timer->timerMode)
    {
    case OVERFLOW:
        tier = TIER_OVF_IT_ENABLE;
        break;
    case COMPARE:
        tier = TIER_MAT_IT_ENABLE;
        break;
    case CAPTURE:
        tier = TIER_TCAR_IT_ENABLE;
        break;
    default:
        tier = 0;
        break;
    }

    // Set interrupt enabled for either overflow, compare or capture
    set_32(timerBaseAddress + GPTIMER_TIER, tier);

    clear_interrupt_flag(timer);

    // Register handler and enable interrupt source
    uint32_t irq_nr = get_irq_number(timer->timerNr);
    register_interrupt_handler(&timer_handler, irq_nr);
}

void clear_interrupt_flag(Timer_t * timer)
{
    uint32_t timerBaseAddress = get_timer_address(timer->timerNr);
    set_32(timerBaseAddress + GPTIMER_TISR, (1 << 1)); // Clear interrupt flag
}

void start_timer(Timer_t * timer)
{
    uint32_t timerBaseAddress = get_timer_address(timer->timerNr);

    uint32_t reloadMode;

    if (timer->reloadType == AUTORELOAD)
    {
        reloadMode = TCLR_AR_AUTORELOAD;
    }
    else
    {
        reloadMode = TCLR_AR_ONESHOT;
    }

    set_32(timerBaseAddress + GPTIMER_TCLR, reloadMode | TCLR_ST_ON);
}

void stop_timer(Timer_t * timer)
{
    uint32_t timerBaseAddress = get_timer_address(timer->timerNr);
    clear_32(timerBaseAddress + GPTIMER_TCLR, TCLR_ST_ON);
}

#define MAX_TIMER_LOAD_VAL 0xFFFFFFFE

static uint32_t calc_load_value(uint32_t clockRate, uint32_t interval_us)
{
    // page 2614, modified
    uint32_t loadValue = (0xFFFFFFFF + 1
            - get_s_from_us(interval_us) * clockRate);

    if (loadValue > MAX_TIMER_LOAD_VAL)
    {
        loadValue = MAX_TIMER_LOAD_VAL;
    }
    return loadValue;
}

static uint32_t get_clock_rate(uint32_t interval_us)
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
static float get_s_from_us(uint32_t us)
{
    return us / 1000000.0;
}
