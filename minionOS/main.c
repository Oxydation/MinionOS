#include <stdio.h>
#include <inttypes.h>

#include <kernel/common/mmio.h>
#include <kernel/devices/omap3530/includes/beagleBoardC4.h>
#include <kernel/devices/omap3530/includes/modeSwitch.h>
#include <kernel/hal/gpio/gpio.h>
#include <kernel/hal/interrupts/interrupts.h>
#include <kernel/hal/timer/systemTimer.h>
#include "global/types.h"
#include "kernel/systemModules/scheduler/scheduler.h"

void process1(void);
void process2(void);

int main(void)
{
    _disable_interrupts();
    interrupts_initIrq();

    // Set output direction
    gpio_pinMode(GPIO_USR1_LED, OUTPUT);
    gpio_pinMode(GPIO_USR2_LED, OUTPUT);

    systemTimer_init(1000);
    scheduler_init();

    _enable_interrupts();
    _enable_IRQ();

    systemTimer_start();

    //scheduler_startProcess(&process1);
    //scheduler_startProcess(&process2);
    scheduler_start();

    modeSwitch_switchToUserMode();
    while (1)
    {

    }
    return 0;
}

#pragma CODE_SECTION(process1,".process1")
void process1(void)
{
    volatile long i = 0;
    while (1)
    {
        gpio_digitalWrite(GPIO_USR1_LED, LOW);
        for (i = 0; i < 2000000L; i++)
        {

        }
        gpio_digitalWrite(GPIO_USR1_LED, HIGH);
        for (i = 0; i < 2000000L; i++)
        {

        }
    }
}

#pragma CODE_SECTION(process2,".process2")
void process2(void)
{
    volatile long i = 0;
    while (1)
    {
        gpio_digitalWrite(GPIO_USR2_LED, LOW);
        for (i = 0; i < 2000000L; i++)
        {

        }
        gpio_digitalWrite(GPIO_USR2_LED, HIGH);
        for (i = 0; i < 2000000L; i++)
        {

        }
    }
}
