/*
 * ContextSwitch.c
 *
 *  Created on: 09.04.2017
 *      Author: sabrinarosmann
 */

#include <inttypes.h>
#include "kernel/devices/omap3530/includes/contextSwitch.h"

void contextSwitch_switchToUserMode(void)
{
     asm volatile (
             " MOV R1, LR \n"           // save contents of link register (return address)
             " MSR CPSR_c, #0x10 \n"    // set the mode to user-mode
             " MOV LR, R1 \n"           // write saved return address to link register
     );
}





