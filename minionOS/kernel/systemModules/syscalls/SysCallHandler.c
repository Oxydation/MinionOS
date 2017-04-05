/*
 * SysCallHandler.c
 *
 *  Created on: 05.04.2017
 *      Author: Jasmin
 */
#include "SysCallHandler.h"
#include "SysCallNumbers.h"

void handleSysCall(int swi_number, SysCall_Args args){
    switch(swi_number){
    case LED_SYSCALL:
        //TODO
        break;
    default:
        //TODO
        break;
    }
}

