#include <kernel/common/Common.h>
#include <stdio.h>
#include <inttypes.h>
#include <kernel/devices/omap3530/includes/Clock.h>
#include <kernel/devices/omap3530/includes/GPIO.h>
#include <kernel/devices/omap3530/includes/Interrupts.h>
#include <kernel/hal/timer/Timer.h>
#include "kernel/hal/gpio/GPIO.h"
#include "kernel/devices/omap3530/includes/BeagleBoardC4.h"
#include "kernel/hal/interrupts/Interrupts.h"

uint8_t on = TRUE;
Timer_t * g_timer1;

void timer_handler1(void)
{
    if (on)
    {
        digitalWrite(GPIO_USR1_LED, HIGH);
        digitalWrite(GPIO_USR2_LED, LOW);
        on = FALSE;
    }
    else
    {
        digitalWrite(GPIO_USR1_LED, LOW);
        digitalWrite(GPIO_USR2_LED, HIGH);
        on = TRUE;
    }

    clear_interrupt_flag(g_timer1);
}

int main(void)
{
    _disable_interrupts();
    init_irq();

    // Set output direction
    pinMode(GPIO_USR1_LED, OUTPUT);
    pinMode(GPIO_USR2_LED, OUTPUT);

    g_timer1 = create_timer(OVERFLOW, AUTORELOAD, 1000 * 500, &timer_handler1);

    _enable_interrupts();
    _enable_IRQ();

    start_timer(g_timer1);

    while (1)
    {

    }
    return 0;
}
