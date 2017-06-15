/*
 * mmu.c
 *
 *  Created on: 14.06.2017
 *      Author: sabrinarosmann
 */

#include <kernel/systemModules/mmu/mmu.h>

static Process_t processes[MAX_ALLOWED_PROCESSES];
static Process_t* g_currentMMUProcess;

/* Page tables */
/* VADDRESS, PTADDRESS, MasterPTADDRESS, PTTYPE, DOM */
static PageTable_t masterPTOS = { .vAddress = KERNEL_REGION_START_ADDRESS,
                                  .ptAddress = MASTER_PT_OS_START_ADDRESS,
                                  .masterPtAddress = MASTER_PT_OS_START_ADDRESS,
                                  .type = MASTER,
                                  .dom = 0};

static PageTable_t pageTablePT = { .vAddress = PAGE_TABLE_REGION_START_ADDRESS,
                                   .ptAddress = PAGETABLE_PT_START_ADDRESS,
                                   .masterPtAddress = MASTER_PT_OS_START_ADDRESS,
                                   .type = COARSE,
                                   .dom = 0};

/* Page status arrays */
static PageStatus_t bootRegionStatus[NR_OF_PAGES_IN_BOOT_REGION];                   // 1024 pages with 1 MB
static PageStatus_t kernelRegionStatus[NR_OF_PAGES_IN_KERNEL_REGION];               // 5 pages with 1 MB
static PageStatus_t pageTableRegionStatus[NR_OF_PAGES_IN_PAGE_TABLE_REGION];        // 256 pages with 4 KB
static PageStatus_t processMemoryRegionStatus[NR_OF_PAGES_IN_PROCESSMEMORY_REGION]; // 1018 pages with 1 MB

/* Region tables */
/* VADDRESS, PAGESIZE, NUMPAGES, AP, CB, nrOfReservedPages, PADDRESS, &PT, page status */
static Region_t bootRegion = { .vAddress = BOOT_REGION_START_ADDRESS, .pageSize = SECTION, .numPages = NR_OF_PAGES_IN_BOOT_REGION,
                               .AP = RWRW, .CB = WT, .reservedPages = 0, .pAddress = BOOT_REGION_START_ADDRESS,
                               .PT = &masterPTOS, .pageStatus = bootRegionStatus};

static Region_t kernelRegion = { .vAddress = KERNEL_REGION_START_ADDRESS, .pageSize = SECTION, .numPages = NR_OF_PAGES_IN_KERNEL_REGION,
                                 .AP = RWRW, .CB = WT, .reservedPages = 0, .pAddress = KERNEL_REGION_START_ADDRESS,
                                 .PT = &masterPTOS, .pageStatus = kernelRegionStatus};

static Region_t pageTableRegion = { .vAddress = PAGE_TABLE_REGION_START_ADDRESS, .pageSize = SMALL_PAGE,
                                    .numPages = NR_OF_PAGES_IN_PAGE_TABLE_REGION, .AP = RWRW, .CB = WT, .reservedPages = 0,
                                    .pAddress = PAGE_TABLE_REGION_START_ADDRESS, .PT = &pageTablePT,
                                    .pageStatus = pageTableRegionStatus};

static Region_t processMemoryRegion = { .vAddress = PROCESSMEMORY_REGION_START_ADDRESS, .pageSize = SECTION,
                                        .numPages = NR_OF_PAGES_IN_PROCESSMEMORY_REGION, .AP = RWRW, .CB = WT, .reservedPages = 0,
                                        .pAddress = PROCESSMEMORY_REGION_START_ADDRESS, .PT = NULL,
                                        .pageStatus = processMemoryRegionStatus};

/* declarations of static functions */
static void mmu_initTTB(void);
static void mmu_setAllDomainAccesses(void);
static void mmu_initAllPT(void);
static int8_t mmu_initPT(PageTable_t* pt);
static void mmu_mapAllRegions(void);
static int8_t mmu_mapRegion(Region_t* region, uint16_t nrOfPages, uint8_t processId);
static int8_t mmu_mapSectionTableRegion(Region_t* region, uint16_t nrOfPages, uint8_t processId);
static int8_t mmu_mapCoarseTableRegion(Region_t* region, uint16_t nrOfPages, uint8_t processId);
static void mmu_attachPT(PageTable_t* pt, PageTable_t* masterPT);
static void mmu_setProcessPT(PageTable_t pt);
static int16_t mmu_findFreePagesInRegion(Region_t* region, uint16_t nrOfPages);
static uint16_t mmu_getPageIndexInRegion(Region_t* region, uint32_t pAddress);
static void mmu_reservePagesForProcess(Process_t* process);
static uint16_t mmu_getNrOfNeededPagesForProcess(uint32_t nrOfNeededBytes);
static void mmu_freePagesForProcess(Process_t* process);
static void mmu_freePTOfProcess(Process_t* process);


