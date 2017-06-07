/*
 * sdCardStorage.c
 *
 *  Created on: 20.05.2017
 *      Author: benimurza
 */

#include "abstractStorage.h"
#include "../../hal/mmc_sd/sdCard.h"

uint32_t readSector(uint8_t * buf, uint32_t address){
    return sdCard_read512ByteBlock(buf, address);
}
