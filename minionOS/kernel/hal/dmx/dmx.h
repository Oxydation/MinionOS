/*
 * dmx.h
 *
 *  Created on: 22.05.2017
 *      Author: Jasmin
 */

#ifndef KERNEL_HAL_DMX_DMX_H_
#define KERNEL_HAL_DMX_DMX_H_
#include "global/types.h"

// DMX pins
#define DMX_DI_PIN 6    // DI Driver Input of ST485
#define DMX_DE_PIN 8    // DE Driver Output Enable of ST485
#define DMX_CHANNELS 25 // 1 StartCode, 11 Channels

// Pad configuration addresses
#define PADCONF_BASE 0x48002030
#define CONTROL_PADCONF_UART2_TX 0x48002178
#define CONTROL_PADCONF_UART2_CTS 0x48002174

void dmx_init();
void dmx_send(int startChannel, const uint8_t * data, uint8_t size);
void dmx_writeToChannel(uint8_t * data, int channel, uint8_t value);

#endif /* KERNEL_HAL_DMX_DMX_H_ */
