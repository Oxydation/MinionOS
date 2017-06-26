/*
 * uart.c
 *
 *  Created on: 26 Jun 2017
 *      Author: Mathias
 */

#include <kernel/hal/uart/uart.h>
#include "kernel/devices/omap3530/includes/uart.h"
#include "kernel/devices/omap3530/includes/interrupts.h"

extern UartModule_t uart_getModuleFromIrqSource(uint8_t source)
{
    switch (source)
    {
    case UART1_IRQ:
        return UART1;
    case UART2_IRQ:
        return UART2;
    case UART3_IRQ:
        return UART3;
    }

    return UART3;
}

