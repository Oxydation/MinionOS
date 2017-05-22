/*
 * mmu.c
 *
 *  Created on: 09.05.2017
 *      Author: sabrinarosmann
 */

#include <kernel/devices/omap3530/includes/mmu.h>
#include <stdio.h>

void mmu_writeValueToPTE(uint32_t* PTEptr, uint32_t value, uint16_t nrOfEntries);
void mmu_setTTB(uint32_t* ttb);

/* Page tables */
/* VADDRESS, PTADDRESS, MasterPTADDRESS, PTTYPE, DOM */
PageTable_t masterPT = {0x80000000, 0x80500000, 0x80500000, MASTER, 3};
PageTable_t systemPT = {0x80000000, 0x80504000, 0x80500000, COARSE, 3};
PageTable_t task1PT =  {0x84700000, 0x80504400, 0x80500000, COARSE, 3};
PageTable_t task2PT =  {0x84700000, 0x80504800, 0x80500000, COARSE, 3};
PageTable_t task3PT =  {0x84700000, 0x80504c00, 0x80500000, COARSE, 3};

/* Region tables */
/* VADDRESS, PAGESIZE, NUMPAGES, AP, CB, PADDRESS, &PT */
Region_t kernelRegion = {0x80000000, LARGE_PAGE, 80, RWNA, WT, 0x80000000, &systemPT};
Region_t pageTableRegion = {0x80500000, LARGE_PAGE, 1, RWRW, WT, 0x80500000, &systemPT};
Region_t t1Region = {0x84520000, SMALL_PAGE, 256, RWRW, WT, 0x80520000, &task1PT};
Region_t t2Region = {0x84520000, SMALL_PAGE, 256, RWRW, WT, 0x80620000, &task2PT};
Region_t t3Region = {0x84520000, SMALL_PAGE, 256, RWRW, WT, 0x80720000, &task3PT};

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

void mmu_mapAllRegions(void) {
    mmu_mapRegion(&kernelRegion);
    mmu_mapRegion(&pageTableRegion);
    mmu_mapRegion(&t1Region);
    mmu_mapRegion(&t2Region);
    mmu_mapRegion(&t3Region);
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
            //return -1;
        }
    }
}

void mmu_mapSectionTableRegion(Region_t* region) {

    int16_t i;
    uint32_t * PTEptr;
    uint32_t PTE;

    PTEptr = (uint32_t*)region->PT->ptAddress;      /* base address page table */
    PTEptr += region->vAddress>>20;                 /* set to first PTE in region */
    PTEptr += (region->numPages - 1);               /* set to last PTE in region */

    PTE = region->pAddress & 0xfff00000;            /* set physical address */
    PTE |= (region->AP & 0x3) << 10;                /* set access permissions */
    PTE |= region->PT->dom << 5;                    /* set domain for section */
    PTE |= (region->CB & 0x3) << 2;                 /* set cache & wb attributes */
    PTE |= 0x12;                                    /* set as section entry */

    for (i = region->numPages -1; i >= 0; i--) {    /* fill PTE in region */
        *PTEptr-- = PTE + (i << 20);                /* i = 1 MB section */
    }
}

int8_t mmu_mapCoarseTableRegion(Region_t* region) {

    int16_t i;
    uint32_t* PTEptr;
    uint32_t PTE;
    uint8_t tempAP = region->AP & 0x3;              /* AND with 0x3 so that only the last two bits are taken */

    PTEptr = (uint32_t*)region->PT->ptAddress;      /* base address of the page table */

    switch (region->pageSize) {
        case LARGE_PAGE: {
            PTEptr += (region->vAddress & 0x000ff000) >> 12;    /* set to first PTE in region */
            while (*PTEptr != 0) {                              /* jumps over existing PTEs if page table maps more than one region */
                PTEptr++;
            }
            PTEptr += (region->numPages - 1);                   /* set to last PTE in region */

            PTE = region->pAddress & 0xffff0000;                /* set physical address - in large page 16 bits are for base physical address */
            PTE |= tempAP << 10;                                /* set access permissions subpage 3 */
            PTE |= tempAP << 8;                                 /* set access permissions subpage 2 */
            PTE |= tempAP << 6;                                 /* set access permissions subpage 1 */
            PTE |= tempAP << 4;                                 /* set access permissions subpage 0 */
            PTE |= (region->CB & 0x3) << 2;                     /* set cache & WB attribute */
            PTE |= 0x1;                                         /* set as large page */

            for (i = region->numPages -1; i >= 0; i--) {
                *PTEptr-- = PTE + (i << 16);                    /* i = 64 KB large page */
            }
            break;
        }
        case SMALL_PAGE: {
            PTEptr += (region->vAddress & 0x000ff000) >> 12;    /* set to first PTE in region */
            while (*PTEptr != 0) {                              /* jumps over existing PTEs if page table maps more than one region */
                PTEptr++;
            }
            PTEptr += (region->numPages - 1);                   /* set to last PTE in region */

            PTE = region->pAddress & 0xfffff000;                /* set physical address - in small page 20 bits are for base physical address */
            PTE |= tempAP << 10;                                /* set access permissions subpage 3 */
            PTE |= tempAP << 8;                                 /* set access permissions subpage 2 */
            PTE |= tempAP << 6;                                 /* set access permissions subpage 1 */
            PTE |= tempAP << 4;                                 /* set access permissions subpage 0 */
            PTE |= (region->CB & 0x3) << 2;                     /* set cache & WB attribute */
            PTE |= 0x2;                                         /* set as small page */

            for (i = region->numPages -1; i >= 0; i--) {        /* fill in table entries for region */
                *PTEptr-- = PTE + (i << 12);                    /* i = 4 KB small page */
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
    mmu_attachPT(&masterPT);
    mmu_attachPT(&systemPT);
    mmu_attachPT(&task1PT);
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
            mmu_setTTB(ttb);
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
    uint8_t right = RWNA << D3;
    mmu_setDomainAccess(right, CHANGE_ALL_DOM);
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


