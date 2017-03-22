    .global isr_irq
;    .global isr_fiq
;    .global isr_swi
;    .global isr_reset
;    .global isr_pabt
;    .global isr_dabt

    .sect ".intvecs"
    .word 0 ; reset interrupt
    .word 0 ; undefined instruction interrupt
    .word 0 ; software interrupt
    .word 0 ; abort (prefetch) interrupt
    .word 0 ; abort (data) interrupt
    .word 0 ; reserved
    B isr_irq ; IRQ interrupt
    .word 0 ; FIQ interrupt
