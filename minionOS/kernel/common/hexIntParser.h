/*
 * hexIntParser.h
 *
 *  Created on: 22.06.2017
 *      Author: sabrinarosmann
 */

#ifndef KERNEL_COMMON_HEXINTPARSER_H_
#define KERNEL_COMMON_HEXINTPARSER_H_

#include <inttypes.h>

uint8_t hexParser_parseHex4ToInt(uint8_t i);
uint8_t hexParser_parseHex8ToInt(uint8_t* i);
uint32_t hexParser_parseHex16ToInt(uint8_t* i);

#endif /* KERNEL_COMMON_HEXINTPARSER_H_ */
