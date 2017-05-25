#include <stdio.h>
#include <inttypes.h>

#include "kernel/hal/uart/uart.h"
#include "kernel/hal/dmx/dmx.h"
#include "kernel/common/mmio.h"
#include "kernel/devices/omap3530/includes/gpio.h"

#define PADCONF_BASE 0x48002030
#define CONTROL_PADCONF_UART2_TX 0x48002178
#define CONTROL_PADCONF_UART2_CTS 0x48002174

int main(void) {
   //main_dmx();
    set32(CONTROL_PADCONF_UART2_TX, 0x00<<0);
    set32(CONTROL_PADCONF_UART2_CTS, 0x00<<16);

    main_dmx();
}
