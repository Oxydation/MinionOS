/*
 * mmu.h
 *
 *  Created on: 09.05.2017
 *      Author: sabrinarosmann
 */

#ifndef KERNEL_DEVICES_OMAP3530_INCLUDES_MMU_H_
#define KERNEL_DEVICES_OMAP3530_INCLUDES_MMU_H_

#include <inttypes.h>
#include <stdio.h>
#include "kernel/systemModules/processManagement/processManager.h"
#include "kernel/systemModules/scheduler/scheduler.h"

#define VIRTUAL_START_ADDRESS   0x0000000

/* page table types */
#define FAULT   0
#define COARSE  1
#define MASTER  2

/* page sizes */
#define SMALL_PAGE  0   /* 4 KB */
#define LARGE_PAGE  1   /* 64 KB */
#define SECTION     2   /* 1 MB */

#define SMALL_PAGE_SIZE     0x1000
#define LARGE_PAGE_SIZE     0x10000
#define SECTION_SIZE        0x100000

#define NR_OF_BYTES_IN_SMALL_PAGE   4096
#define NR_OF_BYTES_IN_LARGE_PAGE   65536
#define NR_OF_BYTES_IN_SECTION      1048576

/* access permissions */
/* privileged mode / user mode */
/* NA = no access, RO = read only, RW = read/write */
#define NANA    0x0
#define RWNA    0x1
#define RWRO    0x2
#define RWRW    0x3

/* caches and write buffer */
#define cb      0x0     /* not cached / not buffered */
#define cB      0x1     /* not cached / buffered */
#define WT      0x2     /* write through cache */
#define WB      0x3     /* write back cache */

/* domains */
#define D0              0
#define D1              2
#define D2              4
#define D3              6
#define D4              8
#define D5              10
#define D6              12
#define D7              14
#define D8              16
#define D9              18
#define D10             20
#define D11             22
#define D12             24
#define D13             26
#define D14             28
#define D15             30
#define MASK_ALL_DOM    0xffffffff

/* CP15 c1 control register */
#define ENABLE_MMU          (1 << 0)
#define ENABLE_ALIGNMENT    (1 << 1)    /* alignment detection */
#define ENABLE_D_CACHE      (1 << 2)    /* data cache */
#define ENABLE_I_CACHE      (1 << 12)   /* instruction cache */

#define CHANGE_MMU          (1 << 0)
#define CHANGE_ALIGNMENT    (1 << 1)    /* alignment detection */
#define CHANGE_D_CACHE      (1 << 2)    /* data cache */
#define CHANGE_I_CACHE      (1 << 12)   /* instruction cache */
#define CHANGE_TRE          (1 << 28)   /* controls the TEX remap functionality in the MMU */
#define CHANGE_AFE          (1 << 29)   /* is the Access Flag Enable bit */

/* fault status codes */
#define ALIGNMENT_FAULT             1
#define TRANSLATION_FAULT_SECTION   5
#define TRANSLATION_FAULT_PAGE      7
#define ACCESS_FLAG_FAULT_SECTION   3
#define ACCESS_FLAG_FAULT_PAGE      6
#define DOMAIN_FAULT_SECTION        9
#define DOMAIN_FAULT_PAGE           11
#define PERMISSION_FAULT_SECTION    13
#define PERMISSION_FAULT_PAGE       15

/* structs */
typedef struct {
    uint32_t vAddress;          // identifies the starting address of a 1 MB section of virtual memory controlled by either a section entry or an L2 page table
    uint32_t ptAddress;         // is the address where the page table is located in virtual memory
    uint32_t masterPtAddress;   // is the address of the parent master L1 page table
    uint8_t type;               // identifies the type of the page table, it can be COARSE or MASTER
    uint8_t dom;                // sets the domain assigned to the 1 MB memory blocks of an L1 table entry
} PageTable_t;

typedef struct {
    char reserved;
    uint8_t processId;
} PageStatus_t;

