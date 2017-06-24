/*
 * loader.h
 *
 *  Created on: 19.06.2017
 *      Author: sabrinarosmann
 */

#ifndef KERNEL_SYSTEMMODULES_LOADER_LOADER_H_
#define KERNEL_SYSTEMMODULES_LOADER_LOADER_H_

#include <inttypes.h>
#include "applications/systemCallApi.h"
#include "kernel/systemModules/processManagement/processManager.h"
#include "kernel/systemModules/loader/intelHexParser.h"
#include "kernel/systemModules/loader/elfParser.h"

#define LOAD_PROCESS_OK         1
#define NOT_ABLE_TO_LOAD_FILE   -1

#define BUFFER_SIZE     262144 /* 256 KB*/

typedef enum {
    INTEL_HEX, ELF
} FileType_t;

int8_t loader_loadProcess(const char* fileName, FileType_t fileType);

#endif /* KERNEL_SYSTEMMODULES_LOADER_LOADER_H_ */
