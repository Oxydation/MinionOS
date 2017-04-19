/*
 * dispatcher.h
 *
 *  Created on: 05.04.2017
 *      Author: Jasmin
 */

#ifndef KERNEL_SYSTEMMODULES_SYSCALLS_DISPATCHER_H_
#define KERNEL_SYSTEMMODULES_SYSCALLS_DISPATCHER_H_

#include "arguments.h"

void dispatcher_dispatch(SysCall_Args args);

#endif /* KERNEL_SYSTEMMODULES_SYSCALLS_DISPATCHER_H_ */
