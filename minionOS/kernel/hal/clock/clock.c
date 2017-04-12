/*
 * Clock.c
 *
 *  Created on: 5 Apr 2017
 *      Author: Mathias
 */

#include <kernel/common/mmio.h>
#include <kernel/hal/clock/clock.h>

void clock_setTimerClockSource(TimerNumber_t timerNr, ClockSource_t clockSource)
{
    if (clockSource == KHZ_32_CLOCK)
    {
        clear32(PER_CM + CM_CLKSEL_PER, (1UL << (uint32_t)timerNr));
    }
    else if(clockSource == SYS_CLOCK)
    {
        set32(PER_CM + CM_CLKSEL_PER, (1UL << (uint32_t)timerNr));
    }
}
