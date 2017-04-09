/*
 * Common.c
 *
 *  Created on: 15 Mar 2017
 *      Author: Mathias
 */
#include <kernel/common/Common.h>

/**
 * Sets the value to a given address.
 */
void set32(uint32_t address, uint32_t value)
{
    uint32_t * mem = (uint32_t *) address;
    *mem = value;
}

/**
 * Gets the value at a given address.
 */
uint32_t get32(uint32_t address)
{
    return *((uint32_t *) address);
}

void or32(uint32_t address, uint32_t value)
{
    uint32_t * mem = (uint32_t *) address;
    *mem |= value;
}

void and32(uint32_t address, uint32_t value)
{
    uint32_t * mem = (uint32_t *) address;
    *mem &= value;
}

void clear32(uint32_t address, uint32_t value)
{
    uint32_t * mem = (uint32_t *) address;
    *mem &= ~value;
}
