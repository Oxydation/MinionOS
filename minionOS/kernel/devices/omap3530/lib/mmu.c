/*
 * mmu.c
 *
 *  Created on: 09.05.2017
 *      Author: sabrinarosmann
 */

#include <kernel/devices/omap3530/includes/mmu.h>
#include <stdio.h>

/* Page tables */
/* VADDRESS, PTADDRESS, MasterPTADDRESS, PTTYPE, DOM */
PageTable_t masterPTOS = {0x80000000, 0x80500000, 0x80500000, MASTER, 0};
PageTable_t masterPTProcess = {0x00000000, 0x80504000, 0x80504000, MASTER, 0};
PageTable_t task1PT = {0x00000000, 0x80508000, 0x80504000, COARSE, 0};
PageTable_t task2PT = {0x00000000, 0x8050C000, 0x80504000, COARSE, 0};

/* Region tables */
/* VADDRESS, PAGESIZE, NUMPAGES, AP, CB, PADDRESS, &PT */
Region_t bootRegion = {0x40000000, SECTION, 1024, RWRW, WT, 0x40000000, &masterPTOS};
Region_t kernelRegion = {0x80000000, SECTION, 5, RWRW, WT, 0x80000000, &masterPTOS};
Region_t pageTableRegion = {0x80500000, SECTION, 1, RWRW, WT, 0x80500000, &masterPTOS};
//Region_t t1Region = {0x00000000, SMALL_PAGE, 256, RWRW, WT, 0x80600000, &task1PT};
//Region_t t2Region = {0x00000000, SMALL_PAGE, 256, RWRW, WT, 0x80700000, &task2PT};
Region_t t1Region = {0x80600000, SECTION, 1, RWRW, WT, 0x80600000, &masterPTOS};
Region_t t2Region = {0x80700000, SECTION, 1, RWRW, WT, 0x80700000, &masterPTOS};

void mmu_initTTB(void) {
    mmu_setTTBCR();
    mmu_setTTBR0(masterPTProcess.ptAddress, 0xFFF);     /* master PT for processes */
    mmu_setTTBR1(masterPTOS.ptAddress, 0x3FFF);          /* master PT for OS */
}

void mmu_initAllPT(void) {
    mmu_initPT(&masterPTOS);
    mmu_initPT(&masterPTProcess);
    //mmu_initPT(&task1PT);
    //mmu_initPT(&task2PT);
    //mmu_initPT(&task3PT);
}

/**
 * initializes a page table by taking the memory area allocated
 * for a page table and setting it with FAULT values
 */
int8_t mmu_initPT(PageTable_t* pt) {

    uint16_t nrOfEntries;                            /* number of entries in the page table */
    uint32_t value;                                 /* page table entry */
    uint32_t* PTEptr = (uint32_t*)pt->ptAddress;    /* points to page table entry in PT */

    switch (pt->type) {
        case COARSE: {
            nrOfEntries = 256;     /* a COARSE PT has 256 PT entries */
            value = mmu_createSecondLevelFaultDescriptor();
            break;
        }
        case MASTER: {
            nrOfEntries = 4096;    /* a MASTER PT has 4096 PT entries */
            value = mmu_createFirstLevelFaultDescriptor();
            break;
        }
        default: {
            printf("mmuInitPT: UNKNOWN pagetable type\n");
            return -1;
        }
    }

    mmu_writeValueToPTE(PTEptr, value, nrOfEntries);

    return 0;
}

void mmu_mapAllRegions(void) {
    mmu_mapRegion(&bootRegion);
    mmu_mapRegion(&kernelRegion);
    mmu_mapRegion(&pageTableRegion);
    mmu_mapRegion(&t1Region);
    mmu_mapRegion(&t2Region);
    //mmu_mapRegion(&t1Region);
    //mmu_mapRegion(&t2Region);
    //mmu_mapRegion(&t3Region);
}

void mmu_mapRegion(Region_t* region) {

    switch(region->PT->type) {
        case MASTER: {
            mmu_mapSectionTableRegion(region);
            break;
        }
        case COARSE: {
            mmu_mapCoarseTableRegion(region);
            break;
        }
        default: {
            printf("UNKNOWN page table type\n");
        }
    }
}

void mmu_mapSectionTableRegion(Region_t* region) {

    int16_t i;
    uint32_t* PTEptr;                               /* pointer to page table entry */
    uint32_t pAddress;                              /* physical address */
    uint32_t tableIndex;
    uint8_t domain = region->PT->dom;
    uint8_t buffered = region->CB & 0x1;
    uint8_t cached = region->CB >> 1;
    uint8_t AP = region->AP;

    PTEptr = (uint32_t*)region->PT->ptAddress;
    //tableIndex = region->vAddress &= ~0xC0000000;   /* clear bits [31:30] - see "Cortex A8 - TRM", Figure B3-9 */
    tableIndex = region->vAddress;
    tableIndex = tableIndex >> 20;                  /* take bits [31:20] as table index */
    tableIndex = tableIndex << 2;
    PTEptr = (uint32_t*)((uint32_t)PTEptr + tableIndex);         /* set to first PTE in region */

    pAddress = region->pAddress & 0xfff00000;       /* take only bits [31:20] */
    for (i = 0; i < region->numPages; i++) {
        uint32_t descriptor = mmu_createFirstLevelSectionDescriptor(domain, buffered, cached, AP);
        descriptor &= ~0xFFF00000;
        descriptor |= pAddress;
        *PTEptr++ = descriptor;
        pAddress += (1 << 20);                      /* jump to start of next 1 MB section */
    }
}

