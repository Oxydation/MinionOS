/*
 * hexIntParser.h
 *
 *  Created on: 22.06.2017
 *      Author: sabrinarosmann
 */

#ifndef KERNEL_COMMON_HEXINTPARSER_H_
#define KERNEL_COMMON_HEXINTPARSER_H_

#include <inttypes.h>

uint8_t hexIntParser_parseHex4ToInt(uint8_t i);
uint8_t hexIntParser_parseHex8ToInt(uint8_t* i);
uint32_t hexIntParser_parseHex16ToInt(uint8_t* i);
uint8_t hexIntParser_parseIntToHex8(uint8_t* i);
uint16_t hexIntParser_parseIntToHex16(uint8_t* i);
uint32_t hexIntParser_parseIntToHex24(uint8_t* i);
uint32_t hexIntParser_parseIntToHex32(uint8_t* i);

#endif /* KERNEL_COMMON_HEXINTPARSER_H_ */
