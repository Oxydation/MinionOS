/*
 * ledControl.c
 *
 *  Created on: 05.04.2017
 *      Author: Arno Riedmann
 */

#include "drivers/led/ledControl.h"
#include "global/Common.h"
#include "kernel/hal/gpio/GPIO.h"
#include "kernel/devices/omap3530/includes/GPIO.h"
#include "kernel/devices/omap3530/includes/BeagleBoardC4.h"

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
    pinMode(pin, OUTPUT);
    if (turnOn) {
        digitalWrite(pin, HIGH);
    } else {
        digitalWrite(pin, LOW);
    }
}

