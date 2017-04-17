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
#include "kernel/systemModules/processManagement/processManager.h"

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

    int * var = &process1;
    int * var2 = & process2;

    processManager_loadProcess(0x80600000, 0x80601000);
    //processManager_loadProcess(0x80602000, 0x80603000);

    scheduler_start();

    modeSwitch_switchToUserMode();
    while (1)
    {

    }
    return 0;
}

#pragma CODE_SECTION(process1,".process1") // DDR0_PROC1: o = 0x81FF0000
void process1(void)
{
    volatile unsigned long i = 0;
    uint32_t* out = (uint32_t*) (GPIO_BASE_ADDR(GPIO_USR1_LED) + GPIO_DATAOUT);

    while(1){
        bitSet(*out, GPIO_PIN_POS(GPIO_USR1_LED));
        for (i = 0; i < 400000L; i++)
        {

        }
        bitClear(*out, GPIO_PIN_POS(GPIO_USR1_LED));
        for (i = 0; i < 400000L; i++)
        {

        }
    }
}

#pragma CODE_SECTION(process2,".process2") //  DDR0_PROC2: o = 0x82FF1000
void process2(void)
{
    volatile long i = 0;
    while (1)
    {
        gpio_digitalWrite(GPIO_USR2_LED, LOW);
        for (i = 0; i < 200000L; i++)
        {

        }
        gpio_digitalWrite(GPIO_USR2_LED, HIGH);
        for (i = 0; i < 200000L; i++)
        {

        }
    }
}
