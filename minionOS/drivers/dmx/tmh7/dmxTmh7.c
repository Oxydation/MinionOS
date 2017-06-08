/*
 * dmxTmh7.c
 *
 *  Created on: 7 Jun 2017
 *      Author: Mathias
 */
#include "dmxTmh7.h"
#include <string.h>

static void createDataTmh7(uint16_t startChannel,
                               const DmxDataTMH7_t * data, uint8_t * result);

void dmx_sendTMH7(uint16_t startChannel, const DmxDataTMH7_t * data)
{
    uint16_t arraySize = startChannel + DMX_CHANNELS_TMH7 - 1;
    uint8_t dmxData[arraySize];
    memset(dmxData, 0, arraySize * sizeof(uint8_t));

    createDataTmh7(startChannel, data, dmxData);

    dmx_send(startChannel, &dmxData, arraySize);
}

static void createDataTmh7(uint16_t startChannel,
                               const DmxDataTMH7_t * data, uint8_t * result)
{
    result[0] = data->startCode;
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
