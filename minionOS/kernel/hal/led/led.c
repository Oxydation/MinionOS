/*
 * led.c
 *
 *  Created on: 19.04.2017
 *      Author: Jasmin
 */
#include "led.h"
#include "kernel/hal/gpio/gpio.h"
#include "kernel/devices/omap3530/includes/gpio.h"
#include "kernel/devices/omap3530/includes/beagleBoardC4.h"
#include "global/types.h"

#define LED_0_PIN GPIO_USR0_LED
#define LED_1_PIN GPIO_USR1_LED

void led_activateLed(bool turnOn, int led){
    uint8_t pin;
    switch (led) {
    case LED_0:
        pin = LED_0_PIN;
        break;
    case LED_1:
        pin = LED_1_PIN;
        break;
    default:
        return;
    }
    gpio_pinMode(pin, OUTPUT);
    if (turnOn) {
        gpio_digitalWrite(pin, LOW);
    } else {
        gpio_digitalWrite(pin, HIGH);
    }
}

