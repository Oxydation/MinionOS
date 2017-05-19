#include <stdio.h>
#include <inttypes.h>

#include "kernel/hal/uart/uart.h"

int main(void) {
    initModule(UART3, 0);
    char in;
    while (1) {
        receive(UART3, &in, 1);
        transmit(UART3, &in, 1);
    }
}
