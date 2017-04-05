/*
 * Interrupts.c
 *
 *  Created on: 18 Mar 2017
 *      Author: Mathias
 *  Description: Provides common interrupt initialization and handling.
 *  Important information: The stack must be set up to handle interrupts.
 */

#include <kernel/devices/omap3530/includes/Interrupts.h>
#include <stdio.h>
#include "kernel/hal/interrupts/Interrupts.h"
#include "kernel/common/Common.h"

// Keep book of all interrupt handlers
static InterruptHandler_t gInterruptHandlers[NROF_IR_VECTORS] = { 0 };

/*
 * Registers a new interrupt handler at a given IRQ-Position.
 */
void register_interrupt_handler(InterruptHandler_t handler, uint8_t irq_nr)
{
    enable_irq_source(irq_nr);
    gInterruptHandlers[irq_nr] = handler; // set handler at given irq
}

#pragma SET_CODE_SECTION(".ISR")
#pragma INTERRUPT (isr_irq, IRQ)
void isr_irq(void)
{
    // Call the dispatcher of the interrupts
    // dispatch_interrupts();

    //__asm(" STMFD SP!, {R0-R12, LR}"); // save critical context (Register, link register, spsr)
    //__asm(" MRS R11, SPSR");

    // Second version
    // Gets the current IRQ status
//    uint32_t status[3];
//    status[0] = (*(Address_t) (INTCPS_PENDING_IRQ(0)));
//    status[1] = (*(Address_t) (INTCPS_PENDING_IRQ(1)));
//    status[2] = (*(Address_t) (INTCPS_PENDING_IRQ(2)));
//
//    int i = 0;
//    for (i = 0; i < NROF_IR_VECTORS; i++)
//    {
//        if (status[i / 32] & (1u << i%32))
//        {
//            InterruptHandler_t handler = gInterruptHandlers[i];
//                if (handler != 0)
//                    handler(i); // call handler if set
//                // Clear IRQ interrupt output
//                    (*(Address_t) (INTCPS_CONTROL)) |= INTCPS_CONTROL_NEWIRQAGR;
//        }
//    }

    // First version ----
    uint32_t activeIRQ = (*(Address_t) (INTCPS_SIR_IRQ)) & INTCPS_SIR_IRQ_MASK;
    InterruptHandler_t handler = gInterruptHandlers[activeIRQ];
    if (handler != 0)
        handler(activeIRQ); // call handler if set

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
void isr_reset(void)
{

}

#pragma INTERRUPT (isr_swi, SWI)
void isr_swi(void)
{
// Four arguments can be passed through R0 - R3
// Structures uses the R0 (with address)
// Float uses two registers
    __asm(" MOVS PC,R14;");
}

#pragma INTERRUPT (isr_fiq, FIQ)
void isr_fiq(void)
{

}

#pragma INTERRUPT (isr_undef, UDEF)
void isr_undef(void)
{

}
#pragma INTERRUPT (isr_undef, DABT)
void isr_dabt(void)
{

}

#pragma INTERRUPT (isr_undef, PABT)
void isr_pabt(void)
{

}
