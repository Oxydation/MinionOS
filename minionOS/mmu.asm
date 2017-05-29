.section .text
	.global mmu_flushTLB
	.global mmu_setMMUControl
	.global mmu_setDomainAccess
	.global mmu_setTTBR0
	.global mmu_setTTBR1
	.global mmu_setTTBCR
	.global mmu_writeValueToPTE
	.global mmu_initCP15
	.global mmu_flushCache

;------------------------------------------------------------------------------------------------------

mmu_flushTLB:
	mov r1, #0						; save 0x0 into register r1
	mcr p15, #0, r1, c8, c7, #0		; flush instruction and data TLB
	mov pc, lr						; set return address

;------------------------------------------------------------------------------------------------------

mmu_flushCache:
	mov r1, #0						; save 0x0 into register r1
	mcr p15, #0, r1, c7, c5, #0		; flush instruction and data cache
	mov pc, lr						; set return address

;------------------------------------------------------------------------------------------------------

mmu_setMMUControl:
	mrc p15, #0, r2, c1, c0, #0		; save content of CP15:c1 register to register r2
	bic r2, r2, r1					; clear bits that change
	orr r2, r2, r0					; set bits
	mcr p15, #0, r2, c1, c0, #0		; save content of register r2 to CP15:c1 register
	nop								; time to allow MMU to finish
	nop
	nop
	nop
	nop
	mov pc, lr						; jump back to calling function

;------------------------------------------------------------------------------------------------------

mmu_setDomainAccess:
	mov r3, r0						; save value to set to register r3
	mov r4, r1						; save mask to register r4
	mrc p15, #0, r5, c3, c0, #0		; save content of CP15:c3 register to register r5
	bic r5, r5, r4					; clear bits that change
	orr r5, r5, r3					; set bits that change
	mcr p15, #0, r5, c3, c0, #0		; save content of register r5 to CP15:c3 register
	mov pc, lr						; jump back to calling function

;------------------------------------------------------------------------------------------------------

mmu_setTTBR0:
	bic r0, r0, r1					; clear bits [11:0]
	mcr p15, #0, r0, c2, c0, #0		; TTB -> CP15:c2:c0 (Translation Table Base Register 0)
	mov pc, lr						; jump back to calling function

;------------------------------------------------------------------------------------------------------

mmu_setTTBR1:
	bic r0, r0, r1					; clear bits [13:0]
	mcr p15, #0, r0, c2, c0, #1		; TTB -> CP15:c2:c1 (Translation Table Base Register 1)
	mov pc, lr						; jump back to calling function

;------------------------------------------------------------------------------------------------------

mmu_setTTBCR:
	mrc p15, #0, r1, c2, c0, #2		; save content of Translation Table Base Control Register to register 1
	mvn r2, #0
	bic r1, r1, r2					; clear all bits to enable translation table walk on a TLB miss for Tanslation Table Base Registers 0 and 1 (bits [5:4])
									; 	and to use the 32-bit translation system (bit [31])
	orr r1, r1, #0x2				; set N (bits [2:0]) to 0b010, so the first address translated with TTBR1 is 0x40000000
									; (see "Cortex A8 - TRM", Table B3-1)
	mcr p15, #0, r1, c2, c0, #2		; save content of register 1 to Translation Table Base Control Register
	mov pc, lr						; jump back to calling function

;------------------------------------------------------------------------------------------------------

mmu_writeValueToPTE:
	mov r5, r0			; save pointer to PTE to register r5
	mov r6, r1			; save value to write to register r6
	mov r7, r2			; save number of entries to write to register r7
	mov r1, r6			; save PTE value to register r0
	mov r2, r6			; save PTE value to register r1
	mov r3, r6			; save PTE value to register r2
	mov r4, r6			; save PTE value to register r3
	mov r0, #1			; initialize counter
loop:
	; write 32 entries at once
	stmia r5!, {r1-r4} 	; STMIA = store multiple, increment after - PTEprt = register containing start address - ! = enables base register writeback, must be set - {r0-r3} = valid registers to be loaded
	stmia r5!, {r1-r4}
	stmia r5!, {r1-r4}
	stmia r5!, {r1-r4}
	stmia r5!, {r1-r4}
	stmia r5!, {r1-r4}
	stmia r5!, {r1-r4}
	stmia r5!, {r1-r4}
	add r0, r0, #32 	; increment counter
	cmp r0, r7			; check the limit
	ble loop			; loop if not finished
end:
	mov pc, lr			; jump back to calling function

;------------------------------------------------------------------------------------------------------

mmu_initCP15:
	mcr p15, #0, r0, c12, c0, #0	; set vector table address
	mov pc, lr

;------------------------------------------------------------------------------------------------------

