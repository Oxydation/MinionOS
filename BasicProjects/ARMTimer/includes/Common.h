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

/* Macros for bit manipulation */
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

/*
 * GPIO read and write operations; Direction mode operation
 */
extern void digitalWrite(uint8_t pin, uint8_t val);
extern uint8_t digitalRead(uint8_t pin);
extern void pinMode(uint8_t pin, uint8_t mode);

/**
 * Provides basic methods to set and get a value at a given address.
 */
extern void set_32(uint32_t address, uint32_t value);
extern uint32_t Get32(uint32_t address);

#endif /* COMMON_H_ */
