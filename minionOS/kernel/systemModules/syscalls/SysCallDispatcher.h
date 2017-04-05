/*
 * SysCallDispatcher.h
 *
 *  Created on: 05.04.2017
 *      Author: Jasmin
 */

#ifndef KERNEL_SYSTEMMODULES_SYSCALLS_SYSCALLDISPATCHER_H_
#define KERNEL_SYSTEMMODULES_SYSCALLS_SYSCALLDISPATCHER_H_

typedef struct {
    int a;
    int b;
    int c;
    int d;
} SysCall_Args;

void syscalls_dispatchSysCall(int syscallNumber, SysCall_Args args);

#endif /* KERNEL_SYSTEMMODULES_SYSCALLS_SYSCALLDISPATCHER_H_ */
