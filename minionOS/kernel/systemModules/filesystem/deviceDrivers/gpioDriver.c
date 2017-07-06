#include <kernel/systemModules/filesystem/deviceDrivers/gpioDriver.h>
#include "kernel/hal/gpio/gpio.h"
#include "kernel/devices/omap3530/includes/gpio.h"
#include "kernel/devices/omap3530/includes/beagleBoardC4.h"
#include "global/types.h"
#include <string.h>

static void open(void) {
}

static int read(uint8_t* buffer, unsigned int bufferSize) {
    return 0;
}

static void writeLed1(const uint8_t* buffer, unsigned int bufferSize) {
    gpio_pinMode(GPIO_USR0_LED, OUTPUT);

    const char* charBuf = (const char*) buffer;
    // inversed logic in digitalWrite
    if (strncmp(charBuf, "1", bufferSize) == 0) {
        gpio_digitalWrite(GPIO_USR0_LED, LOW);
    } else if (strncmp(charBuf, "0", bufferSize) == 0) {
        gpio_digitalWrite(GPIO_USR0_LED, HIGH);
    }
}

static void writeLed2(const uint8_t* buffer, unsigned int bufferSize) {
    gpio_pinMode(GPIO_USR1_LED, OUTPUT);

    const char* charBuf = (const char*) buffer;
    // inversed logic in digitalWrite
    if (strncmp(charBuf, "1", bufferSize) == 0) {
        gpio_digitalWrite(GPIO_USR1_LED, LOW);
    } else if (strncmp(charBuf, "0", bufferSize) == 0) {
        gpio_digitalWrite(GPIO_USR1_LED, HIGH);
    }
}

static void release(void) {
    // NO OP
}

FileOperations_t led1Driver = {
        .open = open,
        .read = read,
        .write = writeLed1,
        .release = release
};


FileOperations_t led2Driver = {
        .open = open,
        .read = read,
        .write = writeLed2,
        .release = release
};
