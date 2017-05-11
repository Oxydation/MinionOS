.section .text
	.global mmu_copyPTEValue

mmu_copyPTEValue:
	mov r5, r0		; save input parameter (PTE) to register r5
	mov r0, r5		; save PTE to register r0
	mov r1, r5		; save PTE to register r1
	mov r2, r5		; save PTE to register r2
	mov r3, r5		; save PTE to register r3
	mov pc, lr		; jump back to calling function
