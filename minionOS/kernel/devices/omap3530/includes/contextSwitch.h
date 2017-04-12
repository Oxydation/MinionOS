/*
 * ContextSwitch.h
 *
 *  Created on: 09.04.2017
 *      Author: sabrinarosmann
 */

#ifndef KERNEL_DEVICES_OMAP3530_INCLUDES_CONTEXTSWITCH_H_
#define KERNEL_DEVICES_OMAP3530_INCLUDES_CONTEXTSWITCH_H_

#define MODE_USR    0x10
#define MODE_FIQ    0x11
#define MODE_IRQ    0x12
#define MODE_SVC    0x13
#define MODE_ABT    0x17
#define MODE_UND    0x1B
#define MODE_SYS    0x1F

extern void contextSwitch_switchToUserMode(void);

#endif /* KERNEL_DEVICES_OMAP3530_INCLUDES_CONTEXTSWITCH_H_ */
