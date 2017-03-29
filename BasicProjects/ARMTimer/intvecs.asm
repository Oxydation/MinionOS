	.global isr_reset
    .global isr_swii
    .global isr_irq
    .global isr_fiq
    .global isr_pabt
    .global isr_dabt
    .global isr_undef

    .sect ".intvecs"
   	B isr_reset ; reset interrupt
    B isr_undef; undefined instruction interrupt
  	B isr_swii ; software interrupt
    B isr_pabt; abort (prefetch) interrupt
    B isr_dabt; abort (data) interrupt
    .word 0; reserved
    B isr_irq; IRQ interrupt
    B isr_fiq; FIQ interrupt
    ; check page 153 of spnu151n
