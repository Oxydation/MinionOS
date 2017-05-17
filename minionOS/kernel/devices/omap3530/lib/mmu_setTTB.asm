.section .text
	.global mmu_setTTB

mmu_setTTB:
	mcr p15, #0, r0, c2, c0, #0		; TTB -> CP15:c2:c0
	mov pc, lr						; jump back to calling function
