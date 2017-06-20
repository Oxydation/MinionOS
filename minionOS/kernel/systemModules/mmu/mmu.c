/*
 * mmu.c
 *
 *  Created on: 14.06.2017
 *      Author: sabrinarosmann
 */

#include <kernel/systemModules/mmu/mmu.h>

static Process_t g_processes[MAX_ALLOWED_PROCESSES];
static Process_t* g_currentMMUProcess;

/* Page tables */
/* VADDRESS, PTADDRESS, MasterPTADDRESS, PTTYPE, DOM */
static PageTable_t g_masterPTOS = { .vAddress = KERNEL_REGION_START_ADDRESS,
                                  .ptAddress = MASTER_PT_OS_START_ADDRESS,
                                  .masterPtAddress = MASTER_PT_OS_START_ADDRESS,
                                  .type = MASTER,
                                  .dom = 0};

static PageTable_t g_pageTablePT = { .vAddress = PAGE_TABLE_REGION_START_ADDRESS,
                                   .ptAddress = PAGETABLE_PT_START_ADDRESS,
                                   .masterPtAddress = MASTER_PT_OS_START_ADDRESS,
                                   .type = COARSE,
                                   .dom = 0};

/* Page status arrays */
static PageStatus_t g_bootRegionStatus[NR_OF_PAGES_IN_BOOT_REGION];                   // 1024 pages with 1 MB
static PageStatus_t g_kernelRegionStatus[NR_OF_PAGES_IN_KERNEL_REGION];               // 5 pages with 1 MB
static PageStatus_t g_pageTableRegionStatus[NR_OF_PAGES_IN_PAGE_TABLE_REGION];        // 256 pages with 4 KB
static PageStatus_t g_processMemoryRegionStatus[NR_OF_PAGES_IN_PROCESSMEMORY_REGION]; // 1018 pages with 1 MB

/* Region tables */
/* VADDRESS, PAGESIZE, NUMPAGES, AP, CB, nrOfReservedPages, PADDRESS, &PT, page status */
static Region_t g_bootRegion = { .vAddress = BOOT_REGION_START_ADDRESS, .pageSize = SECTION, .numPages = NR_OF_PAGES_IN_BOOT_REGION,
                                 .AP = RWRW, .CB = WT, .reservedPages = 0, .pAddress = BOOT_REGION_START_ADDRESS,
                                 .PT = &g_masterPTOS, .pageStatus = g_bootRegionStatus};

static Region_t g_kernelRegion = { .vAddress = KERNEL_REGION_START_ADDRESS, .pageSize = SECTION, .numPages = NR_OF_PAGES_IN_KERNEL_REGION,
                                   .AP = RWRW, .CB = WT, .reservedPages = 0, .pAddress = KERNEL_REGION_START_ADDRESS,
                                   .PT = &g_masterPTOS, .pageStatus = g_kernelRegionStatus};

static Region_t g_pageTableRegion = { .vAddress = PAGE_TABLE_REGION_START_ADDRESS, .pageSize = SMALL_PAGE,
                                      .numPages = NR_OF_PAGES_IN_PAGE_TABLE_REGION, .AP = RWRW, .CB = WT, .reservedPages = 0,
                                      .pAddress = PAGE_TABLE_REGION_START_ADDRESS, .PT = &g_pageTablePT,
                                      .pageStatus = g_pageTableRegionStatus};

static Region_t g_processMemoryRegion = { .vAddress = PROCESSMEMORY_REGION_START_ADDRESS, .pageSize = SECTION,
                                          .numPages = NR_OF_PAGES_IN_PROCESSMEMORY_REGION, .AP = RWRW, .CB = WT, .reservedPages = 0,
                                          .pAddress = PROCESSMEMORY_REGION_START_ADDRESS, .PT = NULL,
                                          .pageStatus = g_processMemoryRegionStatus};

