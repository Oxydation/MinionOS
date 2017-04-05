/*
 * Common.h
 *
 *  Created on: 5 Apr 2017
 *      Author: Mathias
 */

#ifndef KERNEL_COMMON_COMMON_H_
#define KERNEL_COMMON_COMMON_H_

#include "../../global/Common.h"

#include <inttypes.h>
/*
 * Represents a address.
 */
typedef volatile unsigned int * Address_t;

/**
 * Provides basic methods to set and get a value at a given address.
 */
extern void set_32(uint32_t address, uint32_t value);
extern uint32_t get_32(uint32_t address);
extern void or_32(uint32_t address, uint32_t value);
extern void and_32(uint32_t address, uint32_t value);
extern void clear_32(uint32_t address, uint32_t value);


#endif /* KERNEL_COMMON_COMMON_H_ */
