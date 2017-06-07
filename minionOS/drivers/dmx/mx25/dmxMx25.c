/*
 * dmxMx25.c
 *
 *  Created on: 7 Jun 2017
 *      Author: Mathias
 */

#include "dmxMx25.h"

static uint8_t dmxData[DMX_CHANNELS_MX25] = { 0 };


void dmx_sendMx25(int startChannel, const DmxDataMx25_t * data)
{
    dmxData[0] = data->startCode;
    dmx_writeToChannel(dmxData, startChannel, data->pan);
    dmx_writeToChannel(dmxData, startChannel + 1, data->tilt);
    dmx_writeToChannel(dmxData, startChannel + 2, data->speed);
    dmx_writeToChannel(dmxData, startChannel + 3, data->red);
    dmx_writeToChannel(dmxData, startChannel + 4, data->green);
    dmx_writeToChannel(dmxData, startChannel + 5, data->blue);
    dmx_writeToChannel(dmxData, startChannel + 6, data->colorMacro);
    dmx_writeToChannel(dmxData, startChannel + 7, data->ledCircuit);
    dmx_writeToChannel(dmxData, startChannel + 8, data->dimmingIntensity);

    dmx_send(startChannel, &dmxData, DMX_CHANNELS_MX25);
}

