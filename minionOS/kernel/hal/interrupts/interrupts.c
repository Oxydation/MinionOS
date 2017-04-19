/*
 * Interrupts.c
 *
 *  Created on: 18 Mar 2017
 *      Author: Mathias
 *  Description: Provides common interrupt initialization and handling.
 *  Important information: The stack must be set up to handle interrupts.
 */

#include <kernel/common/mmio.h>
#include <kernel/devices/omap3530/includes/interrupts.h>
#include <kernel/hal/interrupts/interrupts.h>
#include <kernel/systemModules/sysCalls/arguments.h>
#include <kernel/systemModules/sysCalls/dispatcher.h>
#include <stdio.h>
#include "global/types.h"

// Keep book of all interrupt handlers
static InterruptHandler_t g_interruptHandlers[NROF_IR_VECTORS] = { 0 };

/*
 * Registers a new interrupt handler at a given IRQ-Position.
 */
void interrupts_registerHandler(InterruptHandler_t handler, uint8_t irq_nr) {
    interrupts_enableIrqSource(irq_nr);
    g_interruptHandlers[irq_nr] = handler; // set handler at given irq
}

void enable_interrupts() {
    _enable_interrupts();
}

void disable_interrupts() {
    _disable_interrupts();
}

#pragma SET_CODE_SECTION(".ISR")
#pragma INTERRUPT (isr_irq, IRQ)
void isr_irq(void) {
    //__asm(" STMFD SP!, {R0-R12, LR}"); // save critical context (Register, link register, spsr)
    //__asm(" MRS R11, SPSR");

    uint32_t activeIrq = (*(Address_t) (INTCPS_SIR_IRQ)) & INTCPS_SIR_IRQ_MASK;
    InterruptHandler_t handler = g_interruptHandlers[activeIrq];
    if (handler != 0)
        handler(activeIrq); // call handler if set

    // Clear IRQ interrupt output
    (*(Address_t) (INTCPS_CONTROL)) |= INTCPS_CONTROL_NEWIRQAGR;

    // Data syncronization Barrier
    __asm(" MOV R0, #0");
    __asm(" MCR P15, #0, R0, C7, C10, #4");

    // Restore critical context
    // __asm(" MSR SPSR, R11");
    // __asm(" LDMFD SP!, {R0-R12, LR}");

    // Reenable interrupts
    //_enable_interrupts();
    //_enable_IRQ();

    // __asm(" SUBS PC,LR,#4;");
}

#pragma INTERRUPT (isr_reset, RESET)
void isr_reset(void) {

}

#pragma INTERRUPT (isr_swi, SWI)
void isr_swi(SysCallArgs_t args) {
    dispatcher_dispatch(args);
}

#pragma INTERRUPT (isr_fiq, FIQ)
void isr_fiq(void) {

}

#pragma INTERRUPT (isr_undef, UDEF)
void isr_undef(void) {

}
#pragma INTERRUPT (isr_undef, DABT)
void isr_dabt(void) {

}

#pragma INTERRUPT (isr_undef, PABT)
void isr_pabt(void) {

}
