/*
 * dispatcher.c
 *
 *  Created on: 05.04.2017
 *      Author: Jasmin
 */
#include <kernel/systemModules/syscalls/dispatcher.h>
#include <kernel/systemModules/syscalls/syscallNumbers.h>
#include "drivers/led/ledControl.h"

void dispatcher_dispatch(SysCallArgs_t args) {
    switch (args.syscallNumber) {
    case LED_SYSCALL:
        ledControl_activateLed((bool) args.a, args.b);
        break;
    default:
        //TODO
        break;
    }
}

