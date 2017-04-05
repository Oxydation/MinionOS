/*
 * Common.c
 *
 *  Created on: 15 Mar 2017
 *      Author: Mathias
 */
#include <includes/Common.h>

/**
 * Sets the value to a given address.
 */
void set_32(uint32_t address, uint32_t value)
{
    uint32_t * mem = (uint32_t *) address;
    *mem = value;
}

/**
 * Gets the value at a given address.
 */
uint32_t get_32(uint32_t address)
{
    return *((uint32_t *) address);
}

void or_32(uint32_t address, uint32_t value)
{
    uint32_t * mem = (uint32_t *) address;
    *mem |= value;
}

void and_32(uint32_t address, uint32_t value)
{
    uint32_t * mem = (uint32_t *) address;
    *mem &= value;
}

void clear_32(uint32_t address, uint32_t value)
{
    uint32_t * mem = (uint32_t *) address;
    *mem &= ~value;
}
