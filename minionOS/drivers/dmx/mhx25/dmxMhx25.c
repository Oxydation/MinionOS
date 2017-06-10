/*
 * dmxMx25.c
 *
 *  Created on: 7 Jun 2017
 *      Author: Mathias
 */

#include <drivers/dmx/mhx25/dmxMhx25.h>

void dmx_createMhX25Packet(uint16_t startChannel, const DmxDataMhX25_t * data, uint8_t * result)
{
    dmx_writeToChannel(result, startChannel, data->pan);
    dmx_writeToChannel(result, startChannel + 1, data->tilt);
    dmx_writeToChannel(result, startChannel + 2, data->panFine);
    dmx_writeToChannel(result, startChannel + 3, data->tiltFine);
    dmx_writeToChannel(result, startChannel + 4, data->speed);
    dmx_writeToChannel(result, startChannel + 5, data->colour);
    dmx_writeToChannel(result, startChannel + 6, data->shutter);
    dmx_writeToChannel(result, startChannel + 7, data->dimmer);
    dmx_writeToChannel(result, startChannel + 8, data->gobo);
    dmx_writeToChannel(result, startChannel + 9, data->goboRotation);
    dmx_writeToChannel(result, startChannel + 10, data->specialFunctions);
    dmx_writeToChannel(result, startChannel + 11, data->programmes);
}
