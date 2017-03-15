/*
 * Common.h
 *
 *  Created on: 15 Mar 2017
 *      Author: Mathias
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <inttypes.h>

#define LOW 0x0
#define HIGH !LOW

#define INPUT 0x1
#define OUTPUT 0x0

/* Useful macros */
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

extern void digitalWrite(uint8_t pin, uint8_t val);
extern uint8_t digitalRead(uint8_t pin);
extern void pinMode(uint8_t pin, uint8_t mode);

#endif /* COMMON_H_ */
