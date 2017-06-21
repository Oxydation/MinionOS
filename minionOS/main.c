#include <stdio.h>
#include <inttypes.h>

#include <kernel/common/mmio.h>
#include <kernel/devices/omap3530/includes/beagleBoardC4.h>
#include <kernel/devices/omap3530/includes/modeSwitch.h>
#include <kernel/hal/gpio/gpio.h>
#include <kernel/hal/interrupts/interrupts.h>
#include <kernel/hal/timer/systemTimer.h>
#include <kernel/hal/timer/timer.h>
#include <kernel/systemModules/mmu/mmu.h>
#include "global/types.h"
#include "kernel/systemModules/scheduler/scheduler.h"
#include "kernel/systemModules/processManagement/processManager.h"

#define USE_SYSTEMTMR   1

void process1(void);
void process2(void);
void process3(void);
static int useLoopInProcesses = 0;

int main(void)
{
    _disable_interrupts();

    process3();
    process2();
    process1();

    useLoopInProcesses = 0;

    mmu_initMMU();

    interrupts_initIrq();

    // Set output direction
    gpio_pinMode(GPIO_USR0_LED, OUTPUT);
    gpio_pinMode(GPIO_USR1_LED, OUTPUT);

    systemTimer_init(1000);
    scheduler_init();

    /* physicalStartAddress, nrOfNeededBytes */
    processManager_loadProcess(0x80600000, 1000);
    processManager_loadProcess(0x80700000, 1000);
    processManager_loadProcess(0x80800000, 1000);

    _enable_interrupts();
    _enable_IRQ();

    systemTimer_start();
    scheduler_start();

    modeSwitch_switchToUserMode();

    while (1)
    {

    }
}

#pragma CODE_SECTION(process1,".process1") // DDR0_PROC1: o = 0x80600000
void process1(void)
{
    volatile unsigned long i = 0;
    uint32_t* out = (uint32_t*) (GPIO_BASE_ADDR(GPIO_USR1_LED) + GPIO_DATAOUT);

    if (useLoopInProcesses == 1) {
        while (1) {
            bitSet(*out, GPIO_PIN_POS(GPIO_USR1_LED));
        }
    }
    else
    {
        bitSet(*out, GPIO_PIN_POS(GPIO_USR1_LED));
    }

}

#pragma CODE_SECTION(process2,".process2") //  DDR0_PROC2: o = 0x80700000
void process2(void)
{
    volatile unsigned long i = 0;
    uint32_t* out = (uint32_t*) (GPIO_BASE_ADDR(GPIO_USR1_LED) + GPIO_DATAOUT);

    if (useLoopInProcesses == 1) {
        while (1) {
            bitClear(*out, GPIO_PIN_POS(GPIO_USR1_LED));
        }
    }
    else
    {
        bitClear(*out, GPIO_PIN_POS(GPIO_USR1_LED));
    }
}

#pragma CODE_SECTION(process3,".process3") //  DDR0_PROC3: o = 0x80800000
void process3(void)
{
    volatile unsigned long i = 0;
    uint32_t* out = (uint32_t*) (GPIO_BASE_ADDR(GPIO_USR1_LED) + GPIO_DATAOUT);

    if (useLoopInProcesses == 1) {
        while (1) {
            bitClear(*out, GPIO_PIN_POS(GPIO_USR1_LED));
        }
    }
    else
    {
        bitClear(*out, GPIO_PIN_POS(GPIO_USR1_LED));
    }
}
