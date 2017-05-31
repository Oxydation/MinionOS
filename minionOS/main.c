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
#include "kernel/hal/dmx/dmx.h"

void process1(void);
void process2(void);

int main(void)
{
    _disable_interrupts();
    interrupts_initIrq();

    // Set output direction
    gpio_pinMode(GPIO_USR0_LED, OUTPUT);
    gpio_pinMode(GPIO_USR1_LED, OUTPUT);

    systemTimer_init(1000);
    scheduler_init();

    processManager_loadProcess(&process1 + 0x4, 0x80607500);
    //processManager_loadProcess(&process2 + 0x4, 0x8060FF00);

    //processManager_startFirstProcess();

    _enable_interrupts();
    _enable_IRQ();

    systemTimer_start();
    scheduler_start();

    modeSwitch_switchToUserMode();


    while (1)
    {

    }
    return 0;
}

#pragma CODE_SECTION(process1,".process1") // DDR0_PROC1: o = 0x80600000
void process1(void)
{
   main_dmx();
}

#pragma CODE_SECTION(process2,".process2") //  DDR0_PROC2: o = 0x80608000
void process2(void)
{
    volatile unsigned long i = 0;
    uint32_t* out = (uint32_t*) (GPIO_BASE_ADDR(GPIO_USR1_LED) + GPIO_DATAOUT);

    while (1)
    {
        bitClear(*out, GPIO_PIN_POS(GPIO_USR1_LED));
    }
}
