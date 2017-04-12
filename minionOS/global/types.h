/*
 * types.h
 *
 *  Created on: 9 Apr 2017
 *      Author: Mathias
 */

#ifndef GLOBAL_TYPES_H_
#define GLOBAL_TYPES_H_

#include <inttypes.h>

#define LOW 0x0
#define HIGH !LOW

#define FALSE 0
#define TRUE !FALSE

typedef volatile unsigned int * Address_t;

#endif /* GLOBAL_TYPES_H_ */
