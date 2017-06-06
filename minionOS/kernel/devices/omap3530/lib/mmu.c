/*
 * mmu.c
 *
 *  Created on: 09.05.2017
 *      Author: sabrinarosmann
 */

#include <kernel/devices/omap3530/includes/mmu.h>

Process_t processes[MAX_ALLOWED_PROCESSES];

/* Page tables */
/* VADDRESS, PTADDRESS, MasterPTADDRESS, PTTYPE, DOM */
PageTable_t masterPTOS = {0x80000000, 0x80500000, 0x80500000, MASTER, 0};
PageTable_t masterPTProcess = {0x00000000, 0x80508000, 0x80508000, MASTER, 0};
PageTable_t pageTablePT = {0x80500000, 0x80504000, 0x80500000, COARSE, 0};

/* Page status arrays */
PageStatus_t bootRegionStatus[1024];        // 1024 pages with 1 MB
PageStatus_t kernelRegionStatus[5];         // 5 pages with 1 MB
PageStatus_t pageTableRegionStatus[256];    // 256 pages with 4 KB

/* Region tables */
/* VADDRESS, PAGESIZE, NUMPAGES, AP, CB, nrOfReservedPages, PADDRESS, &PT, page status */
Region_t bootRegion = {0x40000000, SECTION, 1024, RWRW, WT, 0, 0x40000000, &masterPTOS, bootRegionStatus};
Region_t kernelRegion = {0x80000000, SECTION, 5, RWRW, WT, 0, 0x80000000, &masterPTOS, kernelRegionStatus};
Region_t pageTableRegion = {0x80500000, SMALL_PAGE, 256, RWRW, WT, 0, 0x80500000, &pageTablePT, pageTableRegionStatus};

void mmu_initTTB(void) {
    mmu_setTTBCR();
    mmu_setTTBR0(masterPTProcess.ptAddress, 0xFFF);     /* master PT for processes */
    mmu_setTTBR1(masterPTOS.ptAddress, 0x3FFF);          /* master PT for OS */
}

void mmu_setProcessPT(PageTable_t* pt) {
    mmu_setTTBR0(pt->ptAddress, 0xFFF);
}

void mmu_initAllPT(void) {
    mmu_initPT(&masterPTOS);
    mmu_initPT(&masterPTProcess);
    mmu_initPT(&pageTablePT);
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
    mmu_mapRegion(&bootRegion, bootRegion.numPages, 0);
    mmu_mapRegion(&kernelRegion, kernelRegion.numPages, 0);
    mmu_attachPT(&pageTablePT, &masterPTOS);
    //mmu_mapRegion(&pageTableRegion, pageTableRegion.numPages);
    mmu_mapRegion(&pageTableRegion, 12, 0);
}

void mmu_mapRegion(Region_t* region, uint16_t nrOfPages, uint8_t processId) {

    switch(region->PT->type) {
        case MASTER: {
            mmu_mapSectionTableRegion(region, nrOfPages, processId);
            break;
        }
        case COARSE: {
            mmu_mapCoarseTableRegion(region, nrOfPages, processId);
            break;
        }
        default: {
            printf("UNKNOWN page table type\n");
        }
    }
}

void mmu_mapSectionTableRegion(Region_t* region, uint16_t nrOfPages, uint8_t processId) {

    int16_t i;
    uint32_t* PTEptr;                                       /* pointer to page table entry */
    uint32_t pAddress;                                      /* physical address */
    uint32_t tableIndex;
    uint8_t domain = region->PT->dom;
    uint8_t buffered = region->CB & 0x1;
    uint8_t cached = region->CB >> 1;
    uint8_t AP = region->AP;

    PTEptr = (uint32_t*)region->PT->ptAddress;
    tableIndex = region->vAddress;
    tableIndex = tableIndex >> 20;                          /* take bits [31:20] as table index */
    tableIndex = tableIndex << 2;
    PTEptr = (uint32_t*)((uint32_t)PTEptr + tableIndex);    /* set to first PTE in region */

    pAddress = region->pAddress & 0xfff00000;               /* take only bits [31:20] */
    PageStatus_t* status = region->pageStatus;
    for (i = 0; i < nrOfPages; i++) {
        uint32_t descriptor = mmu_createFirstLevelSectionDescriptor(domain, buffered, cached, AP);
        descriptor &= ~0xFFF00000;
        descriptor |= pAddress;
        *PTEptr++ = descriptor;
        status->reserved = 1;
        status->processId = processId;
        status++;
        pAddress += (1 << 20);                              /* jump to start of next 1 MB section */
    }
    region->reservedPages = region->reservedPages + nrOfPages;
}

