#include "systemCallNumber.h"
#include "kernel/systemModules/systemCalls/dispatcher.h"
#include "drivers/led/ledControl.h"
#include "kernel/systemModules/filesystem/vfs.h"
#include "kernel/hal/dmx/dmx.h"
#include "drivers/dmx/tmh7/dmxTmh7.h"
#include "drivers/dmx/mhx25/dmxMhx25.h"
#include "kernel/systemModules/scheduler/semaphore.h"

int dispatcher_dispatch(SysCallArgs_t args) {
    switch (args.systemCallNumber) {
    case SYSCALL_DMX_SEND:
        dmx_send((uint8_t*) args.a, args.b);
        return 0;
    case SYSCALL_LED:
        ledControl_activateLed((bool) args.a, args.b);
        return 0;
    case SYSCALL_FILE_OPEN:
        return vfs_open((const char*) args.a);
    case SYSCALL_FILE_READ:
        return vfs_read(args.a, (uint8_t*) args.b, args.c);
    case SYSCALL_FILE_WRITE:
        vfs_write(args.a, (const uint8_t*) args.b, args.c);
        return 0;
    case SYSCALL_FILE_CLOSE:
        vfs_close(args.a);
        return 0;
    case SYSCALL_READDIR:
        return (int) vfs_readdir((const char*) args.a);
    case SYSCALL_SEMAPHORE_INIT:
        semaphore_managedInit((const char*) args.a, args.b);
        return 0;
    case SYSCALL_SEMAPHORE_P:
        semaphore_managedP((const char*) args.a);
        return 0;
    case SYSCALL_SEMAPHORE_V:
        semaphore_managedV((const char*) args.a);
        return 0;
    }
    return -1;
}

