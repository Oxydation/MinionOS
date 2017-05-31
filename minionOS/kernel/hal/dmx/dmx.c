/*
 * dmx.c
 *
 *  Created on: 22.05.2017
 *      Author: Jasmin
 */
#include "kernel/hal/dmx/dmx.h"
#include "kernel/hal/uart/uart.h"
#include "kernel/hal/timer/systemTimer.h"
#include "kernel/hal/timer/timer.h"
#include "global/types.h"

// DMX pins
#define DMX_DI_PIN 6    // DI Driver Input of ST485
#define DMX_DE_PIN 8    // DE Driver Output Enable of ST485
#define DMX_CHANNELS 12 // 1 StartCode, 11 Channels

#define PADCONF_BASE 0x48002030
#define CONTROL_PADCONF_UART2_TX 0x48002178
#define CONTROL_PADCONF_UART2_CTS 0x48002174

uint8_t dmxData[DMX_CHANNELS] = { };

/*
 * Writes the values 0...255 to channel 1...11
 */
void dmx_writeToChannel(int channel, uint8_t value)
{

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

static Timer_t * g_timer;
static void sendData();

static volatile uint8_t waiting = TRUE;

static void tick()
{
    waiting = FALSE;
    timer_clearInterruptFlag(g_timer);
}

static void initTimer()
{
    g_timer = timer_create(OVERFLOW, AUTORELOAD, 10, &tick);
}

static void sendData()
{
    dmxData[0] = 0;                 // StartCode
    dmx_writeToChannel(1, 1);       // Pan
    dmx_writeToChannel(2, 100);     // Tilt
    dmx_writeToChannel(3, 100);     // Speed
    dmx_writeToChannel(4, 0);       // Red
    dmx_writeToChannel(5, 0);       // Green
    dmx_writeToChannel(6, 255);     // Blue
    dmx_writeToChannel(7, 255);     // Color-Macro
    dmx_writeToChannel(8, 255);     // LED Circuit
    dmx_writeToChannel(9, 255);     // Dimmerintensity
    dmx_writeToChannel(10, 0);    // 16-bit PAN
    dmx_writeToChannel(11, 0);    // 16-bit TILT

    transmit(UART2, dmxData, DMX_CHANNELS);
}

int main_dmx(void)
{
    set32(CONTROL_PADCONF_UART2_TX, 0x00 << 0);
    set32(CONTROL_PADCONF_UART2_CTS, 0x00 << 16);

    UartConfig_t config = { .baudMultiple = x16, .baudRate = 250000, .stopMode =
                                    STOP_1,
                            .parityMode = NO_PARITY, .wordLength = LENGTH_8 };

    initModule(UART2, config);

    initTimer();
    timer_start(g_timer);

    while (1)
    {
        disableModule(UART2);
        timer_resetCounter(g_timer);
        waiting = TRUE;
        while (waiting == TRUE){}
        enableModule(UART2);
        sendData();
    }
    return 0;
}

//        volatile uint32_t i = 0;
//                while (i <= 170) {
//                    i++;
//                }

