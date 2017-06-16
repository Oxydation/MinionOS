/*
 * mmu.c
 *
 *  Created on: 02.05.2017
 *      Author: sabrinarosmann
 */

#include "kernel/hal/mmu/mmu.h"

uint32_t mmu_createFirstLevelFaultDescriptor(void) {
    FirstLevelFaultDescriptor_t faultDescriptor;
    faultDescriptor.descriptor.IGN = 0b0;
    faultDescriptor.descriptor.Type = 0b00;
    return faultDescriptor.raw;
}

uint32_t mmu_createFirstLevelPageTableDescriptor(uint8_t domain) {
    FirstLevelPagetableDescriptor_t ptDescriptor;
    ptDescriptor.descriptor.DOM = domain;
    ptDescriptor.descriptor.SBZ = 0;
    ptDescriptor.descriptor.Type = 0b01;
    ptDescriptor.descriptor.IMP = 0;
    ptDescriptor.descriptor.NS = 0;
    ptDescriptor.descriptor.PXN = 0;
    return ptDescriptor.raw;
}

uint32_t mmu_createFirstLevelSectionDescriptor(uint8_t domain, uint8_t buffered, uint8_t cached, uint8_t accessPermission) {
    FirstLevelSectionDescriptor_t sectionDescriptor;
    sectionDescriptor.descriptor.PXN = 0b0;
    sectionDescriptor.descriptor.Type = 0b1;
    sectionDescriptor.descriptor.B = buffered;
    sectionDescriptor.descriptor.C = cached;
    sectionDescriptor.descriptor.XN = 0b0;
    sectionDescriptor.descriptor.DOM = domain;
    sectionDescriptor.descriptor.IMP = 0b0;
    sectionDescriptor.descriptor.AP0_1 = accessPermission;
    sectionDescriptor.descriptor.TEX = 0;
    sectionDescriptor.descriptor.AP2 = 0;
    sectionDescriptor.descriptor.nG = 0;
    sectionDescriptor.descriptor.SBZ = 0;
    sectionDescriptor.descriptor.NS = 0;
    sectionDescriptor.descriptor.SBA = 0;
    return sectionDescriptor.raw;
}

uint32_t mmu_createSecondLevelFaultDescriptor(void) {
    SecondLevelFaultDescriptor_t faultDescriptor;
    faultDescriptor.descriptor.IGN = 0b0;
    faultDescriptor.descriptor.Type = 0b00;
    return faultDescriptor.raw;
}

uint32_t mmu_createSecondLevelLargePageDescriptor(uint8_t buffered, uint8_t cached, uint8_t accessPermission) {
    SecondLevelLargePageDescriptor_t lpDescriptor;
    lpDescriptor.descriptor.Type = 0b01;
    lpDescriptor.descriptor.B = buffered;
    lpDescriptor.descriptor.C = cached;
    lpDescriptor.descriptor.AP1_0 = accessPermission;
    lpDescriptor.descriptor.SBZ = 0b0;
    lpDescriptor.descriptor.AP2 = 0b0;
    lpDescriptor.descriptor.S = 0b0;
    lpDescriptor.descriptor.nG = 0b0;
    lpDescriptor.descriptor.TEX = 0b0;
    lpDescriptor.descriptor.XN = 0b0;
    return lpDescriptor.raw;
}

uint32_t mmu_createSecondLevelSmallPageDescriptor(uint8_t buffered, uint8_t cached, uint8_t accessPermission) {
    SecondLevelSmallPageDescriptor_t spDescriptor;
    spDescriptor.descriptor.XN = 0b0;
    spDescriptor.descriptor.Type = 0b1;
    spDescriptor.descriptor.B = buffered;
    spDescriptor.descriptor.C = cached;
    spDescriptor.descriptor.AP1_0 = accessPermission;
    spDescriptor.descriptor.TEX = 0b0;
    spDescriptor.descriptor.AP2 = 0b0;
    spDescriptor.descriptor.S = 0b0;
    spDescriptor.descriptor.nG = 0b0;
    return spDescriptor.raw;
}



