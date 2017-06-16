/*
 * mmu.h
 *
 *  Created on: 14.06.2017
 *      Author: sabrinarosmann
 */

#ifndef KERNEL_SYSTEMMODULES_MMU_MMU_H_
#define KERNEL_SYSTEMMODULES_MMU_MMU_H_

#include <inttypes.h>
#include <stdio.h>
#include "kernel/systemModules/processManagement/processManager.h"
#include "kernel/devices/omap3530/includes/mmu.h"
#include "kernel/hal/mmu/mmu.h"

#define VIRTUAL_PROCESS_START_ADDRESS           0x00000000
#define BOOT_REGION_START_ADDRESS               0x40000000
#define KERNEL_REGION_START_ADDRESS             0x80000000
#define PAGE_TABLE_REGION_START_ADDRESS         0x80500000
#define PROCESSMEMORY_REGION_START_ADDRESS      0x80600000

#define MASTER_PT_OS_START_ADDRESS              0x80500000
#define PAGETABLE_PT_START_ADDRESS              0x80504000

#define NR_OF_PAGES_IN_BOOT_REGION              1024
#define NR_OF_PAGES_IN_KERNEL_REGION            5
#define NR_OF_PAGES_IN_PAGE_TABLE_REGION        256
#define NR_OF_PAGES_IN_PROCESSMEMORY_REGION     1018

/* page table types */
#define FAULT   0
#define COARSE  1
#define MASTER  2

/* page sizes */
#define SMALL_PAGE                  0   /* 4 KB */
#define LARGE_PAGE                  1   /* 64 KB */
#define SECTION                     2   /* 1 MB */

#define SMALL_PAGE_SIZE             0x1000      /* 4 KB */
#define LARGE_PAGE_SIZE             0x10000     /* 64 KB */
#define SECTION_SIZE                0x100000    /* 1 MB */

#define NR_OF_BYTES_IN_SMALL_PAGE   4096
#define NR_OF_BYTES_IN_LARGE_PAGE   65536
#define NR_OF_BYTES_IN_SECTION      1048576

#define VECTOR_TABLE_BASE_ADDRESS   0x4020FFC0

#define PROCESS_INIT_OK             1
#define PROCESS_INIT_NOT_OK         -1
#define PT_INIT_OK                  1
#define PT_INIT_NOT_OK              -1
#define MAP_REGION_OK               1
#define MAP_REGION_NOT_OK           -1

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

typedef struct {
    PCB_t* pcb;
    PageTable_t pageTable;
    Region_t region;
} Process_t;

/* functions for initializing MMU */
void mmu_initMMU(void);

/* functions for process management */
int8_t mmu_initProcess(uint32_t pAddress, uint32_t vAddress, uint32_t nrOfNeededBytes, PCB_t* pcb);
void mmu_switchProcess(PCB_t* pcb);
void mmu_killProcess(ProcessId_t processId);
uint32_t* mmu_getPhysicalMemoryForProcess(uint32_t nrOfNeededBytes);

/* functions for handling faults */
void mmu_handleSectionTranslationFault(uint32_t faultAddress);

#endif /* KERNEL_SYSTEMMODULES_MMU_MMU_H_ */