int8_t mmu_mapCoarseTableRegion(Region_t* region, uint16_t nrOfPages, uint8_t processId) {

    int16_t i;
    uint32_t* PTEptr;                                       /* pointer to page table entry */
    uint32_t pAddress;                                      /* physical address */
    uint32_t tableIndex;
    uint8_t buffered = region->CB & 0x1;
    uint8_t cached = region->CB >> 1;
    uint8_t AP = region->AP;

    PTEptr = (uint32_t*)region->PT->ptAddress;                  /* base address of the page table */

    switch (region->pageSize) {
        case LARGE_PAGE: {
            // TODO
            break;
        }
        case SMALL_PAGE: {

            tableIndex = region->vAddress;
            tableIndex &= ~0xFFF00000;                              /* clear bits [31:20] */
            tableIndex = tableIndex >> 12;                          /* take bits [19:12] as table index */
            tableIndex = tableIndex << 2;                           /* add two 0-bits at the end */
            PTEptr = (uint32_t*)((uint32_t)PTEptr + tableIndex);    /* set to first PTE in region */

            pAddress = region->pAddress & 0xFFFFF000;               /* take only bits [31:12] */
            PageStatus_t* status = region->pageStatus;
            for (i = 0; i < nrOfPages; i++) {
                uint32_t descriptor = mmu_createSecondLevelSmallPageDescriptor(buffered, cached, AP);
                descriptor &= ~0xFFFFF000;
                descriptor |= pAddress;
                *PTEptr++ = descriptor;
                status->reserved = 1;
                status->processId = processId;
                status++;
                pAddress += (1 << 12);                              /* jump to start of next 4 KB page */
            }
            region->reservedPages = region->reservedPages + nrOfPages;
            break;
        }
        default: {
            printf("mmu_mapCoarseTableRegion: Incorrect page size\n");
            return -1;
        }
    }
    return 0;
}

void mmu_attachPT(PageTable_t* pt, PageTable_t* masterPT) {

    uint32_t* masterPTEptr;
    uint32_t tableIndex;
    uint32_t pteEntry;
    uint32_t pAddress;

    masterPTEptr = (uint32_t*)masterPT->ptAddress;

    tableIndex = pt->vAddress;
    tableIndex = tableIndex >> 20;      /* take bits [31:20] as table index */
    tableIndex = tableIndex << 2;       /* add to 0-bits at the end */
    pAddress = pt->ptAddress & 0xFFFFF000;

    masterPTEptr = (uint32_t*)((uint32_t)masterPTEptr + tableIndex);
    pteEntry = mmu_createFirstLevelPageTableDescriptor(pt->dom);
    pteEntry &= ~0xFFFFF000;
    pteEntry |= pAddress;
    *masterPTEptr = pteEntry;
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

void mmu_initProcess(uint32_t vAddress, uint32_t pAddress) {

    int16_t freePageIndexForPT = mmu_findFreePageInRegion(&pageTableRegion);
    uint32_t* ptPtr;

    if (freePageIndexForPT != -1) {

        ptPtr = (uint32_t*)((uint32_t)pageTableRegion.pAddress + freePageIndexForPT * 0x1000);

        /* VADDRESS, PTADDRESS, MasterPTADDRESS, PTTYPE, DOM */
        PageTable_t taskPT = {vAddress, (uint32_t)ptPtr, masterPTProcess.ptAddress, COARSE, 0};

        /* VADDRESS, PAGESIZE, NUMPAGES, AP, CB, PADDRESS, &PT */
        PageStatus_t* status = (PageStatus_t*)(pageTableRegionStatus + freePageIndexForPT);
        Region_t taskPTRegion = {(uint32_t)ptPtr, SMALL_PAGE, 4, RWRW, WT, 0, (uint32_t)ptPtr, &pageTablePT, status};

        uint8_t processId = processManager_getNextProcessId();
        PageStatus_t taskRegionStatus[256] = {{0,0}};
        Region_t taskRegion = {vAddress, SMALL_PAGE, 256, RWRW, WT, 0, pAddress, &taskPT, taskRegionStatus};

        mmu_mapRegion(&taskPTRegion, taskPTRegion.numPages, processId);
        mmu_attachPT(&taskPT, &masterPTProcess);
        mmu_initPT(&taskPT);
        mmu_mapRegion(&taskRegion, taskRegion.numPages, processId);

        PCB_t pcb = processManager_loadProcess(taskRegion.vAddress, (uint32_t)taskRegion.vAddress + 0x10000);
        Process_t process = {.pcb = &pcb, .pageTable = &taskPT};
        processes[pcb.processId] = process;
    }
}

void mmu_switchProcess(PCB_t pcb) {
    Process_t* process = &processes[pcb.processId];
    mmu_setProcessPT(process->pageTable);

    mmu_flushTLB();
    mmu_flushCache();
}

void mmu_killProcess(void) {

}

int16_t mmu_findFreePageInRegion(Region_t* region) {

    if (region->numPages == region->reservedPages) {
        return -1;
    }

    int i;
    for (i = 0; i < region->numPages; i++) {
        if (region->pageStatus[i].reserved == 0) {
            return i;
        }
    }
    return -1;
}
