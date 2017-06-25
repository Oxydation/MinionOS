/*
 * dmxTmh7.h
 *
 *  Created on: 7 Jun 2017
 *      Author: Mathias
 */

#ifndef DRIVERS_DMX_DMXTMH7_H_
#define DRIVERS_DMX_DMXTMH7_H_

#include "global/types.h"
#include "kernel/hal/dmx/dmx.h"

#define DMX_CHANNELS_TMH7 12

typedef enum
{
    Neutral,
    Macro1,
    Macro2,
    Macro3,
    Macro4,
    Macro5,
    Macro6
} ColorMacro_t;

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
} DmxDataTMH7_t;

void dmx_createTmh7Packet(uint16_t startChannel, const DmxDataTMH7_t * data, uint8_t * result);

#endif /* DRIVERS_DMX_DMXTMH7_H_ */
