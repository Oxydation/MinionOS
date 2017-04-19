/*
 * led.h
 *
 *  Created on: 19.04.2017
 *      Author: Jasmin
 */

#ifndef KERNEL_HAL_LED_LED_H_
#define KERNEL_HAL_LED_LED_H_
#include <stdbool.h>

#define LED_0   0
#define LED_1   1
void led_activateLed(bool turnOn, int led);

#endif /* KERNEL_HAL_LED_LED_H_ */
