/*
 * SysCallHandler.h
 *
 *  Created on: 05.04.2017
 *      Author: Jasmin
 */

#ifndef KERNEL_SYSTEMMODULES_SYSCALLS_SYSCALLHANDLER_H_
#define KERNEL_SYSTEMMODULES_SYSCALLS_SYSCALLHANDLER_H_

typedef struct {
    int a;
    int b;
    int c;
    int d;
}SysCall_Args;

void handleSysCall(int swi_number, SysCall_Args args);

#endif /* KERNEL_SYSTEMMODULES_SYSCALLS_SYSCALLHANDLER_H_ */
