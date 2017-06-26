/*
 * Common.h
 *
 *  Created on: 15 Mar 2017
 *      Author: Mathias
 * Description: Provides common tools to improve and simplify usage of the system.
 */

#ifndef BITMANIPULATION_H_
#define BITMANIPULATION_H_

/* Macros for bit manipulation */
#define bitRead(value, bit)             (((value) >> (bit)) & 0x01)
#define bitSet(value, bit)              ((value) |= (1UL << (bit)))
#define bitClear(value, bit)            ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue)  (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#define getBitRange(x,msb,lsb)          (x >> lsb) & ~(~0 << (msb-lsb+1))
#endif /* BITMANIPULATION_H_ */