/* declarations of static functions */
static void mmu_initTTB(void);
static void mmu_setAllDomainAccesses(void);
static void mmu_initAllPT(void);
static int8_t mmu_initPT(PageTable_t* pt);
static void mmu_mapAllRegions(void);
static int8_t mmu_mapRegion(Region_t* region, uint16_t nrOfPages, int16_t processId);
static int8_t mmu_mapSectionTableRegion(Region_t* region, uint16_t nrOfPages, int16_t processId);
static int8_t mmu_mapCoarseTableRegion(Region_t* region, uint16_t nrOfPages, int16_t processId);
static void mmu_attachPT(PageTable_t* pt, PageTable_t* masterPT);
static void mmu_setProcessPT(PageTable_t pt);
static int16_t mmu_findFreePagesInRegion(Region_t* region, uint16_t nrOfPages);
static uint16_t mmu_getPageIndexInRegion(Region_t* region, uint32_t pAddress);
static void mmu_reservePagesForProcess(Process_t* process);
static int32_t mmu_getNrOfNeededPagesForProcess(uint32_t nrOfNeededBytes);
static int32_t mmu_getNumberOfNeededPages(uint32_t nrOfNeededBytes, uint16_t pageSize);
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
    //enable = ENABLE_MMU | ENABLE_ALIGNMENT | ENABLE_I_CACHE;
    enable = ENABLE_MMU | ENABLE_I_CACHE;
    mmu_setMMUControl(enable, changeMask);
}

int8_t mmu_initProcess(uint32_t pAddress, uint32_t vAddress, uint32_t nrOfNeededBytes, PCB_t* pPcb) {

    uint16_t nrOfNeededPagesForPT = 4;
    uint16_t nrOfNeededPagesForProcess = mmu_getNrOfNeededPagesForProcess(nrOfNeededBytes);

    int16_t freePageIndexForPT = mmu_findFreePagesInRegion(&g_pageTableRegion, nrOfNeededPagesForPT);
    int16_t pageIndexOfProcess = mmu_getPageIndexInRegion(&g_processMemoryRegion, pAddress);
    uint32_t* pPT;
    PageStatus_t* pStatus = (PageStatus_t*)(g_processMemoryRegion.pageStatus + pageIndexOfProcess);

    if (pStatus->reserved == 0) {
        if (freePageIndexForPT != -1) {

            pPT = (uint32_t*)((uint32_t)g_pageTableRegion.pAddress + freePageIndexForPT * SMALL_PAGE_SIZE);

            PageTable_t taskPT = {vAddress, (uint32_t)pPT, (uint32_t)pPT, MASTER, 0};

            PageStatus_t* pPTStatus = (PageStatus_t*)(g_pageTableRegionStatus + freePageIndexForPT);
            Region_t taskPTRegion = {(uint32_t)pPT, SMALL_PAGE, nrOfNeededPagesForPT, RWRW, WT, 0, (uint32_t)pPT, &g_pageTablePT, pPTStatus};
            g_pageTableRegion.reservedPages += nrOfNeededPagesForPT;

            PageStatus_t* pTaskRegionStatus = (PageStatus_t*)(g_processMemoryRegionStatus + pageIndexOfProcess);
            Region_t taskRegion = {vAddress, SECTION, nrOfNeededPagesForProcess, RWRW, WT, 0, pAddress, &taskPT, pTaskRegionStatus};
            g_processMemoryRegion.reservedPages += nrOfNeededPagesForProcess;

            mmu_mapRegion(&taskPTRegion, taskPTRegion.numPages, pPcb->processId);
            mmu_setProcessPT(taskPT);
            mmu_initPT(&taskPT);

            Process_t process = {.pcb = pPcb, .pageTable = taskPT, .region = taskRegion};
            g_processes[pPcb->processId] = process;
            mmu_reservePagesForProcess(&process);
            return PROCESS_INIT_OK;
        }
    }
    return PROCESS_INIT_NOT_OK;
}

int8_t mmu_mapRegionDirectly(uint32_t pAddress, uint32_t nrOfNeededBytes, uint16_t pageSize) {

    uint16_t nrOfNeededPages = mmu_getNumberOfNeededPages(nrOfNeededBytes, pageSize);
    Region_t directlyMappedRegion = { .vAddress = pAddress, .pageSize = pageSize,  .numPages = nrOfNeededPages, .AP = RWRW, .CB = WT,
                                        .reservedPages = 0, .pAddress = pAddress, .PT = &g_masterPTOS};
    return mmu_mapRegion(&directlyMappedRegion, nrOfNeededPages, -1);
}

void mmu_switchProcess(PCB_t* pcb) {

    Process_t* pProcess = &g_processes[pcb->processId];
    mmu_setProcessPT(pProcess->pageTable);
    g_currentMMUProcess = pProcess;

    mmu_flushTLB();
    mmu_flushCache();
}

