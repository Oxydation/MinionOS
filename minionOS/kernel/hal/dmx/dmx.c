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

/*
 * Writes the values 0...255 to channel 1...11
 */
void dmx_writeToChannel(uint8_t * data, int channel, uint8_t value)
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

void dmx_send(int startChannel, const uint8_t * data, uint8_t size)
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

    // Mark between two packets
    pad_setMode(GPIO);
    gpio_digitalWrite(146, HIGH);
    delay(500);
    gpio_digitalWrite(146, LOW);

    delay(100);
}
