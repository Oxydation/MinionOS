/****************************************************************************/
/*  OMAP3530.cmd                                                            */
/*  Copyright (c) 2010  Texas Instruments Incorporated                      */
/*  Author: Rafael de Souza                                                 */
/*                                                                          */
/*    Description: This file is a sample linker command file that can be    */
/*                 used for linking programs built with the C compiler and  */
/*                 running the resulting .out file on an OMAP3530.          */
/*                 Use it as a guideline.  You will want to                 */
/*                 change the memory layout to match your specific          */
/*                 target system.  You may want to change the allocation    */
/*                 scheme according to the size of your program.            */
/*                                                                          */
/****************************************************************************/

MEMORY
{


#ifndef DSP_CORE  /* ARM memory map */
    SRAM:           o = 0x40200000  l = 0x0000FFC4  /* 64kB Internal SRAM */
    INTVEC:			o = 0x4020FFC4	l = 0x0000003B  // ffcc?
    IVASHL2RAM:     o = 0x5C7F8000  l = 0x00008000  /* 32kB Shared IVA L2 RAM */
    IVASHL2RAM_C:   o = 0x5C800000  l = 0x00010000  /* 64kB Shared IVA L2 Cache RAM */
    IVASHL1PRAM:    o = 0x5CE00000  l = 0x00008000  /* 32kB Shared IVA L1 Program RAM */
    IVASHL1DRAM:    o = 0x5CF04000  l = 0x0000C000  /* 48kB Shared IVA L1 Data RAM */
    IVASHL1DRAM_C:  o = 0x5CF10000  l = 0x00008000  /* 32kB Shared IVA L1 Data Cache RAM */
    DDR0:           o = 0x80000000  l = 0x40000000  /* 1GB external DDR Bank 0 */
    DDR1:           o = 0xC0000000  l = 0x40000000  /* 1GB external DDR Bank 1 */

#else             /* DSP memory map */

    IVASHL2RAM:     o = 0x007F8000  l = 0x00008000  /* 32kB IVA L2 RAM */
    IVASHL2RAM_C:   o = 0x00800000  l = 0x00010000  /* 64kB IVA L2 Cache RAM */
    IVASHL1PRAM:    o = 0x00E00000  l = 0x00008000  /* 32kB IVA L1 Program RAM */
    IVASHL1DRAM:    o = 0x00F04000  l = 0x0000C000  /* 48kB IVA L1 Data RAM */
    IVASHL1DRAM_C:  o = 0x00F10000  l = 0x00008000  /* 32kB IVA L1 Data Cache RAM */

#endif
}

stackSize = 0x40000; // 1048kB

SECTIONS
{
	.intvecs	   >  INTVEC
	.ISR		   > SRAM
#ifndef DSP_CORE   /* ARM memory map */

    .text          >  DDR0
    .stack         >  DDR0
    .bss           >  DDR0
    .cio           >  DDR0
    .const         >  DDR0
    .data          >  DDR0
    .switch        >  DDR0
    .sysmem        >  DDR0
    .far           >  DDR0
    .args          >  DDR0
    .ppinfo        >  DDR0
    .ppdata        >  DDR0
  
    /* TI-ABI or COFF sections */
    .pinit         >  SRAM
    .cinit         >  SRAM
  
    /* EABI sections */
    .binit         >  SRAM
    .init_array    >  SRAM
    .neardata      >  SRAM
    .fardata       >  SRAM
    .rodata        >  SRAM
    .c6xabi.exidx  >  SRAM
    .c6xabi.extab  >  SRAM

    /* Stacks */
   	.stacks > DDR0 {
       . = align(4);
       . = . + stackSize;
       __stackUsr = .; /* User stack */
       . = . + stackSize;
       __stackIrq = .; /* IRQ Interrupt Stack */
       . = . + stackSize;
        __stackFiq = .; /* FIQ Interrupt Stack */
       . = . + stackSize;
       __stackSvc = .; /* Supervisor Interrupt Stack */
       . = . + stackSize;
       __stackAbt = .; /* Abort Interrupt Stack */
       . = . + stackSize;
        __stackDbt = .; /* Dbt Interrupt Stack */
       . = . + stackSize;
   }

#else              /* DSP memory map */

    .text          >  IVASHL2RAM
    .stack         >  IVASHL2RAM
    .bss           >  IVASHL2RAM
    .cio           >  IVASHL2RAM
    .const         >  IVASHL2RAM
    .data          >  IVASHL2RAM
    .switch        >  IVASHL2RAM
    .sysmem        >  IVASHL2RAM
    .far           >  IVASHL2RAM
    .args          >  IVASHL2RAM
    .ppinfo        >  IVASHL2RAM
    .ppdata        >  IVASHL2RAM
  
    /* TI-ABI or COFF sections */
    .pinit         >  IVASHL2RAM
    .cinit         >  IVASHL2RAM
  
    /* EABI sections */
    .binit         >  IVASHL2RAM
    .init_array    >  IVASHL2RAM
    .neardata      >  IVASHL2RAM
    .fardata       >  IVASHL2RAM
    .rodata        >  IVASHL2RAM
    .c6xabi.exidx  >  IVASHL2RAM
    .c6xabi.extab  >  IVASHL2RAM

#endif
}
