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

    /*or32(CONTROL_PADCONF_UART2_TX, 0x03<<3);
    or32(CONTROL_PADCONF_UART2_TX, 0x03<<19);
    or32(CONTROL_PADCONF_UART2_CTS, 0x03<<3);
    or32(CONTROL_PADCONF_UART2_CTS, 0x03<<19);*/

   /* or32(CONTROL_PADCONF_UART2_TX, 0x01<<8);
    or32(CONTROL_PADCONF_UART2_TX, 0x01<<24);
    or32(CONTROL_PADCONF_UART2_CTS, 0x01<<8);
    or32(CONTROL_PADCONF_UART2_CTS, 0x01<<24);*/
/*
    UartConfig_t config = {.baudMultiple = x16,
                           .baudRate = 250000,
                           .stopMode = STOP_1,
                           .parityMode = NO_PARITY,
                           .wordLength = LENGTH_8 };
    initModule(UART2, config);
//    enableBreak(UART2);

    uint8_t in = 'a';
    int i = 0;
//    while (i < 100) {
//       // receive(UART3, &in, 1);
//        transmit(UART2, &in, 1);
//        i++;
//    }
//    disableBreak(UART2);
    i = 0;
       while (1) {
          // receive(UART3, &in, 1);
           transmit(UART2, &in, 1);
           i++;
       }*/
}
