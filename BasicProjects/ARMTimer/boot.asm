
;****************************************************************************
;* BOOT.ASM
;*
;* THIS IS THE INITAL BOOT ROUTINE FOR TMS470 C++ PROGRAMS.
;* IT MUST BE LINKED AND LOADED WITH ALL C++ PROGRAMS.
;*
;* THIS MODULE PERFORMS THE FOLLOWING ACTIONS:
;*   1) ALLOCATES THE STACK AND INITIALIZES THE STACK POINTER
;*   2) CALLS AUTO-INITIALIZATION ROUTINE
;*   3) CALLS THE FUNCTION MAIN TO START THE C++ PROGRAM
;*   4) CALLS THE STANDARD EXIT ROUTINE
;*
;* THIS MODULE DEFINES THE FOLLOWING GLOBAL SYMBOLS:
;*   1) __stack     STACK MEMORY AREA
;*   2) _c_int00    BOOT ROUTINE
;*
;****************************************************************************
	.armfunc _c_int00

;****************************************************************************
; Accomodate different lowerd names in different ABIs
;****************************************************************************
   .if   __TI_EABI_ASSEMBLER
        .asg	_system_pre_init, PRE_INIT_RTN
        .asg	__TI_auto_init, AUTO_INIT_RTN
        .asg	_args_main,   ARGS_MAIN_RTN
        .asg	exit,         EXIT_RTN
        .asg    main_func_sp, MAIN_FUNC_SP
   .else ; COFF TI ARM9 ABI
        .asg	__system_pre_init, PRE_INIT_RTN
        .asg	__TI_auto_init, AUTO_INIT_RTN ; NOTE does not use COFF prefix
        .asg	__args_main,   ARGS_MAIN_RTN
        .asg	_exit,         EXIT_RTN
        .asg    _main_func_sp, MAIN_FUNC_SP
   .endif

;****************************************************************************
;*  32 BIT STATE BOOT ROUTINE                                               *
;****************************************************************************

;***************************************************************
;* Define stacks for supervisor, IRQ and abort mode.
;***************************************************************
		.global __stackSvc
		.global __stackAbt
		.global __stackIrq

stackSvc	.long	__stackSvc
stackAbt	.long	__stackAbt
stackIrq	.long	__stackIrq

        .global __stack
;***************************************************************
;* DEFINE THE USER MODE STACK (DEFAULT SIZE IS 512)
;***************************************************************
__stack:.usect  ".stack", 0, 4

        .global _c_int00
