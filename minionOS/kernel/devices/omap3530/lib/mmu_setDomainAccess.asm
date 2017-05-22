.section .text
	.global mmu_setDomainAccess

mmu_setDomainAccess:
	mov r3, r0						; save value to set to register r3
	mov r4, r1						; save mask to register r4
	mrc p15, #0, r5, c3, c0, #0		; save content of CP15:c3 register to register r5
	bic r5, r5, r4					; clear bits that change
	orr r5, r5, r3					; set bits that change
	mcr p15, #0, r5, c3, c0, #0		; save content of register r5 to CP15:c3 register
	mov pc, lr						; jump back to calling function
