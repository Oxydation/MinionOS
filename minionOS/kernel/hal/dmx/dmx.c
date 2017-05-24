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
    UartConfig_t config;
    initModule(UART2, config);

 /*       //TODO UART2 configuration

        // 12. Load the new protocol formatting (parity, stop bit, char length) and switch to register operational mode
        clearBit(uart2.LCR, LCR_DIV_EN);
        clearBit(uart2.LCR, LCR_BREAK_EN);

        clearBit(uart2.LCR, LCR_PARITY_EN);

        setBit(uart2.LCR, LCR_CHAR_LENGTH_1);
        setBit(uart2.LCR, LCR_CHAR_LENGTH_2);

        clearBit(uart2.LCR, LCR_NB_STOP);


        setBit(uart2.RHR, 0);
        setBit(uart2.RHR, 1);
        setBit(uart2.RHR, 2);
        setBit(uart2.RHR, 3);
        setBit(uart2.RHR, 4);
        setBit(uart2.RHR, 5);
        setBit(uart2.RHR, 6);
        setBit(uart2.RHR, 7);*/

        //fill dmxData
        dmxData[0] = 0; //StartCode
        dmx_writeToChannel(1, 128);
        dmx_writeToChannel(2, 128);
        dmx_writeToChannel(3, 0);
        dmx_writeToChannel(4, 0);
        dmx_writeToChannel(5, 0);
        dmx_writeToChannel(6, 255);
        dmx_writeToChannel(7, 0);
        dmx_writeToChannel(8, 255);
        dmx_writeToChannel(9, 0);
        dmx_writeToChannel(10, 0);
        dmx_writeToChannel(11, 0);

        //send data
        transmit(UART2, dmxData, DMX_CHANNELS);
        return 0;
    }
