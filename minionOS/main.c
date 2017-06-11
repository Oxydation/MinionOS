#include "kernel/systemModules/filesystem/vfs.h"
#include "kernel/devices/omap3530/includes/modeSwitch.h"
#include "applications/shell/shell.h"
#include "kernel/hal/mmc_sd/sdCard.h"

void main(void) {
    sdCard_initialize_Ch1();
    vfs_init();

    modeSwitch_switchToUserMode();

    shell_loop();
}
