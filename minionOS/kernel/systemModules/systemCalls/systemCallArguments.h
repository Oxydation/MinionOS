#ifndef KERNEL_SYSTEMMODULES_SYSTEMCALLS_SYSTEMCALLARGUMENTS_H_
#define KERNEL_SYSTEMMODULES_SYSTEMCALLS_SYSTEMCALLARGUMENTS_H_

#include "systemCallNumber.h"

typedef struct {
    SystemCallNumber systemCallNumber;
    int a;
    int b;
    int c;
} SysCallArgs_t;

#endif /* KERNEL_SYSTEMMODULES_SYSTEMCALLS_SYSTEMCALLARGUMENTS_H_ */
