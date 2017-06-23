	.armfunc _c_init00
	.global  _c_init00
	.asg main, ARGS_MAIN_RTN
	.global ARGS_MAIN_RTN
	.global __TI_auto_init
	;.cdecls C,LIST,"system.h" ;
	.ref main
	.ref exit

_c_init00: .asmfunc

	bl	__TI_auto_init
	mov r10, #0x00100000
	ldr r0,  [r10]
	add r1, r10, #0x10

	bl ARGS_MAIN_RTN
	bl exit
.end
