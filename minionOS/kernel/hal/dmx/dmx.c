/*
 * dmx.c
 *
 *  Created on: 22.05.2017
 *      Author: Jasmin
 */
#include "kernel/hal/dmx/dmx.h"
#include "kernel/hal/uart/uart.h"

//DMX pins
#define DMX_DI_PIN 6 //DI Driver Input of ST485
#define DMX_DE_PIN 8 //DE Driver Output Enable of ST485
#define DMX_CHANNELS 12 //1 StartCode, 11 Channels

uint8_t dmxData[DMX_CHANNELS] = { };

/*
 * Writes the values 0...255 to channel 1...11
 */
void dmx_writeToChannel(int channel, uint8_t value) {

    if (channel < 1)
        channel = 1;
    if (channel > DMX_CHANNELS)
        channel = DMX_CHANNELS;
    if (value < 0)
        value = 0;
    if (value > 255)
        value = 255;

    dmxData[channel] = value;
}

int main_dmx(void) {
    UartConfig_t config = {
        .baudMultiple = x16,
        .baudRate = 250000,
        .stopMode =  STOP_1,
        .parityMode = NO_PARITY,
        .wordLength = LENGTH_8 };

    initModule(UART2, config);

    while (1) {

        disableModule(UART2);
        volatile uint32_t i = 0;
        while (i <= 170) {
            i++;
        }

        enableModule(UART2);

        //fill dmxData
        dmxData[0] = 0; //StartCode
//        dmx_writeToChannel(1, 128);
//        dmx_writeToChannel(2, 128);
//        dmx_writeToChannel(3, 0);
//        dmx_writeToChannel(4, 0);
//        dmx_writeToChannel(5, 0);
//        dmx_writeToChannel(6, 255);
//        dmx_writeToChannel(7, 0);
//        dmx_writeToChannel(8, 255);
//        dmx_writeToChannel(9, 0);
//        dmx_writeToChannel(10, 0);
//        dmx_writeToChannel(11, 0);
//
        dmx_writeToChannel(1, 50);
       dmx_writeToChannel(2, 100);
       dmx_writeToChannel(3, 255);
       dmx_writeToChannel(4, 0);
       dmx_writeToChannel(5, 0);
       dmx_writeToChannel(6, 255);
       dmx_writeToChannel(7, 255);
       dmx_writeToChannel(8, 255);
       dmx_writeToChannel(9, 255);
       dmx_writeToChannel(10, 255);
       dmx_writeToChannel(11, 255);

        //send data
        transmit(UART2, dmxData, DMX_CHANNELS);
    }
    return 0;
}