void mmu_initMMU(void) {

    /* initialize coprocessor 15 by telling it the address of the vector table */
    mmu_initCP15(VECTOR_TABLE_BASE_ADDRESS);

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

    /* flush TLB */
    mmu_flushTLB();

    /* enable MMU */
    enable = ENABLE_MMU | ENABLE_ALIGNMENT | ENABLE_I_CACHE;
    mmu_setMMUControl(enable, changeMask);
}

int8_t mmu_initProcess(uint32_t pAddress, uint16_t nrOfNeededPages) {

    uint16_t nrOfNeededPagesForPT = 4;

    int16_t freePageIndexForPT = mmu_findFreePagesInRegion(&pageTableRegion, nrOfNeededPagesForPT);
    int16_t pageIndexOfProcess = mmu_getPageIndexInRegion(&processMemoryRegion, pAddress);
    uint32_t* ptPtr;
    PageStatus_t* status = (PageStatus_t*)(processMemoryRegion.pageStatus + pageIndexOfProcess);

    if (status->reserved == 0) {
        if (freePageIndexForPT != -1) {

            ptPtr = (uint32_t*)((uint32_t)pageTableRegion.pAddress + freePageIndexForPT * SMALL_PAGE_SIZE);

            PageTable_t taskPT = {VIRTUAL_PROCESS_START_ADDRESS, (uint32_t)ptPtr, (uint32_t)ptPtr, MASTER, 0};

            PageStatus_t* ptStatus = (PageStatus_t*)(pageTableRegionStatus + freePageIndexForPT);
            Region_t taskPTRegion = {(uint32_t)ptPtr, SMALL_PAGE, nrOfNeededPagesForPT, RWRW, WT, 0, (uint32_t)ptPtr, &pageTablePT, ptStatus};
            pageTableRegion.reservedPages += nrOfNeededPagesForPT;

            uint8_t processId = processManager_getNextProcessId();
            PageStatus_t* taskRegionStatus = (PageStatus_t*)(processMemoryRegionStatus + pageIndexOfProcess);
            Region_t taskRegion = {VIRTUAL_PROCESS_START_ADDRESS, SECTION, nrOfNeededPages, RWRW, WT, 0, pAddress, &taskPT, taskRegionStatus};
            processMemoryRegion.reservedPages += nrOfNeededPages;

            mmu_mapRegion(&taskPTRegion, taskPTRegion.numPages, processId);
            mmu_setProcessPT(taskPT);
            mmu_initPT(&taskPT);

            PCB_t* pcb = processManager_loadProcess(taskRegion.vAddress, (uint32_t)taskRegion.vAddress + 0x10000);
            Process_t process = {.pcb = pcb, .pageTable = taskPT, .region = taskRegion};
            processes[pcb->processId] = process;
            mmu_reservePagesForProcess(&process);
            return PROCESS_INIT_OK;
        }
    }
    return PROCESS_INIT_NOT_OK;
}

void mmu_switchProcess(PCB_t* pcb) {

    Process_t* process = &processes[pcb->processId];
    mmu_setProcessPT(process->pageTable);
    g_currentMMUProcess = process;

    mmu_flushTLB();
    mmu_flushCache();
}

void mmu_killProcess(ProcessId_t processId) {

    Process_t* process = &processes[processId];
    mmu_freePagesForProcess(process);
    mmu_freePTOfProcess(process);
    processManager_killProcess(processId);
}

void mmu_handleSectionTranslationFault(uint32_t faultAddress) {

    uint8_t index;
    uint32_t vAddress;
    uint32_t pAddress;

    index = faultAddress / SECTION_SIZE;      // get index of page to map by dividing through section size

    vAddress = (uint32_t)g_currentMMUProcess->region.vAddress + index * SECTION_SIZE;
    pAddress = (uint32_t)g_currentMMUProcess->region.pAddress + index * SECTION_SIZE;

    uint8_t AP = g_currentMMUProcess->region.AP;
    uint8_t CB = g_currentMMUProcess->region.CB;
    PageStatus_t* status = (PageStatus_t*)(g_currentMMUProcess->region.pageStatus + index);
    PageTable_t* pt = &g_currentMMUProcess->pageTable;
    Region_t taskRegion = {vAddress, SECTION, 1, AP, CB, 0, pAddress, pt, status};

    mmu_mapRegion(&taskRegion, taskRegion.numPages, g_currentMMUProcess->pcb->processId);
    g_currentMMUProcess->region.reservedPages += taskRegion.reservedPages;
}

uint32_t* mmu_getPhysicalMemoryForProcess(uint32_t nrOfNeededBytes) {

    uint16_t nrOfNeededPages = mmu_getNrOfNeededPagesForProcess(nrOfNeededBytes);
    uint16_t pageIndex = mmu_findFreePagesInRegion(&processMemoryRegion, nrOfNeededPages);
    return (uint32_t*)((uint32_t)processMemoryRegion.pAddress + pageIndex * SECTION_SIZE);
}

