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

/*
 * Gets all IRQ channels and mark those which are pending.
 */
uint8_t * get_pending_irqs(void)
{
    uint8_t * pendingIrqs;
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

    return pendingIrqs;
}

/*
 * Enables the given IRQ channel.
 */
void enable_irq(uint8_t irq_nr)
{
    set_32(INTCPS_MIR_SET(irq_nr/REGISTER_SIZE), irq_nr % REGISTER_SIZE);
}
