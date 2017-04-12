/*
 * contextSwitch.h
 *
 *  Created on: 12 Apr 2017
 *      Author: Mathias
 */

#ifndef KERNEL_DEVICES_OMAP3530_INCLUDES_CONTEXTSWITCH_H_
#define KERNEL_DEVICES_OMAP3530_INCLUDES_CONTEXTSWITCH_H_

#include <inttypes.h>

typedef struct PCB
{
    uint32_t cpsr;
    uint32_t restartAddress;
    uint32_t R0;
    uint32_t R1;
    uint32_t R2;
    uint32_t R3;
    uint32_t R4;
    uint32_t R5;
    uint32_t R6;
    uint32_t R7;
    uint32_t R8;
    uint32_t R9;
    uint32_t R10;
    uint32_t R11;
    uint32_t R12;
    uint32_t R13;   // SP
    uint32_t R14;   // LR
} PCB_t;

extern void asm_saveContext(PCB_t * pcb);
extern void asm_loadContext(PCB_t * pcb);
extern void asm_continuePreviousProcess();
#endif /* KERNEL_DEVICES_OMAP3530_INCLUDES_CONTEXTSWITCH_H_ */
