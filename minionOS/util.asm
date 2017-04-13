	.global asm_dataSynchronizationBarrier

asm_dataSynchronizationBarrier
	MOV R0, #0
	MCR P15, #0, R0, C7, C10, #4
	; check omap tech. ref; page 1061
