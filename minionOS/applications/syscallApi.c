/*
 * syscalls.c
 *
 *  Created on: 19.04.2017
 *      Author: Jasmin
 */
#include "applications/syscallApi.h"
#include "kernel/systemModules/syscalls/arguments.h"
#include "kernel/systemModules/syscalls/syscallNumbers.h"

#pragma SWI_ALIAS(syscall, LED_SYSCALL);
static int syscall(SysCall_Args args);

int syscalls_enableLed(bool turnOn, int led) {
    SysCall_Args args = { LED_SYSCALL, turnOn, led };
    syscall(args);
}

