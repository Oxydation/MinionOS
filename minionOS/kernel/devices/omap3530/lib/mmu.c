/*
 * mmu.c
 *
 *  Created on: 09.05.2017
 *      Author: sabrinarosmann
 */

#include <kernel/devices/omap3530/includes/mmu.h>
#include <stdio.h>

void mmu_copyPTEValue(uint32_t PTE);
void mmu_writeValueToPTE(uint32_t* PTEptr, uint32_t value, uint16_t nrOfEntries);

/* Page tables */
/* VADDRESS, PTADDRESS, MasterPTADDRESS, PTTYPE, DOM */
PageTable_t masterPT = {0x80000000, 0x80500000, 0x80500000, MASTER, 3};
PageTable_t systemPT = {0x80000000, 0x80504000, 0x80500000, COARSE, 3};
PageTable_t task1PT =  {0x84700000, 0x80504400, 0x80500000, COARSE, 3};
PageTable_t task2PT =  {0x84700000, 0x80504800, 0x80500000, COARSE, 3};
PageTable_t task3PT =  {0x84700000, 0x80504c00, 0x80500000, COARSE, 3};

/* Region tables */
/* VADDRESS, PAGESIZE, NUMPAGES, AP, CB, PADDRESS, &PT */
Region_t kernelRegion = {0x80000000, 4, 1280, RWNA, WT, 0x80000000, &systemPT};
Region_t pageTableRegion = {0x80500000, 4, 512, RWRW, WT, 0x80500000, &systemPT};
Region_t t1Region = {0x84700000, 4, 256, RWRW, WT, 0x80700000, &task1PT};
Region_t t2Region = {0x84700000, 4, 256, RWRW, WT, 0x80800000, &task2PT};
Region_t t3Region = {0x84700000, 4, 256, RWRW, WT, 0x80900000, &task3PT};

void mmu_initAllPT(void) {
    mmu_initPT(&masterPT);
    mmu_initPT(&systemPT);
    mmu_initPT(&task1PT);
    mmu_initPT(&task2PT);
    mmu_initPT(&task3PT);
}

/**
 * initializes a page table by taking the memory area allocated
 * for a page table and setting it with FAULT values
 */
int8_t mmu_initPT(PageTable_t* pt) {

    uint16_t nrOfEntries;                            /* number of entries in the page table */
    uint32_t value = FAULT;                         /* page table entry */
    uint32_t* PTEptr = (uint32_t*)pt->ptAddress;    /* points to page table entry in PT */

    switch (pt->type) {
        case COARSE: {
            nrOfEntries = 256;     /* a COARSE PT has 256 PT entries */
            break;
        }
        case MASTER: {
            nrOfEntries = 4096;    /* a MASTER PT has 4096 PT entries */
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

uint8_t mmu_mapRegion(Region_t* region) {

}

void mmu_mapSectionTableRegion(Region_t* region) {

}

void mmu_mapCoarseTableRegion(Region_t* region) {

}

uint8_t mmu_attachPT(PageTable_t* pt) {

}

void mmu_domainAccessSet(uint8_t value, uint16_t mask) {

}

void controlSet(uint8_t value, uint16_t mask) {

}

void mmu_initMMU(void) {

}

void ttbSet(uint8_t ttb) {

}

void flushCache(void) {

}

void flushTLB(void) {

}

void processIDSet(uint8_t value) {

}


