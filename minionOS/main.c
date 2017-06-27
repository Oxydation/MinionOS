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
#include "kernel/systemModules/filesystem/vfs.h"
#include "kernel/hal/mmc_sd/sdCard.h"
#include "kernel/hal/uart/uart.h"
#include "kernel/systemModules/loader/loader.h"
#include "systemCallApi.h"

UartConfig_t g_uartConfig = { .baudMultiple = x16, .baudRate = 115200,
                                .stopMode = STOP_1, .parityMode = NO_PARITY,
                                .wordLength = LENGTH_8 };

int main(void)
{
    _disable_interrupts();

    mmu_initMMU();

    interrupts_initIrq();

    // Set output direction
    gpio_pinMode(GPIO_USR0_LED, OUTPUT);
    gpio_pinMode(GPIO_USR1_LED, OUTPUT);

    uart_initModule(UART3, g_uartConfig);
    sdCard_initialize_Ch1();
    vfs_init();
    systemTimer_init(1000);
    scheduler_init();

    sysCalls_enableLed(1, LED_0);
    sysCalls_enableLed(1, LED_1);

    loader_loadProcess("/LEDON.OUT", ELF);
    loader_loadProcess("/LEDOFF.OUT", ELF);
    loader_loadProcess("/SHELL.OUT", ELF);
    loader_loadProcess("/GAME123.OUT", ELF);

    _enable_interrupts();
    _enable_IRQ();

    systemTimer_start();
    scheduler_start();

    modeSwitch_switchToUserMode();

    while (1)
    {

    }
}
