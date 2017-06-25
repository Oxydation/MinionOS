/*
 * delay.c
 *
 *  Created on: 7 Jun 2017
 *      Author: Mathias
 */
#include "delay.h"

void delay(int amount)
{
    volatile uint32_t i = 0;
    for (i = 0; i < amount; i++)
        ;
}
