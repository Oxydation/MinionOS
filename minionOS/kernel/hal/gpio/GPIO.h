/*
 * GPIO.h
 *
 *  Created on: 5 Apr 2017
 *      Author: Mathias
 */

#ifndef KERNEL_HAL_GPIO_GPIO_H_
#define KERNEL_HAL_GPIO_GPIO_H_

#include <inttypes.h>

/*
 * GPIO read and write operations; Direction mode operation
 */
extern void gpio_digitalWrite(uint8_t pin, uint8_t val);
extern uint8_t gpio_digitalRead(uint8_t pin);
extern void gpio_pinMode(uint8_t pin, uint8_t mode);

#endif /* KERNEL_HAL_GPIO_GPIO_H_ */
