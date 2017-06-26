/*
 * dmxTmh7.c
 *
 *  Created on: 7 Jun 2017
 *      Author: Mathias
 */
#include "dmxTmh7.h"

static void dmx_writeToChannel(uint8_t * data, uint16_t channel, uint8_t value);
void dmx_createTmh7Packet(uint16_t startChannel,
                               const DmxDataTMH7_t * data, uint8_t * result)
{
    dmx_writeToChannel(result, startChannel, data->pan);
    dmx_writeToChannel(result, startChannel + 1, data->tilt);
    dmx_writeToChannel(result, startChannel + 2, data->speed);
    dmx_writeToChannel(result, startChannel + 3, data->red);
    dmx_writeToChannel(result, startChannel + 4, data->green);
    dmx_writeToChannel(result, startChannel + 5, data->blue);
    dmx_writeToChannel(result, startChannel + 6, data->colorMacro);
    dmx_writeToChannel(result, startChannel + 7, data->ledCircuit);
    dmx_writeToChannel(result, startChannel + 8, data->dimmingIntensity);
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

