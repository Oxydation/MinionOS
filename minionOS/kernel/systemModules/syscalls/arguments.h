/*
 * arguments.h
 *
 *  Created on: 19.04.2017
 *      Author: Jasmin
 */

#ifndef KERNEL_SYSTEMMODULES_SYSCALLS_ARGUMENTS_H_
#define KERNEL_SYSTEMMODULES_SYSCALLS_ARGUMENTS_H_

typedef struct {
    int syscallNumber;
    int a;
    int b;
    int c;
} SysCallArgs_t;

#endif /* KERNEL_SYSTEMMODULES_SYSCALLS_ARGUMENTS_H_ */
