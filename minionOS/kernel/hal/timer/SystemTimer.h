/*
 * SystemTimer.h
 *
 *  Created on: 5 Apr 2017
 *      Author: Mathias
 */

#ifndef KERNEL_HAL_TIMER_SYSTEMTIMER_H_
#define KERNEL_HAL_TIMER_SYSTEMTIMER_H_

#include "Timer.h"
#include <inttypes.h>


void init_system_timer(uint32_t interval_us);
void start_system_timer();
void stop_system_timer();

void subscribe_systimer_callback(uint32_t interval_ms,
                                  TickCallback_t callback);
//void desubscribe_systimer_callback(TimerCallbackSubscription_t * subscription);

#endif /* KERNEL_HAL_TIMER_SYSTEMTIMER_H_ */
