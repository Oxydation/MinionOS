.section .text
	.global mmu_writeValueToPTE

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
	add r0, r0, #32 		; increment counter
	cmp r0, r7			; check the limit
	ble loop			; loop if not finished
end:
	mov pc, lr			; jump back to calling function
