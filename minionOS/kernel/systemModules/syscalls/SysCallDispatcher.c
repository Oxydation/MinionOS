/*
 * SysCallDispatcher.c
 *
 *  Created on: 05.04.2017
 *      Author: Jasmin
 */
#include "kernel/systemModules/syscalls/SysCallDispatcher.h"
#include "SysCallNumbers.h"
#include "drivers/led/LedControl.h"

void syscalls_dispatchSysCall(int syscallNumber, SysCall_Args args){
    switch(syscallNumber){
    case LED_SYSCALL:
        led_control((bool) args.a, args.b);
        break;
    default:
        //TODO
        break;
    }
}

