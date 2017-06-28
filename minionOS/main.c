#include <inttypes.h>
#include "kernel/devices/omap3530/includes/modeSwitch.h"
#include "kernel/hal/interrupts/interrupts.h"
#include "kernel/hal/timer/systemTimer.h"
#include "kernel/systemModules/mmu/mmu.h"
#include "kernel/hal/dmx/dmx.h"
#include "kernel/systemModules/scheduler/scheduler.h"
#include "kernel/systemModules/filesystem/vfs.h"
#include "kernel/systemModules/loader/loader.h"
#include "systemCallApi.h"

int main(void)
{
    _disable_interrupts();

    mmu_initMMU();

    interrupts_initIrq();

    dmx_init();
    vfs_init();
    systemTimer_init(1000);
    scheduler_init();

    sysCalls_enableLed(false, LED_0);
    sysCalls_enableLed(false, LED_1);

    //loader_loadProcess("/LEDON.OUT", ELF);
    //loader_loadProcess("/LEDOFF.OUT", ELF);
    loader_loadProcess("/SHELL.OUT", ELF);
    //loader_loadProcess("/GAME123.OUT", ELF);

    _enable_interrupts();
    _enable_IRQ();

    systemTimer_start();
    scheduler_start();

    modeSwitch_switchToUserMode();

    while (1)
    {

    }
}
