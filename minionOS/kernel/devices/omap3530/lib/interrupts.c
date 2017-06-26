/*
 * Omap3530Interrupts.c
 *
 *  Created on: 18 Mar 2017
 *      Author: Mathias
 *  Description: Device dependent implementation of interrupt stuff.
 */
#include <kernel/common/mmio.h>
#include <kernel/devices/omap3530/includes/interrupts.h>
#include <kernel/devices/omap3530/includes/omap3530.h>
#include <kernel/hal/interrupts/interrupts.h>
#include <string.h>
#include "global/types.h"

void interrupts_initIrq()
{
    clear32(INTCPS_IDLE, 1);
    set32(INTCPS_IDLE, 0);
    interrupts_disableAllInterruptSources();
}

/*
 * Gets all IRQ channels and mark those which are pending.
 */
void interrupts_getPendingIrqs(uint8_t* pendingIrqs)
{
    memset(pendingIrqs, 0, NROF_IR_VECTORS * sizeof(uint8_t));

    int i = 0;

    // Gets the current IRQ status
    uint32_t status[3];
    status[0] = get32(INTCPS_PENDING_IRQ(0));
    status[1] = get32(INTCPS_PENDING_IRQ(1));
    status[2] = get32(INTCPS_PENDING_IRQ(2));

    for (i = 0; i < NROF_IR_VECTORS; i++)
    {
        if (status[i / REGISTER_SIZE] & (1u << i))
        {
            pendingIrqs[i] = TRUE;
        }
    }
}

/*
 * Enables the given IRQ channel.
 */
void interrupts_enableIrqSource(uint8_t irq_source)
{
    uint8_t bank = irq_source / REGISTER_SIZE;
    uint32_t bit = 1UL << (irq_source % REGISTER_SIZE);
    or32(INTCPS_MIR_CLEAR(bank), bit);
    clear32(INTCPS_ILR(bank), 0);
}

void interrupts_disableIrqSource(uint8_t irq_source)
{
    uint8_t bank = irq_source / REGISTER_SIZE;
    uint32_t bit = 1UL << (irq_source % REGISTER_SIZE);
    set32(INTCPS_MIR_SET(bank), bit); //todo: check if set is correct
}

/*
 * Checks if the IRQ channel is activated.
 */
uint8_t interrupts_getIrqSourceState(uint8_t irq_source)
{
    uint8_t bank = irq_source / REGISTER_SIZE;
    uint32_t bit = 1UL << (irq_source % REGISTER_SIZE);
    if ((get32(INTCPS_MIR(bank)) & bit) == bit)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

void interrupts_disableAllInterruptSources()
{
    // Disable all interrupts
    set32(INTCPS_MIR(0), 0xFFFFFFFF);
    set32(INTCPS_MIR(1), 0xFFFFFFFF);
    set32(INTCPS_MIR(2), 0xFFFFFFFF);
    //or_32(INTCPS_CONTROL, INTCPS_CONTROL_NEWIRQAGR);
}
