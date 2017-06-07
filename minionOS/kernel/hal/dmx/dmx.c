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
#include "kernel/hal/gpio/gpio.h"
#include <kernel/devices/omap3530/includes/gpio.h>

// DMX pins
#define DMX_DI_PIN 6    // DI Driver Input of ST485
#define DMX_DE_PIN 8    // DE Driver Output Enable of ST485
#define DMX_CHANNELS 25 // 1 StartCode, 11 Channels

#define PADCONF_BASE 0x48002030
#define CONTROL_PADCONF_UART2_TX 0x48002178
#define CONTROL_PADCONF_UART2_CTS 0x48002174

static uint8_t dmxData[DMX_CHANNELS] = { 0 };
static void delay(int amount);

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

static uint8_t g_pan = 0;
static uint8_t g_tilt = 0;

static void sendData()
{
    dmxData[0] = 0;                 // StartCode
    dmx_writeToChannel(1, g_pan++);     // Pan
    dmx_writeToChannel(2, g_tilt++);     // Tilt
    dmx_writeToChannel(3, 0);     // Speed
    dmx_writeToChannel(4, 0);     // Red
    dmx_writeToChannel(5, 0);     // Green
    dmx_writeToChannel(6, 255);     // Blue
    dmx_writeToChannel(7, 0);     // Color-Macro
    dmx_writeToChannel(8, 255);     // LED Circuit
    dmx_writeToChannel(9, 255);     // Dimmerintensity
    dmx_writeToChannel(10, 0);      // 16-bit PAN
    dmx_writeToChannel(11, 0);      // 16-bit TILT

    uart_transmit(UART2, dmxData, DMX_CHANNELS);
}

typedef enum
{
    UART, GPIO
} GpioMode_t;

static void pad_setMode(GpioMode_t mode)
{
    switch (mode)
    {
    case UART:
        set32(CONTROL_PADCONF_UART2_TX, 0x00 << 0);
        set32(CONTROL_PADCONF_UART2_CTS, 0x00 << 16);
        break;
    case GPIO:
        set32(CONTROL_PADCONF_UART2_TX, 0x04 << 0);
        set32(CONTROL_PADCONF_UART2_CTS, 0x04 << 16);
        break;
    }
}

int main_dmx(void)
{
    UartConfig_t config = { .baudMultiple = x16, .baudRate = 250000, .stopMode =
                                    STOP_1_5,
                            .parityMode = NO_PARITY, .wordLength = LENGTH_8 };
    pad_setMode(UART);
    uart_initModule(UART2, config);
    gpio_pinMode(146, OUTPUT);

    while (1)
    {
        // reset
        pad_setMode(GPIO);
        gpio_digitalWrite(146, LOW);
        delay(1000);

        // mark
        gpio_digitalWrite(146, HIGH);
        delay(500);
        gpio_digitalWrite(146, LOW);

        // send data
        pad_setMode(UART);
        uart_updateConfig(UART2, config);
        sendData();

        // MARK zwischen paketen
        pad_setMode(GPIO);
        gpio_digitalWrite(146, HIGH);
        delay(500);
        gpio_digitalWrite(146, LOW);

        delay(2000);
    }
    return 0;
}

static void delay(int amount)
{
    volatile int i = 0;
    for (i = 0; i < amount; i++)
        ;
}
