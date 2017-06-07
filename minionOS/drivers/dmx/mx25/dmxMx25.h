/*
 * dmxMx25.h
 *
 *  Created on: 7 Jun 2017
 *      Author: Mathias
 */

#ifndef DRIVERS_DMX_MX25_DMXMX25_H_
#define DRIVERS_DMX_MX25_DMXMX25_H_

#include "global/types.h"
#include "kernel/hal/dmx/dmx.h"

#define DMX_CHANNELS_MX25 12

typedef struct
{
    uint8_t startCode;

    uint8_t pan;
    uint8_t tilt;
    uint8_t speed;

    uint8_t red;
    uint8_t green;
    uint8_t blue;

    uint8_t colorMacro;
    uint8_t ledCircuit;
    uint8_t dimmingIntensity;
} DmxDataMx25_t;

void dmx_sendMx25(int startChannel, const DmxDataMx25_t * data);


#endif /* DRIVERS_DMX_MX25_DMXMX25_H_ */