int8_t mmu_mapCoarseTableRegion(Region_t* region) {

    int16_t i;
    uint32_t* PTEptr;
    uint32_t PTE;
    uint8_t AP = region->AP;
    uint8_t C = region->CB & 0x10;
    uint8_t B = region->CB & 0x01;
    uint32_t pAddress;

    PTEptr = (uint32_t*)region->PT->ptAddress;      /* base address of the page table */

    switch (region->pageSize) {
        case LARGE_PAGE: {
            PTEptr += (region->vAddress & 0x000ff000) >> 12;    /* set to first PTE in region */
            while (*PTEptr != 0) {                              /* jumps over existing PTEs if page table maps more than one region */
                PTEptr++;
            }
            PTEptr += (region->numPages - 1);                   /* set to last PTE in region */

            PTE = region->pAddress & 0xffff0000;                /* set physical address - in large page 16 bits are for base physical address */
            PTE |= AP << 10;                                /* set access permissions subpage 3 */
            PTE |= AP << 8;                                 /* set access permissions subpage 2 */
            PTE |= AP << 6;                                 /* set access permissions subpage 1 */
            PTE |= AP << 4;                                 /* set access permissions subpage 0 */
            PTE |= (region->CB & 0x3) << 2;                     /* set cache & WB attribute */
            PTE |= 0x1;                                         /* set as large page */

            for (i = region->numPages -1; i >= 0; i--) {
                *PTEptr-- = PTE + (i << 16);                    /* i = 64 KB large page */
            }
            break;
        }
        case SMALL_PAGE: {
//            PTEptr += (region->vAddress & 0x000ff000) >> 12;    /* set to first PTE in region */
//            while (*PTEptr != 0) {                              /* jumps over existing PTEs if page table maps more than one region */
//                PTEptr++;
//            }
//            PTEptr += (region->numPages - 1);                   /* set to last PTE in region */

//            PTE = region->pAddress & 0xfffff000;                /* set physical address - in small page 20 bits are for base physical address */
//            PTE |= tempAP << 10;                                /* set access permissions subpage 3 */
//            PTE |= tempAP << 8;                                 /* set access permissions subpage 2 */
//            PTE |= tempAP << 6;                                 /* set access permissions subpage 1 */
//            PTE |= tempAP << 4;                                 /* set access permissions subpage 0 */
//            PTE |= (region->CB & 0x3) << 2;                     /* set cache & WB attribute */
//            PTE |= 0x2;                                         /* set as small page */

//            for (i = region->numPages -1; i >= 0; i--) {        /* fill in table entries for region */
//                *PTEptr-- = PTE + (i << 12);                    /* i = 4 KB small page */
//            }
//            break;

            pAddress = region->pAddress & 0xfffff000;
            for (i = 0; i <= region->numPages -1; i++) {
                // TODO!
                *PTEptr++ = mmu_createSecondLevelSmallPageDescriptor(B, C, AP);
                pAddress += (i << 12);                          /* 4 KB small page */
            }
            break;
        }
        default: {
            printf("mmu_mapCoarseTableRegion: Incorrect page size\n");
            return -1;
        }
    }
    return 0;
}

void mmu_attachAllPT(void) {
    //mmu_attachPT(&masterPT);
    //mmu_attachPT(&systemPT);
    //mmu_attachPT(&task1PT);
}

int8_t mmu_attachPT(PageTable_t* pt) {

    uint32_t* ttb;                          /* translation table base address */
    uint32_t PTE;
    uint32_t offset;
    uint32_t * pEntry;

    ttb = (uint32_t*)pt->masterPtAddress;   /* read translation table base address from PT */
    offset = (pt->vAddress)>>20;            /* determine PTE from vAddress -> virtual address is divided by 1 MB
                                               by shifting the virtual address right by 20 bits. Adding this offset
                                               to the master L1 base address generates a pointer to the address
                                               within the L1 master table that represents the translation for the
                                               1 MB section */

    switch (pt->type) {
        case MASTER: {
            //mmu_setTTBR0(ttb);
            break;
        }
        case COARSE: {
            /* PTE = addr L2 PT | domain | COARSE PT type */
            PTE = (pt->ptAddress & 0xfffffc00);
            PTE |= pt->dom<<5;
            PTE |= 0x11;
            pEntry = ttb + offset;      /* offset acts as an index */
            *pEntry = PTE;
            break;
        }
        default: {
            printf("UNKNOWN page table type\n");
            return -1;
        }
    }
    return 0;
}

void mmu_setAllDomainAccesses(void) {
    uint32_t right = MASK_ALL_DOM;
    mmu_setDomainAccess(right, MASK_ALL_DOM);
}

void mmu_initMMU(void) {

    /* initialize coprocessor 15 by telling it the address of the vector table */
    mmu_initCP15(0x4020FFC0);

    /* disable MMU */
    uint32_t enable, changeMask;
    enable = 0x0;
    changeMask = CHANGE_MMU | CHANGE_ALIGNMENT | CHANGE_D_CACHE | CHANGE_I_CACHE | CHANGE_AFE | CHANGE_TRE;
    mmu_setMMUControl(enable, changeMask);

    /* flush cache */
    mmu_flushCache();

    /* set translation table base addresses */
    mmu_initTTB();

    /* set domain access */
    mmu_setAllDomainAccesses();

    /* initialize system page tables */
    mmu_initAllPT();

    /* fill page tables with translation & attribute data */
    mmu_mapAllRegions();

    /* activate page tables */
   // mmu_attachAllPT();

    /* flush TLB */
    mmu_flushTLB();

    /* enable MMU, caches and write buffer */
    //enable = ENABLE_MMU | ENABLE_D_CACHE | ENABLE_I_CACHE;
    enable = ENABLE_MMU | ENABLE_ALIGNMENT | ENABLE_I_CACHE; // | ENABLE_D_CACHE;
    mmu_setMMUControl(enable, changeMask);
}

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

