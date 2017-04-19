/*
 * ledControl.c
 *
 *  Created on: 05.04.2017
 *      Author: Arno Riedmann
 */

#include "ledControl.h"
#include "global/types.h"
#include "kernel/hal/gpio/gpio.h"
#include "kernel/devices/omap3530/includes/gpio.h"
#include "kernel/devices/omap3530/includes/beagleBoardC4.h"

#define LED_1   1
#define LED_2   2
#define LED_1_PIN GPIO_USR1_LED
#define LED_2_PIN GPIO_USR2_LED

void ledControl_activateLed(bool turnOn, int led) {
    uint8_t pin;
    switch (led) {
    case LED_1:
        pin = LED_1_PIN;
        break;
    case LED_2:
        pin = LED_2_PIN;
        break;
    default:
        return;
    }
    gpio_pinMode(pin, OUTPUT);
    if (turnOn) {
        gpio_digitalWrite(pin, HIGH);
    } else {
        gpio_digitalWrite(pin, LOW);
    }
}

