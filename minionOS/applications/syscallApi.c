/*
 * syscalls.c
 *
 *  Created on: 19.04.2017
 *      Author: Jasmin
 */
#include <applications/sysCallApi.h>
#include <kernel/systemModules/sysCalls/arguments.h>
#include <kernel/systemModules/sysCalls/sysCallNumbers.h>

#define UNUSED_SWI_NUMBER 0

#pragma SWI_ALIAS(sysCall, UNUSED_SWI_NUMBER);
static int sysCall(SysCallArgs_t args);

int sysCalls_enableLed(bool turnOn, int led) {
    SysCallArgs_t args = { LED_SYSCALL, turnOn, led };
    sysCall(args);
}

