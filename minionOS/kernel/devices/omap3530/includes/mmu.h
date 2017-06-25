/*
 * mmu.h
 *
 *  Created on: 09.05.2017
 *      Author: sabrinarosmann
 */

#ifndef KERNEL_DEVICES_OMAP3530_INCLUDES_MMU_H_
#define KERNEL_DEVICES_OMAP3530_INCLUDES_MMU_H_

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

#define TTBR0_BIT_MASK      0xFFF
#define TTBR1_BIT_MASK      0x3FFF

#endif /* KERNEL_DEVICES_OMAP3530_INCLUDES_MMU_H_ */