static void mmu_initTTB(void) {
    mmu_setTTBCR();
    mmu_setTTBR1(masterPTOS.ptAddress, TTBR1_BIT_MASK);          /* master PT for OS */
}

static void mmu_setProcessPT(PageTable_t pt) {
    mmu_setTTBR0(pt.ptAddress, TTBR0_BIT_MASK);
}

static void mmu_initAllPT(void) {
    mmu_initPT(&masterPTOS);
    mmu_initPT(&pageTablePT);
}

/**
 * initializes a page table by taking the memory area allocated
 * for a page table and setting it with FAULT values
 */
static int8_t mmu_initPT(PageTable_t* pt) {

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
            return PT_INIT_NOT_OK;
        }
    }

    mmu_writeValueToPTE(PTEptr, value, nrOfEntries);

    return PT_INIT_OK;
}

static void mmu_mapAllRegions(void) {
    mmu_mapRegion(&bootRegion, bootRegion.numPages, 0);
    mmu_mapRegion(&kernelRegion, kernelRegion.numPages, 0);
    mmu_attachPT(&pageTablePT, &masterPTOS);
    mmu_mapRegion(&pageTableRegion, 12, 0);
}

static int8_t mmu_mapRegion(Region_t* region, uint16_t nrOfPages, uint8_t processId) {

    switch(region->PT->type) {
        case MASTER: {
            return mmu_mapSectionTableRegion(region, nrOfPages, processId);
        }
        case COARSE: {
            return mmu_mapCoarseTableRegion(region, nrOfPages, processId);
        }
        default: {
            printf("UNKNOWN page table type\n");
            return MAP_REGION_NOT_OK;
        }
    }
}

static int8_t mmu_mapSectionTableRegion(Region_t* region, uint16_t nrOfPages, uint8_t processId) {

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
    return MAP_REGION_OK;
}

static int8_t mmu_mapCoarseTableRegion(Region_t* region, uint16_t nrOfPages, uint8_t processId) {

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
            return MAP_REGION_NOT_OK;
        }
    }
    return MAP_REGION_OK;
}

static void mmu_attachPT(PageTable_t* pt, PageTable_t* masterPT) {

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

static void mmu_setAllDomainAccesses(void) {
    uint32_t right = MASK_ALL_DOM;
    mmu_setDomainAccess(right, MASK_ALL_DOM);
}

static int16_t mmu_findFreePagesInRegion(Region_t* region, uint16_t nrOfPages) {

    uint16_t counter = 0;
    int16_t index = 0;

    if (region->numPages == region->reservedPages) {
        return -1;
    }

    int i;
    for (i = 0; i < region->numPages; i++) {
        if (region->pageStatus[i].reserved == 0) {
            counter++;
            if (counter == 1) {
                index = i;
            }
            if (counter == nrOfPages) {
                return index;
            }
        }
        else {
            counter = 0;
        }
    }
    return -1;
}

static uint16_t mmu_getPageIndexInRegion(Region_t* region, uint32_t pAddress) {

    uint32_t diff = (uint32_t)pAddress - (uint32_t)region->pAddress;
    uint32_t pageSize;

    switch (region->pageSize) {
        case SMALL_PAGE:
            pageSize = SMALL_PAGE_SIZE;
            break;
        case LARGE_PAGE:
            pageSize = LARGE_PAGE_SIZE;
            break;
        case SECTION:
            pageSize = SECTION_SIZE;
            break;
    }

    return diff / pageSize;
}

static uint16_t mmu_getNrOfNeededPagesForProcess(uint32_t nrOfNeededBytes) {

    uint16_t nrOfNeededPages = nrOfNeededBytes / NR_OF_BYTES_IN_SECTION;
    uint16_t remainder = nrOfNeededBytes % NR_OF_BYTES_IN_SECTION;

    if (remainder > 0) {
        nrOfNeededPages++;
    }
    return nrOfNeededPages;
}

static void mmu_reservePagesForProcess(Process_t* process) {

    int i;
    PageStatus_t* status = process->region.pageStatus;
    for (i = 0; i < process->region.numPages; i++) {
        status->reserved = 1;
        status->processId = process->pcb->processId;
        status++;
    }
}

static void mmu_freePagesForProcess(Process_t* process) {

    int i;
    PageStatus_t* status = process->region.pageStatus;
    for (i = 0; i < process->region.numPages; i++) {
        status->reserved = 0;
        status->processId = 0;
        status++;
    }
}

static void mmu_freePTOfProcess(Process_t* process) {

    mmu_initPT(&process->pageTable);
    int i;
    PageStatus_t* status = pageTableRegion.pageStatus;
    for (i = 0; i < pageTableRegion.numPages; i++) {
        if (status->processId == process->pcb->processId) {
            status->reserved = 0;
            status->processId = 0;
        }
        status++;
    }
}



