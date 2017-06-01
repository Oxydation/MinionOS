#include "kernel/systemModules/systemCalls/systemCallNumber.h"
#include "kernel/systemModules/systemCalls/dispatcher.h"
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

