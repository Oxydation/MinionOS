/*
 * abstractStorage.h
 *
 *  Created on: 20.05.2017
 *      Author: benimurza
 *
 *      Provide an "interface"-like header for abstract storage functions.
 */

#ifndef KERNEL_SYSTEMMODULES_FILESYSTEM_ABSTRACTSTORAGE_H_
#define KERNEL_SYSTEMMODULES_FILESYSTEM_ABSTRACTSTORAGE_H_

#include <inttypes.h>

// Better: function pointers
uint32_t readSector(uint8_t * buf, uint32_t address);

#endif /* KERNEL_SYSTEMMODULES_FILESYSTEM_ABSTRACTSTORAGE_H_ */
