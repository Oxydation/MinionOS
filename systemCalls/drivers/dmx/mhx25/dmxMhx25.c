/*
 * dmxMx25.c
 *
 *  Created on: 7 Jun 2017
 *      Author: Mathias
 */

#include <drivers/dmx/mhx25/dmxMhx25.h>

static void dmx_writeToChannel(uint8_t * data, uint16_t channel, uint8_t value);

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

/*
 * Writes the values 0...255 to channel 1...11
 */
static void dmx_writeToChannel(uint8_t * data, uint16_t channel, uint8_t value)
{
    if (channel < 1)
        channel = 1;
    if (value > 255)
        value = 255;

    data[channel] = value;
}
