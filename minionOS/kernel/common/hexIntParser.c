/*
 * hexIntParser.c
 *
 *  Created on: 22.06.2017
 *      Author: sabrinarosmann
 */

#include "kernel/common/hexIntParser.h"

uint8_t hexParser_parseHex4ToInt(uint8_t i)
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
        return 0;
    }
}

uint8_t hexParser_parseHex8ToInt(uint8_t* i)
{
    return (hexParser_parseHex4ToInt(i[0]) << 4) + hexParser_parseHex4ToInt(i[1]);
}

uint32_t hexParser_parseHex16ToInt(uint8_t* i)
{
    return (hexParser_parseHex4ToInt(i[0]) << 12) +
           (hexParser_parseHex4ToInt(i[1]) << 8) +
           (hexParser_parseHex4ToInt(i[2]) << 4) +
           hexParser_parseHex4ToInt(i[3]);
}

