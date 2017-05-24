#include <stdio.h>
#include <inttypes.h>

#include "kernel/hal/uart/uart.h"

int main(void) {
    UartConfig_t config = {};
    initModule(UART3, config);
    uint8_t in;
    while (1) {
        receive(UART3, &in, 1);
        transmit(UART3, &in, 1);
    }
}
