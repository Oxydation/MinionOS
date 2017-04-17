	.global asm_saveContext
	.global asm_loadContext
	.global asm_continuePreviousProcess

asm_saveContext
	; First store the old process's User mode state to the PCB pointed to by R0.
	MRS		R12, SPSR					; Get CPSR of interrupted process
	STR		R12, [R0], #8				; Store CPSR to PCB, point R0 at PCB location for R0 value
										; Store R12 to R0, then R0 = R0 + 8
	LDMFD	R13!, {R2, R3} 				; Reload R0/R1 of interrupted process from stack
	STMIA	R0!, {R2, R3} 				; Store R0/R1 values to PCB, point R0 at PCB location for R2 value
	LDR		R2, [R13, #12]
	STR		R2, [R0, #-12] 				; Store R14_irq, the interrupted process's restart adress
	LDMFD	R13!, {R2, R3, R12}			; Reload remaining stacked values (value of R14 into R1)
	STMIA	R0, {R2-R14}^ 				; Store users R2-R14 - see Note 1
	MOV PC, R14 						; Return to address in R14_irq

asm_loadContext
	; Incr SP -> TOS contains LR we don't need anymore
	;ADD R13, R13, #4
	; Then load the new process's User mode state and return to it.
	LDMIA	R0!, {R12, R14} 			; Put interrupted process's CPSR
	MSR		SPSR_fsxc, R12 				; and restart address in SPSR_irq and R14_irq
	LDMIA	R0, {R0-R14}^ 				; Load user R0-R14 - see Note 2
	NOP									; Note: Cannot use banked register immediately after User mode LDM
	MOVS	PC, R14

asm_continuePreviousProcess
	LDR R14, [SP] ; Load LR from stack
	ADD SP, SP, #4
	MOVS PC, R14 ; Return to user process
