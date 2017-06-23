/*
 * hexIntParser.c
 *
 *  Created on: 22.06.2017
 *      Author: sabrinarosmann
 */

#include "kernel/common/hexIntParser.h"

uint8_t hexIntParser_parseHex4ToInt(uint8_t i)
{
    if (i >= '0' && i <= '9')
    {
        return i - '0';
    }
    else if (i >= 'a' && i <= 'f')
    {
        return i - 'a' + 10;
    }
    else if (i >= 'A' && i <= 'F')
    {
        return i - 'A' + 10;
    }
    else
    {
        return i;
    }
}

uint8_t hexIntParser_parseHex8ToInt(uint8_t* i)
{
    return (hexIntParser_parseHex4ToInt(i[0]) << 4) + hexIntParser_parseHex4ToInt(i[1]);
}

uint32_t hexIntParser_parseHex16ToInt(uint8_t* i)
{
    return (hexIntParser_parseHex4ToInt(i[0]) << 12) +
           (hexIntParser_parseHex4ToInt(i[1]) << 8) +
           (hexIntParser_parseHex4ToInt(i[2]) << 4) +
           hexIntParser_parseHex4ToInt(i[3]);
}

uint8_t hexIntParser_parseIntToHex8(uint8_t* i)
{
    return ((i[0] << 4) + i[1]);
}

uint16_t hexIntParser_parseIntToHex16(uint8_t* i)
{
    return ((i[0] << 12) + (i[1] << 8) + (i[2] << 4) + i[3]);
}

uint32_t hexIntParser_parseIntToHex24(uint8_t* i)
{
    return ((i[0] << 20) + (i[1] << 16) + (i[2] << 12) + (i[3] << 8) + (i[4] << 4) + i[5]);
}

uint32_t hexIntParser_parseIntToHex32(uint8_t* i)
{
    return ((i[0] << 28) + (i[1] << 24) + (i[2] << 20) + (i[3] << 16) + (i[4] << 12) + (i[5] << 8) + (i[6] << 4) + i[7]);
}
