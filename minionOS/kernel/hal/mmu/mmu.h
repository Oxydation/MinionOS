/*
 * mmu.h
 *
 *  Created on: 02.05.2017
 *      Author: sabrinarosmann
 */


#ifndef KERNEL_HAL_MMU_MMU_H_
#define KERNEL_HAL_MMU_MMU_H_

#include <inttypes.h>

typedef union {
    struct {
        uint8_t Type    : 2;
        uint32_t IGN    : 30;   // ignore
    } descriptor;
    uint32_t raw;
} FirstLevelFaultDescriptor_t;

typedef union {
    struct {
        uint8_t Type    : 2;
        uint8_t PXN     : 1;    // privileged execute-never bit
        uint8_t NS      : 1;    // non-secure bit
        uint8_t SBZ     : 1;    // should be zero
        uint8_t DOM     : 4;    // domain
        uint8_t IMP     : 1;    // implementation defined
        uint32_t PTBA   : 22;   // page table base address
    } descriptor;
    uint32_t raw;
} FirstLevelPagetableDescriptor_t;

typedef union {
    struct {
        uint8_t PXN     : 1;    // privileged execute-never bit
        uint8_t Type    : 1;
        uint8_t B       : 1;    // buffered bit
        uint8_t C       : 1;    // cached bit
        uint8_t XN      : 1;    // execute-never bit
        uint8_t DOM     : 4;    // domain
        uint8_t IMP     : 1;    // implementation defined
        uint8_t AP0_1   : 2;    // access permission bits [1:0]
        uint8_t TEX     : 3;    // memory region attribute bits
        uint8_t AP2     : 1;    // access permission bit [2]
        uint8_t S       : 1;    // shared bit
        uint8_t nG      : 1;    // not global bit (TLB)
        uint8_t SBZ     : 1;    // should be zero
        uint8_t NS      : 1;    // non-secure bit
        uint16_t SBA    : 12;   // section base address
    } descriptor;
    uint32_t raw;
} FirstLevelSectionDescriptor_t;

typedef union {
    struct {
        uint8_t Type    : 2;
        uint32_t IGN   : 30;   // ignore
    } descriptor;
    uint32_t raw;
} SecondLevelFaultDescriptor_t;

typedef union {
    struct {
        uint8_t Type    : 2;
        uint8_t B       : 1;    // buffered bit
        uint8_t C       : 1;    // cached bit
        uint8_t AP1_0   : 2;    // access permission bits [1:0]
        uint8_t SBZ     : 3;    // should be zero
        uint8_t AP2     : 1;    // access permission bit [2]
        uint8_t S       : 1;    // shared bit
        uint8_t nG      : 1;    // not global bit (TLB)
        uint8_t TEX     : 3;    // memory region attribute bits
        uint8_t XN      : 1;    // executed-never bit
        uint16_t LPBA   : 16;   // large page base address
    } descriptor;
    uint32_t raw;
} SecondLevelLargePageDescriptor_t;

typedef union {
    struct {
        uint8_t XN      : 1;    // execute-never bit
        uint8_t Type    : 1;
        uint8_t B       : 1;    // buffered bit
        uint8_t C       : 1;    // cached bit
        uint8_t AP1_0   : 2;    // access permission bits [1:0]
        uint8_t TEX     : 3;    // memory region attribute bits
        uint8_t AP2     : 1;    // access permission bit [2]
        uint8_t S       : 1;    // shared bit
        uint8_t nG      : 1;    // not global bit (TLB)
        uint32_t SPBA   : 20;   // small page base address
    } descriptor;
    uint32_t raw;
} SecondLevelSmallPageDescriptor_t;

/* functions for creating descriptors */
uint32_t mmu_createFirstLevelFaultDescriptor(void);
uint32_t mmu_createFirstLevelPageTableDescriptor(uint8_t domain);
uint32_t mmu_createFirstLevelSectionDescriptor(uint8_t domain, uint8_t buffered, uint8_t cached, uint8_t accessPermission);
uint32_t mmu_createSecondLevelFaultDescriptor(void);
uint32_t mmu_createSecondLevelLargePageDescriptor(uint8_t buffered, uint8_t cached, uint8_t accessPermission);
uint32_t mmu_createSecondLevelSmallPageDescriptor(uint8_t buffered, uint8_t cached, uint8_t accessPermission);

/* assembler functions */
void mmu_writeValueToPTE(uint32_t* PTEptr, uint32_t value, uint16_t nrOfEntries);
void mmu_setTTBR0(uint32_t ttb, uint32_t clearBitmask);
void mmu_setTTBR1(uint32_t ttb, uint32_t clearBitmask);
void mmu_setTTBCR(void);
void mmu_initCP15(uint32_t vectorTableAddress);
uint8_t mmu_getDataFaultStatus(void);
uint32_t mmu_getDataFaultAddress(void);
uint8_t mmu_getInstructionFaultStatus(void);
uint32_t mmu_getInstructionFaultAddress(void);
void mmu_setDomainAccess(uint32_t value, uint32_t mask);
void mmu_setMMUControl(uint32_t value, uint32_t mask);
void mmu_flushCache(void);
void mmu_flushTLB(void);

#endif /* KERNEL_HAL_MMU_MMU_H_ */
