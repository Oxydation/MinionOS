/*
 * mmu.h
 *
 *  Created on: 09.05.2017
 *      Author: sabrinarosmann
 */

#ifndef KERNEL_DEVICES_OMAP3530_INCLUDES_MMU_H_
#define KERNEL_DEVICES_OMAP3530_INCLUDES_MMU_H_

#include <inttypes.h>

/* page table types */
#define FAULT   0
#define COARSE  1
#define MASTER  2

/* page sizes */
#define SMALL_PAGE  0   /* 4 KB */
#define LARGE_PAGE  1   /* 64 KB */

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
#define CHANGE_ALL_DOM  0xffffffff

typedef struct {
    uint32_t vAddress;          // identifies the starting address of a 1 MB section of virtual memory controlled by either a section entry or an L2 page table
    uint32_t ptAddress;         // is the address where the page table is located in virtual memory
    uint32_t masterPtAddress;   // is the address of the parent master L1 page table
    uint8_t type;               // identifies the type of the page table, it can be COARSE or MASTER
    uint8_t dom;                // sets the domain assigned to the 1 MB memory blocks of an L1 table entry
} PageTable_t;

typedef struct {
    uint32_t vAddress;          // is the starting address of the region in virtual memory
    uint16_t pageSize;          // is the size of a virtual page
    uint16_t numPages;          // is the number of pages in the region
    uint8_t AP;                 // is the region access permissions
    uint8_t CB;                 // is the cache and write buffer attributes for the region
    uint32_t pAddress;          // is the starting address of the region in physical memory
    PageTable_t* PT;            // is a pointer to the Pagetable in which the region resides
} Region_t;

/* functions */
void mmu_initAllPT(void);
int8_t mmu_initPT(PageTable_t* pt);

void mmu_mapAllRegions(void);
void mmu_mapRegion(Region_t* region);
void mmu_mapSectionTableRegion(Region_t* region);
int8_t mmu_mapCoarseTableRegion(Region_t* region);

void mmu_attachAllPT(void);
int8_t mmu_attachPT(PageTable_t* pt);

void mmu_setAllDomainAccesses(void);
void mmu_setDomainAccess(uint32_t value, uint32_t mask);

void mmu_setControl(uint32_t value, uint32_t mask);

void mmu_initMMU(void);

void ttbSet(uint8_t ttb);
void flushCache(void);
void flushTLB(void);
void processIDSet(uint8_t value);

#endif /* KERNEL_DEVICES_OMAP3530_INCLUDES_MMU_H_ */
