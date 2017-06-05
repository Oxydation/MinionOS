#include <stdio.h>
#include <inttypes.h>

#include <kernel/common/mmio.h>
#include <kernel/devices/omap3530/includes/beagleBoardC4.h>
#include <kernel/devices/omap3530/includes/modeSwitch.h>
#include <kernel/hal/gpio/gpio.h>
#include <kernel/hal/interrupts/interrupts.h>
#include <kernel/hal/timer/systemTimer.h>
#include <kernel/hal/timer/timer.h>
#include <kernel/devices/omap3530/includes/mmu.h>
#include "global/types.h"
#include "kernel/systemModules/scheduler/scheduler.h"
#include "kernel/systemModules/processManagement/processManager.h"
#include "applications/syscallApi.h"

#define USE_SYSTEMTMR   1

void process1(void);
void process2(void);
void mmu_startProcess(uint32_t address);

int main(void)
{
    _disable_interrupts();

    mmu_initMMU();

    interrupts_initIrq();

    // Set output direction
    gpio_pinMode(GPIO_USR0_LED, OUTPUT);
    gpio_pinMode(GPIO_USR1_LED, OUTPUT);

    systemTimer_init(1000);
    scheduler_init();

    mmu_initProcessOne();
    mmu_initProcessTwo();
    mmu_startProcess(0x20000000);

    //processManager_loadProcess(&process1+0x4, 0x806FFC00);
    //processManager_loadProcess(&process2+0x4, 0x807FFC00);

    //processManager_startFirstProcess();

    _enable_interrupts();
    _enable_IRQ();

    systemTimer_start();
    scheduler_start();

    //mmu_startProcess(0x00000000);
    //mmu_startProcess(0x20000000);

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

    //while (1)
    //{
        bitSet(*out, GPIO_PIN_POS(GPIO_USR1_LED));
    //}
}

#pragma CODE_SECTION(process2,".process2") //  DDR0_PROC2: o = 0x80700000
void process2(void)
{
    volatile unsigned long i = 0;
    uint32_t* out = (uint32_t*) (GPIO_BASE_ADDR(GPIO_USR1_LED) + GPIO_DATAOUT);

    //while (1)
    //{
        bitClear(*out, GPIO_PIN_POS(GPIO_USR1_LED));
    //}
}
