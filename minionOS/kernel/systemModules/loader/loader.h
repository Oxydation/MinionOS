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

#define LOAD_PROCESS_OK         1
#define NOT_ABLE_TO_LOAD_FILE   -1

#define BUFFER_SIZE     524288

int8_t loader_loadProcess(const char* fileName);

#endif /* KERNEL_SYSTEMMODULES_LOADER_LOADER_H_ */
