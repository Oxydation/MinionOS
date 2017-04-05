/*
 * Common.h
 *
 *  Created on: 15 Mar 2017
 *      Author: Mathias
 * Description: Provides common tools to improve and simplify usage of the system.
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <inttypes.h>

#define LOW 0x0
#define HIGH !LOW

#define FALSE 0
#define TRUE !FALSE

/* Macros for bit manipulation */
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

#endif /* COMMON_H_ */
