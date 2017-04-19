#include <stdio.h>
#include <inttypes.h>

#include <kernel/common/mmio.h>
#include <kernel/devices/omap3530/includes/beagleBoardC4.h>
#include <kernel/devices/omap3530/includes/modeSwitch.h>
#include <kernel/hal/gpio/gpio.h>
#include <kernel/hal/interrupts/interrupts.h>
#include <kernel/hal/timer/systemTimer.h>
#include <kernel/hal/timer/timer.h>
#include "global/types.h"

#define USE_SYSTEMTMR 1 // uncomment if you want to test system timer; comment if you want to use bare timers

uint8_t g_on1 = TRUE;
uint8_t g_on2 = TRUE;

Timer_t * g_timer1;
Timer_t * g_timer2;

void timerHandler1(void)
{
    if (g_on1)
    {
        gpio_digitalWrite(GPIO_USR1_LED, HIGH);
        g_on1 = FALSE;
    }
    else
    {
        gpio_digitalWrite(GPIO_USR1_LED, LOW);
        g_on1 = TRUE;
    }

#ifndef USE_SYSTEMTMR
    timer_clearInterruptFlag(g_timer1);
#endif
}

void timerHandler2(void)
{
    if (g_on2)
    {
        gpio_digitalWrite(GPIO_USR1_LED, HIGH);
        g_on2 = FALSE;
    }
    else
    {
        gpio_digitalWrite(GPIO_USR1_LED, LOW);
        g_on2 = TRUE;
    }

#ifndef USE_SYSTEMTMR
    timer_clearInterruptFlag(g_timer2);
#endif
}
int main(void)
{
    _disable_interrupts();
    interrupts_initIrq();

    // Set output direction
    gpio_pinMode(GPIO_USR0_LED, OUTPUT);
    gpio_pinMode(GPIO_USR1_LED, OUTPUT);

#ifdef USE_SYSTEMTMR
    systemTimer_init(1000);
#else
    g_timer1 = timer_create(OVERFLOW, AUTORELOAD, 1000 * 1000, &timerHandler1);
    g_timer2 = timer_create(OVERFLOW, AUTORELOAD, 1000 * 500, &timerHandler2);
#endif
    _enable_interrupts();
    _enable_IRQ();

#ifdef USE_SYSTEMTMR
    //Start system timer and subscribe handlers
    systemTimer_start();
    systemTimer_subscribeCallback(1000, &timerHandler1);
    systemTimer_subscribeCallback(500, &timerHandler2);
#else
    timer_start(g_timer1);
    timer_start(g_timer2);
#endif

    modeSwitch_switchToUserMode();
    while (1)
    {

    }
    return 0;
}
