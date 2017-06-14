/*
 * dmx.c
 *
 *  Created on: 22.05.2017
 *      Author: Jasmin, Mathias
 */
#include "kernel/hal/dmx/dmx.h"
#include "kernel/hal/uart/uart.h"
#include "kernel/hal/timer/systemTimer.h"
#include "kernel/hal/timer/timer.h"
#include "kernel/hal/gpio/gpio.h"
#include <kernel/devices/omap3530/includes/gpio.h>
#include "global/delay/delay.h"

static UartConfig_t config = { .baudMultiple = x16, .baudRate = 250000,
                               .stopMode = STOP_1_5, .parityMode = NO_PARITY,
                               .wordLength = LENGTH_8 };

typedef enum
{
    UART, GPIO
} GpioMode_t;

typedef enum{
    Mode0 = 0x00,
    Mode1 = 0x01,
    Mode2 = 0x02,
    Mode3 = 0x03,
    Mode4 = 0x04,
    Mode5 = 0x05,
    Mode6 = 0x06,
    Mode7 = 0x07,
}MuxMode_t;

static void pad_setMode(GpioMode_t mode)
{
    switch (mode)
    {
    case UART:
        set32(CONTROL_PADCONF_UART2_TX, Mode0 << 0);
        set32(CONTROL_PADCONF_UART2_CTS, Mode0 << 16);
        break;
    case GPIO:
        set32(CONTROL_PADCONF_UART2_TX, Mode4 << 0);
        set32(CONTROL_PADCONF_UART2_CTS, Mode4 << 16);
        break;
    }
}

/*
 * Writes the values 0...255 to channel 1...11
 */
void dmx_writeToChannel(uint8_t * data, uint16_t channel, uint8_t value)
{
    if (channel < 1)
        channel = 1;
    if (value > 255)
        value = 255;

    data[channel] = value;
}

void dmx_init()
{
    pad_setMode(UART);
    uart_initModule(UART2, config);
    uart_updateConfig(UART2, config);
    gpio_pinMode(146, OUTPUT);
}

void dmx_send(const uint8_t * data, uint16_t size)
{
    // Reset
    pad_setMode(GPIO);
    gpio_digitalWrite(146, LOW);
    delay(1000);

    // Mark
    gpio_digitalWrite(146, HIGH);
    delay(500);
    gpio_digitalWrite(146, LOW);

    // Send data
    pad_setMode(UART);
    uart_updateConfig(UART2, config);
    uart_transmit(UART2, data, size);

    delay(200);
}
