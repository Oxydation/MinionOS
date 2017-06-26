/*
 * ledControl.c
 *
 *  Created on: 05.04.2017
 *      Author: Arno Riedmann
 */

#include "ledControl.h"
#include "systemCallApi.h"

void ledControl_activateLed(bool turnOn, int led) {
    sysCalls_enableLed(turnOn, led);
}

