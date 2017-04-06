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
#include "kernel/hal/timer/SystemTimer.h"

#define USE_SYSTEMTMR 1 // uncomment if you want to test system timer; comment if you want to use bare timers

uint8_t on1 = TRUE;
uint8_t on2 = TRUE;

Timer_t * g_timer1;
Timer_t * g_timer2;

void timer_handler1(void)
{
    if (on1)
    {
        digitalWrite(GPIO_USR1_LED, HIGH);
        on1 = FALSE;
    }
    else
    {
        digitalWrite(GPIO_USR1_LED, LOW);
        on1 = TRUE;
    }

#ifndef USE_SYSTEMTMR
    clear_interrupt_flag(g_timer1);
#endif
}

void timer_handler2(void)
{
    if (on2)
    {
        digitalWrite(GPIO_USR2_LED, HIGH);
        on2 = FALSE;
    }
    else
    {
        digitalWrite(GPIO_USR2_LED, LOW);
        on2 = TRUE;
    }
#ifndef USE_SYSTEMTMR
    clear_interrupt_flag(g_timer2);
#endif
}
int main(void)
{
    _disable_interrupts();
    init_irq();

    // Set output direction
    pinMode(GPIO_USR1_LED, OUTPUT);
    pinMode(GPIO_USR2_LED, OUTPUT);
#ifdef USE_SYSTEMTMR
    init_system_timer(1000);
#else
     g_timer1 = create_timer(OVERFLOW, AUTORELOAD, 1000 * 1000, &timer_handler1);
     g_timer2 = create_timer(OVERFLOW, AUTORELOAD, 1000 * 500, &timer_handler2);
#endif
    _enable_interrupts();
    _enable_IRQ();

#ifdef USE_SYSTEMTMR
    //Start system timer and subscribe handlers
    start_system_timer();
    subscribe_systimer_callback(1000, &timer_handler1);
    subscribe_systimer_callback(500, &timer_handler2);
#else
    start_timer(g_timer1);
    start_timer(g_timer2);
#endif

    while (1)
    {

    }
    return 0;
}
