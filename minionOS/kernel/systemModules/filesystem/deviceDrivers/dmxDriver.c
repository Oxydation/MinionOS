#include <kernel/systemModules/filesystem/deviceDrivers/dmxDriver.h>
#include "kernel/hal/uart/uart.h"
#include "kernel/hal/gpio/gpio.h"
#include "kernel/devices/omap3530/includes/gpio.h"
#include "kernel/common/mmio.h"
#include "delay/delay.h"
#include "global/types.h"


// DMX pins
#define DMX_DI_PIN      6    // DI Driver Input of ST485
#define DMX_DE_PIN      8    // DE Driver Output Enable of ST485
#define DMX_CHANNELS    25   // 1 StartCode, 11 Channels

// Pad configuration addresses
#define PADCONF_BASE                0x48002030
#define CONTROL_PADCONF_UART2_TX    0x48002178
#define CONTROL_PADCONF_UART2_CTS   0x48002174

typedef enum {
    UART, GPIO
} GpioMode_t;

typedef enum {
    Mode0 = 0x00,
    Mode1 = 0x01,
    Mode2 = 0x02,
    Mode3 = 0x03,
    Mode4 = 0x04,
    Mode5 = 0x05,
    Mode6 = 0x06,
    Mode7 = 0x07,
} MuxMode_t;

static UartConfig_t config = { .baudMultiple = x16, .baudRate = 250000,
                               .stopMode = STOP_1_5, .parityMode = NO_PARITY,
                               .wordLength = LENGTH_8 };

static void pad_setMode(GpioMode_t mode) {
    switch (mode) {
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

static void open(void) {
    pad_setMode(UART);
    uart_initModule(UART2, config);
    uart_updateConfig(UART2, config);
    gpio_pinMode(146, OUTPUT);
}

static int read(uint8_t* buffer, unsigned int bufferSize) {
    // NO OP
}

static void write(const uint8_t* buffer, unsigned int bufferSize) {
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
    //uart_updateConfig(UART2, config);
    uart_transmit(UART2, buffer, bufferSize);

    delay(200);
}
static void release(void) {
    // NO OP
}

FileOperations_t dmxDriver = {
        .open = open,
        .read = read,
        .write = write,
        .release = release
};
