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
	PROCESS:			o = 0x00220000	l = 0x10000000
	STACK_MEMORY:		o = 0x00200000	l = 0x00010000
	SYSMEM_MEMORY:		o =	0x00210000	l = 0x00010000
	ARGC_ARGV_MEMORY:	o = 0x00100000	l = 0x00010000
}

SECTIONS
{
	ORDER
	//.init_args	   >  ARGC_ARGV_MEMORY
    .text          >  PROCESS {
    	init.obj
    	*(.text)
    }
    .stack         >  STACK_MEMORY
    .bss           >  PROCESS
    .cio           >  PROCESS
    .const         >  PROCESS
    .data          >  PROCESS
    .switch        >  PROCESS
    .sysmem        >  SYSMEM_MEMORY
    .far           >  PROCESS
    .args          >  PROCESS
    .ppinfo        >  PROCESS
    .ppdata        >  PROCESS
  
    /* TI-ABI or COFF sections */
    .pinit         >  PROCESS
    .cinit         >  PROCESS
  
    /* EABI sections */
    .binit         >  PROCESS
    .init_array    >  PROCESS
    .neardata      >  PROCESS
    .fardata       >  PROCESS
    .rodata        >  PROCESS
    .c6xabi.exidx  >  PROCESS
    .c6xabi.extab  >  PROCESS
}
