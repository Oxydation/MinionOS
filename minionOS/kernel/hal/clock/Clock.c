/*
 * Clock.c
 *
 *  Created on: 5 Apr 2017
 *      Author: Mathias
 */

#include "kernel/hal/clock/Clock.h"
#include "kernel/common/Common.h"

void set_timer_clock(TimerNumber timerNr, ClockSource_t clockSource)
{
    if (clockSource == KHZ_32_CLOCK)
    {
        clear_32(PER_CM + CM_CLKSEL_PER, (1UL << (uint32_t)timerNr));
    }
    else if(clockSource == SYS_CLOCK)
    {
        set_32(PER_CM + CM_CLKSEL_PER, (1UL << (uint32_t)timerNr));
    }
}