typedef struct {
    uint32_t vAddress;          // is the starting address of the region in virtual memory
    uint16_t pageSize;          // is the size of a virtual page
    uint16_t numPages;          // is the number of pages in the region
    uint8_t AP;                 // is the region access permissions
    uint8_t CB;                 // is the cache and write buffer attributes for the region
    uint16_t reservedPages;     // number of reserved pages in region
    uint32_t pAddress;          // is the starting address of the region in physical memory
    PageTable_t* PT;            // is a pointer to the page table in which the region resides
    PageStatus_t* pageStatus;   // holds the status for each page of the region: 0 - page is free, 1 - page is occupied
} Region_t;

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

typedef struct {
    PCB_t* pcb;
    PageTable_t pageTable;
    Region_t region;
} Process_t;

/* functions for creating descriptors */
uint32_t mmu_createFirstLevelFaultDescriptor(void);
uint32_t mmu_createFirstLevelPageTableDescriptor(uint8_t domain);
uint32_t mmu_createFirstLevelSectionDescriptor(uint8_t domain, uint8_t buffered, uint8_t cached, uint8_t accessPermission);
uint32_t mmu_createSecondLevelFaultDescriptor(void);
uint32_t mmu_createSecondLevelLargePageDescriptor(uint8_t buffered, uint8_t cached, uint8_t accessPermission);
uint32_t mmu_createSecondLevelSmallPageDescriptor(uint8_t buffered, uint8_t cached, uint8_t accessPermission);

/* functions for initializing MMU */
void mmu_initAllPT(void);
int8_t mmu_initPT(PageTable_t* pt);

void mmu_mapAllRegions(void);
void mmu_mapRegion(Region_t* region, uint16_t nrOfPages, uint8_t processId);
void mmu_mapSectionTableRegion(Region_t* region, uint16_t nrOfPages, uint8_t processId);
int8_t mmu_mapCoarseTableRegion(Region_t* region, uint16_t nrOfPages, uint8_t processId);

void mmu_attachPT(PageTable_t* pt, PageTable_t* masterPT);

void mmu_setAllDomainAccesses(void);
void mmu_setDomainAccess(uint32_t value, uint32_t mask);

void mmu_setMMUControl(uint32_t value, uint32_t mask);

void mmu_initMMU(void);

void mmu_flushCache(void);
void mmu_flushTLB(void);

void mmu_setProcessPT(PageTable_t pt);

/* assembler functions */
void mmu_writeValueToPTE(uint32_t* PTEptr, uint32_t value, uint16_t nrOfEntries);
void mmu_initTTB(void);
void mmu_setTTBR0(uint32_t ttb, uint32_t clearBitmask);
void mmu_setTTBR1(uint32_t ttb, uint32_t clearBitmask);
void mmu_setTTBCR(void);
void mmu_initCP15(uint32_t vectorTableAddress);
uint8_t mmu_getDataFaultStatus(void);
uint32_t mmu_getDataFaultAddress(void);
uint8_t mmu_getInstructionFaultStatus(void);
uint32_t mmu_getInstructionFaultAddress(void);

/* functions for process management */
void mmu_initProcess(uint32_t pAddress, uint16_t nrOfNeededPages);
void mmu_switchProcess(PCB_t* pcb);
void mmu_killProcess(ProcessId_t processId);
uint32_t* mmu_getPhysicalMemoryForProcess(uint32_t nrOfNeededBytes);

int16_t mmu_findFreePagesInRegion(Region_t* region, uint16_t nrOfPages);
uint16_t mmu_getPageIndexInRegion(Region_t* region, uint32_t pAddress);
void mmu_reservePagesForProcess(Process_t* process);
void mmu_freePagesForProcess(Process_t* process);
void mmu_freePTOfProcess(Process_t* process);

void mmu_handleSectionTranslationFault(uint32_t faultAddress);

#endif /* KERNEL_DEVICES_OMAP3530_INCLUDES_MMU_H_ */
