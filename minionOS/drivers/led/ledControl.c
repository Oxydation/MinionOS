/*
 * ledControl.c
 *
 *  Created on: 05.04.2017
 *      Author: Arno Riedmann
 */

#include "ledControl.h"
#include "kernel/hal/led/led.h"

void ledControl_activateLed(bool turnOn, int led) {
    led_activateLed(turnOn, led);
}

