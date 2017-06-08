/*
 * dmxMx25.c
 *
 *  Created on: 7 Jun 2017
 *      Author: Mathias
 */

#include <drivers/dmx/mhx25/dmxMx25.h>

static uint8_t dmxData[DMX_CHANNELS_MHX25] = { 0 };


void dmx_sendMhX25(uint16_t startChannel, const DmxDataMhX25_t * data)
{
    dmxData[0] = data->startCode;
    dmx_writeToChannel(dmxData, startChannel, data->pan);
    dmx_writeToChannel(dmxData, startChannel + 1, data->tilt);
    dmx_writeToChannel(dmxData, startChannel + 2, data->panFine);
    dmx_writeToChannel(dmxData, startChannel + 3, data->tiltFine);
    dmx_writeToChannel(dmxData, startChannel + 4, data->speed);
    dmx_writeToChannel(dmxData, startChannel + 5, data->colour);
    dmx_writeToChannel(dmxData, startChannel + 6, data->shutter);
    dmx_writeToChannel(dmxData, startChannel + 7, data->dimmer);
    dmx_writeToChannel(dmxData, startChannel + 8, data->gobo);
    dmx_writeToChannel(dmxData, startChannel + 9, data->goboRotation);
    dmx_writeToChannel(dmxData, startChannel + 10, data->specialFunctions);
    dmx_writeToChannel(dmxData, startChannel + 11, data->programmes);

    dmx_send(startChannel, &dmxData, DMX_CHANNELS_MHX25);
}

