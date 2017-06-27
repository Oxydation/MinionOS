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
	PROCESS:			o = 0x00100000	l = 0x3FF00000
}

SECTIONS
{
	.text		> PROCESS
	.data		> PROCESS
	.bss		> PROCESS
	.stack		> PROCESS
	.cinit		> PROCESS
	.const		> PROCESS
	.sysmem		> PROCESS
}