void mmu_killProcess(ProcessId_t processId) {

    Process_t* pProcess = &g_processes[processId];
    mmu_freePagesForProcess(pProcess);
    mmu_freePTOfProcess(pProcess);
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
    PageStatus_t* pStatus = (PageStatus_t*)(g_currentMMUProcess->region.pageStatus + index);
    PageTable_t* pPT = &g_currentMMUProcess->pageTable;
    Region_t taskRegion = {vAddress, SECTION, 1, AP, CB, 0, pAddress, pPT, pStatus};

    mmu_mapRegion(&taskRegion, taskRegion.numPages, g_currentMMUProcess->pcb->processId);
    g_currentMMUProcess->region.reservedPages += taskRegion.reservedPages;
}

uint32_t* mmu_getPhysicalMemoryForProcess(uint32_t nrOfNeededBytes) {

    uint16_t nrOfNeededPages = mmu_getNrOfNeededPagesForProcess(nrOfNeededBytes);
    uint16_t pageIndex = mmu_findFreePagesInRegion(&g_processMemoryRegion, nrOfNeededPages);
    return (uint32_t*)((uint32_t)g_processMemoryRegion.pAddress + pageIndex * SECTION_SIZE);
}

static void mmu_initTTB(void) {
    mmu_setTTBCR();
    mmu_setTTBR1(g_masterPTOS.ptAddress, TTBR1_BIT_MASK);          /* master PT for OS */
}

static void mmu_setProcessPT(PageTable_t pt) {
    mmu_setTTBR0(pt.ptAddress, TTBR0_BIT_MASK);
}

static void mmu_initAllPT(void) {
    mmu_initPT(&g_masterPTOS);
    mmu_initPT(&g_pageTablePT);
}

/**
 * initializes a page table by taking the memory area allocated
 * for a page table and setting it with FAULT values
 */
static int8_t mmu_initPT(PageTable_t* pt) {

    uint16_t nrOfEntries;                            /* number of entries in the page table */
    uint32_t value;                                 /* page table entry */
    uint32_t* pPTE = (uint32_t*)pt->ptAddress;    /* points to page table entry in PT */

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

    mmu_writeValueToPTE(pPTE, value, nrOfEntries);

    return PT_INIT_OK;
}

static void mmu_mapAllRegions(void) {
    mmu_mapRegion(&g_bootRegion, g_bootRegion.numPages, 0);
    mmu_mapRegion(&g_kernelRegion, g_kernelRegion.numPages, 0);
    mmu_attachPT(&g_pageTablePT, &g_masterPTOS);
    mmu_mapRegion(&g_pageTableRegion, 12, 0);
}