;***************************************************************
;* FUNCTION DEF: _c_int00
;***************************************************************
_c_int00: .asmfunc stack_usage(0)

        .if __TI_NEON_SUPPORT__ | __TI_VFP_SUPPORT__
        ;*------------------------------------------------------
        ;* SETUP PRIVILEGED AND USER MODE ACCESS TO COPROCESSORS
        ;* 10 AND 11, REQUIRED TO ENABLE NEON/VFP
        ;* COPROCESSOR ACCESS CONTROL REG
        ;* BITS [23:22] - CP11, [21:20] - CP10
        ;* SET TO 0b11 TO ENABLE USER AND PRIV MODE ACCESS
        ;*------------------------------------------------------
        MRC      p15,#0x0,r0,c1,c0,#2
        MOV      r3,#0xf00000
        ORR      r0,r0,r3
        MCR      p15,#0x0,r0,c1,c0,#2

        ;*------------------------------------------------------
        ; SET THE EN BIT, FPEXC[30] TO ENABLE NEON AND VFP
        ;*------------------------------------------------------
        MOV      r0,#0x40000000
        FMXR     FPEXC,r0
        .endif

        ;*------------------------------------------------------
        ;* SET TO USER MODE
        ;*------------------------------------------------------
        ;MRS     r0, cpsr
        ;BIC     r0, r0, #0x1F  ; CLEAR MODES
        ;ORR     r0, r0, #0x13  ; SET Supervisor MODE
        ;MSR     cpsr_cf, r0

        ;*------------------------------------------------------
        ;* INITIALIZE THE USER MODE STACK
        ;*------------------------------------------------------
        ;.if __TI_AVOID_EMBEDDED_CONSTANTS
        ;MOVW    sp, __stack
        ;MOVT    sp, __stack
        ;MOVW    r0, __STACK_SIZE
        ;MOVT    r0, __STACK_SIZE
        ;.else
        ;LDR     sp, c_stack
        ;LDR     r0, c_STACK_SIZE
        ;.endif
        ;ADD     sp, sp, r0

        ;*-----------------------------------------------------
        ;* ALIGN THE STACK TO 64-BITS IF EABI.
        ;*-----------------------------------------------------
        .if __TI_EABI_ASSEMBLER
        BIC     sp, sp, #0x07  ; Clear upper 3 bits for 64-bit alignment.
        .endif

        ;*-----------------------------------------------------
        ;* SAVE CURRENT STACK POINTER FOR SDP ANALYSIS
        ;*-----------------------------------------------------
        ;.if __TI_AVOID_EMBEDDED_CONSTANTS
        ;MOVW    r0, MAIN_FUNC_SP
        ;MOVT    r0, MAIN_FUNC_SP
        ;.else
        ;LDR     r0, c_mf_sp
        ;.endif
        ;STR     sp, [r0]

		;*-------------------------------------------------------
		;* Initialize the stacks, assign them

		;*-------------------------------------------------------
		;* Set Interrupt-Stack
		;*-------------------------------------------------------
		CPS		0x12 ; IRQ Mode 0b10010
		LDR		sp,	stackIrq

		;*-------------------------------------------------------
		;* Set AbortStack
		;*-------------------------------------------------------
		CPS      0x17 ; Abort mode 0b10111
		LDR      sp, stackAbt

		;*-------------------------------------------------------
		;* Set KernelStack
		;*-------------------------------------------------------
		CPS      0x13 ; Supervisor mode 0b 10011
		LDR      sp, stackSvc


        ;*------------------------------------------------------

		;* Call the __mpu_init hook function.
        ;*------------------------------------------------------
        BL      __mpu_init

        ;*------------------------------------------------------
        ;* Perform all the required initializations when
        ;* _system_pre_init() returns non-zero:
        ;*   - Process BINIT Table
        ;*   - Perform C auto initialization
        ;*   - Call global constructors
        ;*------------------------------------------------------
        BL      PRE_INIT_RTN
        CMP     R0, #0
        BEQ     bypass_auto_init
        BL      AUTO_INIT_RTN
bypass_auto_init:

        ;*------------------------------------------------------
        ;* CALL APPLICATION
        ;*------------------------------------------------------
        BL      ARGS_MAIN_RTN

        ;*------------------------------------------------------
        ;* IF APPLICATION DIDN'T CALL EXIT, CALL EXIT(1)
        ;*------------------------------------------------------
        MOV     R0, #1
        BL      EXIT_RTN

        ;*------------------------------------------------------
        ;* DONE, LOOP FOREVER
        ;*------------------------------------------------------
L1:     B       L1
        .endasmfunc

;***************************************************************
;* CONSTANTS USED BY THIS MODULE
;***************************************************************
        .if !__TI_AVOID_EMBEDDED_CONSTANTS
c_stack         .long    __stack
c_STACK_SIZE    .long    __STACK_SIZE
c_mf_sp         .long    MAIN_FUNC_SP
        .endif

        .if __TI_EABI_ASSEMBLER
        .data
        .align 4
_stkchk_called:
        .field          0,32
        .else
        .sect   ".cinit"
        .align  4
        .field          4,32
        .field          _stkchk_called+0,32
        .field          0,32

        .bss    _stkchk_called,4,4
        .symdepend ".cinit", ".bss"
        .symdepend ".cinit", ".text"
        .symdepend ".bss", ".text"
        .endif

;******************************************************
;* UNDEFINED REFERENCES                               *
;******************************************************
	.global _stkchk_called
	.global	__STACK_SIZE
        .global PRE_INIT_RTN
        .global AUTO_INIT_RTN
	.global ARGS_MAIN_RTN
	.global MAIN_FUNC_SP
	.global	EXIT_RTN
	.global	__mpu_init

	.end
