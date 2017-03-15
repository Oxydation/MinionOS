/*
 * Omap3530GPIO.c
 *
 *  Created on: 15 Mar 2017
 *      Author: Mathias
 */

#include "Omap3530GPIO.h"
#include "Common.h"

void digitalWrite(uint8_t pin, uint8_t val)
{
    uint64_t* out = GPIO_BASE_ADDR(pin) + GPIO_DATAOUT;

    if (val == LOW)
    {
        bitSet(*out, GPIO_PIN_POS(pin));
    }
    else
    {
        bitClear(*out, GPIO_PIN_POS(pin));
    }
}

uint8_t digitalRead(uint8_t pin)
{
    uint64_t* in = GPIO_BASE_ADDR(pin) + GPIO_DATAIN;

    if (*in & GPIO_PIN_POS(pin))
    {
        return HIGH;
    }
    else
    {
        return LOW;
    }
}

void pinMode(uint8_t pin, uint8_t mode)
{
    uint64_t* gpio_dir_ctr = GPIO_BASE_ADDR(pin) + GPIO_OE;

    if (mode == INPUT)
    {
        //*gpio_dir_ctr |= GPIO_PIN_POS(pin);
        bitSet(*gpio_dir_ctr, GPIO_PIN_POS(pin));
    }
    else
    {
        bitClear(*gpio_dir_ctr, GPIO_PIN_POS(pin));
    }
}

