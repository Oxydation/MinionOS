#include "kernel/systemModules/systemCalls/systemCallArguments.h"
#include "kernel/systemModules/systemCalls/systemCallNumber.h"
#include "applications/systemCallApi.h"

#define UNUSED_SWI_NUMBER 0

#pragma SWI_ALIAS(sysCall, UNUSED_SWI_NUMBER);
static int sysCall(SysCallArgs_t args);

int sysCalls_enableLed(bool turnOn, int led) {
    SysCallArgs_t args = { LED_SYSCALL, turnOn, led };
    sysCall(args);
}

