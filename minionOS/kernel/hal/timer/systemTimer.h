/*
 * SystemTimer.h
 *
 *  Created on: 5 Apr 2017
 *      Author: Mathias
 */

#ifndef KERNEL_HAL_TIMER_SYSTEMTIMER_H_
#define KERNEL_HAL_TIMER_SYSTEMTIMER_H_

#include <inttypes.h>
#include "timer.h"

typedef uint8_t SubscriptionId_t;

void systemTimer_init(uint32_t interval_us);
void systemTimer_start();
void systemTimer_stop();

SubscriptionId_t systemTimer_subscribeCallback(uint32_t interval_ms,
                                  TickCallback_t callback);

void systemTimer_enableSubscription(SubscriptionId_t subscriptionId);
void systemTimer_disableSubscription(SubscriptionId_t subscriptionId);
#endif /* KERNEL_HAL_TIMER_SYSTEMTIMER_H_ */
