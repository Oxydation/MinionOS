/*
 * main.c
 */

#include "systemCallApi.h"

int main(void) {
    int file = sysCalls_openFile("/dev/led1");
    while (1) {
        sysCalls_writeFile(file, "1", 1);
    }
}

