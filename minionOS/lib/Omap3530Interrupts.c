/*
 * Omap3530Interrupts.c
 *
 *  Created on: 18 Mar 2017
 *      Author: Mathias
 *  Description: Device dependent implementation of interrupt stuff.
 */
#include "includes/Interrupts.h"
#include <string.h>
#include "includes/Common.h"
#include "includes/Omap3530.h"

void init_irq()
{
    clear_32(INTCPS_IDLE, 1);
    set_32(INTCPS_IDLE, 0);
    disable_all_interrupt_sources();
}

/*
 * Gets all IRQ channels and mark those which are pending.
 */
void get_pending_irqs(uint8_t* pendingIrqs)
{
    memset(pendingIrqs, 0, NROF_IR_VECTORS * sizeof(uint8_t));

    int i = 0;

    // Gets the current IRQ status
    uint32_t status[3];
    status[0] = get_32(INTCPS_PENDING_IRQ(0));
    status[1] = get_32(INTCPS_PENDING_IRQ(1));
    status[2] = get_32(INTCPS_PENDING_IRQ(2));

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
void enable_irq_source(uint8_t irq_source)
{
    uint8_t bank = irq_source / REGISTER_SIZE;
    uint8_t bit = 1UL << (irq_source % REGISTER_SIZE);
    set_32(INTCPS_MIR_CLEAR(bank), bit);
    clear_32(INTCPS_ILR(bank), 0);
}

void disable_irq_source(uint8_t irq_source)
{
    uint8_t bank = irq_source / REGISTER_SIZE;
    uint8_t bit = 1UL << (irq_source % REGISTER_SIZE);
    set_32(INTCPS_MIR_SET(bank), bit);
}

/*
 * Checks if the IRQ channel is activated.
 */
uint8_t get_irq_source_state(uint8_t irq_source)
{
    uint8_t bank = irq_source / REGISTER_SIZE;
    uint8_t bit = 1UL << (irq_source % REGISTER_SIZE);
    if ((get_32(INTCPS_MIR(bank)) & bit) == bit)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

void disable_all_interrupt_sources()
{
    // Disable all interrupts
    set_32(INTCPS_MIR(0), 0xFFFFFFFF);
    set_32(INTCPS_MIR(1), 0xFFFFFFFF);
    set_32(INTCPS_MIR(2), 0xFFFFFFFF);
    //or_32(INTCPS_CONTROL, INTCPS_CONTROL_NEWIRQAGR);
}