static int8_t mmu_mapRegion(Region_t* region, uint16_t nrOfPages, int16_t processId) {

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

static int8_t mmu_mapSectionTableRegion(Region_t* region, uint16_t nrOfPages, int16_t processId) {

    int16_t i;
    uint32_t* pPTE;                                       /* pointer to page table entry */
    uint32_t pAddress;                                      /* physical address */
    uint32_t tableIndex;
    uint8_t domain = region->PT->dom;
    uint8_t buffered = region->CB & 0x1;
    uint8_t cached = region->CB >> 1;
    uint8_t AP = region->AP;

    pPTE = (uint32_t*)region->PT->ptAddress;
    tableIndex = region->vAddress;
    tableIndex = tableIndex >> 20;                          /* take bits [31:20] as table index */
    tableIndex = tableIndex << 2;
    pPTE = (uint32_t*)((uint32_t)pPTE + tableIndex);    /* set to first PTE in region */

    pAddress = region->pAddress & 0xfff00000;               /* take only bits [31:20] */
    PageStatus_t* pStatus = region->pageStatus;
    for (i = 0; i < nrOfPages; i++) {
        uint32_t descriptor = mmu_createFirstLevelSectionDescriptor(domain, buffered, cached, AP);
        descriptor &= ~0xFFF00000;
        descriptor |= pAddress;
        *pPTE++ = descriptor;
        if (processId >= 0) {
            pStatus->reserved = 1;
            pStatus->processId = processId;
            pStatus++;
        }
        pAddress += (1 << 20);                              /* jump to start of next 1 MB section */
    }
    region->reservedPages = region->reservedPages + nrOfPages;
    return MAP_REGION_OK;
}

static int8_t mmu_mapCoarseTableRegion(Region_t* region, uint16_t nrOfPages, int16_t processId) {

    int16_t i;
    uint32_t* pPTE;                                       /* pointer to page table entry */
    uint32_t pAddress;                                      /* physical address */
    uint32_t tableIndex;
    uint8_t buffered = region->CB & 0x1;
    uint8_t cached = region->CB >> 1;
    uint8_t AP = region->AP;

    pPTE = (uint32_t*)region->PT->ptAddress;                  /* base address of the page table */

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
            pPTE = (uint32_t*)((uint32_t)pPTE + tableIndex);    /* set to first PTE in region */

            pAddress = region->pAddress & 0xFFFFF000;               /* take only bits [31:12] */
            PageStatus_t* pStatus = region->pageStatus;
            for (i = 0; i < nrOfPages; i++) {
                uint32_t descriptor = mmu_createSecondLevelSmallPageDescriptor(buffered, cached, AP);
                descriptor &= ~0xFFFFF000;
                descriptor |= pAddress;
                *pPTE++ = descriptor;
                pStatus->reserved = 1;
                pStatus->processId = processId;
                pStatus++;
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

    uint32_t* pMasterPTE;
    uint32_t tableIndex;
    uint32_t pteEntry;
    uint32_t pAddress;

    pMasterPTE = (uint32_t*)masterPT->ptAddress;

    tableIndex = pt->vAddress;
    tableIndex = tableIndex >> 20;      /* take bits [31:20] as table index */
    tableIndex = tableIndex << 2;       /* add to 0-bits at the end */
    pAddress = pt->ptAddress & 0xFFFFF000;

    pMasterPTE = (uint32_t*)((uint32_t)pMasterPTE + tableIndex);
    pteEntry = mmu_createFirstLevelPageTableDescriptor(pt->dom);
    pteEntry &= ~0xFFFFF000;
    pteEntry |= pAddress;
    *pMasterPTE = pteEntry;
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

static int32_t mmu_getNrOfNeededPagesForProcess(uint32_t nrOfNeededBytes) {

    return mmu_getNumberOfNeededPages(nrOfNeededBytes, SECTION);
}

static int32_t mmu_getNumberOfNeededPages(uint32_t nrOfNeededBytes, uint16_t pageSize) {

    uint32_t nrOfBytesInPage;

    switch (pageSize) {
        case SMALL_PAGE:
            nrOfBytesInPage = NR_OF_BYTES_IN_SMALL_PAGE;
            break;
        case LARGE_PAGE:
            nrOfBytesInPage = NR_OF_BYTES_IN_LARGE_PAGE;
            break;
        case SECTION:
            nrOfBytesInPage = NR_OF_BYTES_IN_SECTION;
            break;
        default:
            printf("UNKNOWN page size \n");
            return -1;
    }

    uint32_t nrOfNeededPages = nrOfNeededBytes / nrOfBytesInPage;
    uint32_t remainder = nrOfNeededBytes % nrOfBytesInPage;

    if (remainder > 0) {
        nrOfNeededPages++;
    }

    return nrOfNeededPages;
}

static void mmu_reservePagesForProcess(Process_t* process) {

    int i;
    PageStatus_t* pStatus = process->region.pageStatus;
    for (i = 0; i < process->region.numPages; i++) {
        pStatus->reserved = 1;
        pStatus->processId = process->pcb->processId;
        pStatus++;
    }
}

static void mmu_freePagesForProcess(Process_t* process) {

    int i;
    PageStatus_t* pStatus = process->region.pageStatus;
    for (i = 0; i < process->region.numPages; i++) {
        pStatus->reserved = 0;
        pStatus->processId = 0;
        pStatus++;
    }
}

static void mmu_freePTOfProcess(Process_t* process) {

    mmu_initPT(&process->pageTable);
    int i;
    PageStatus_t* pStatus = g_pageTableRegion.pageStatus;
    for (i = 0; i < g_pageTableRegion.numPages; i++) {
        if (pStatus->processId == process->pcb->processId) {
            pStatus->reserved = 0;
            pStatus->processId = 0;
        }
        pStatus++;
    }
}



