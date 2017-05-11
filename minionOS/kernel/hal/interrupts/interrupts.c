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
#include <stdio.h>
#include "global/types.h"
#include "kernel/systemModules/processManagement/contextSwitch.h"

// Keep book of all interrupt handlers
static InterruptHandler_t g_interruptHandlers[NROF_IR_VECTORS] = { 0 };
static PCB_t pcb;

/*
 * Registers a new interrupt handler at a given IRQ-Position.
 */
void interrupts_registerHandler(InterruptHandler_t handler, uint8_t irq_nr)
{
    interrupts_enableIrqSource(irq_nr);
    g_interruptHandlers[irq_nr] = handler; // set handler at given irq
}

void enable_interrupts()
{
    _enable_interrupts();
}

void disable_interrupts()
{
    _disable_interrupts();
}

#pragma SET_CODE_SECTION(".ISR")
#pragma INTERRUPT (isr_irq, IRQ)
void isr_irq(void)
{
    __asm(" ADD SP, SP, #8 ");

    // 1. Step: Remember LR (automatically done by compiler, including R0-R3 & R12)
    // 2. Step: Save previous context into PCB
    asm_saveContext(&pcb);

    // 3. Step: Handle Interrupts and reactivate
    uint32_t activeIrq = (*(Address_t) (INTCPS_SIR_IRQ)) & INTCPS_SIR_IRQ_MASK;
    InterruptHandler_t handler = g_interruptHandlers[activeIrq];
    if (handler != 0)
        handler(activeIrq, &pcb);

    // Clear IRQ interrupt output
    (*(Address_t) (INTCPS_CONTROL)) |= INTCPS_CONTROL_NEWIRQAGR;

    asm_dataSynchronizationBarrier();

    // 4. Step: Restore registers (R0-R4 & R1 are automatically restored by compiler)
    // 5. Step: Restore LR (automatically by compiler)
    //Set SP back 5 bytes to pop {R0-R4, R12, LR} from stack
    __asm(" SUB SP, SP, #28 ");

    // 6. Step Continuing with last user process (auto done)
    asm_loadContext(&pcb);
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
