/*
 * Clock.h
 *
 *  Created on: 5 Apr 2017
 *      Author: Mathias
 */

#ifndef KERNEL_HAL_CLOCK_CLOCK_H_
#define KERNEL_HAL_CLOCK_CLOCK_H_

#include "../../devices/omap3530/includes/clock.h"

#include "../gpio/gpio.h"
#include "../timer/timer.h"

typedef enum{
    KHZ_32_CLOCK,
    SYS_CLOCK
} ClockSource_t;

void clock_setTimerClockSource(TimerNumber timerNr, ClockSource_t clockSource);

//void enable_gpio_clock(GpioModule module);
//void enable_mmc_clock(MmcModule module);

#endif /* KERNEL_HAL_CLOCK_CLOCK_H_ */
