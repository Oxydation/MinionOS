#include "kernel/systemModules/filesystem/vfs.h"
#include "kernel/devices/omap3530/includes/modeSwitch.h"
#include "applications/shell/shell.h"

void main(void) {
    vfs_init();

    modeSwitch_switchToUserMode();

    shell_loop();
}
