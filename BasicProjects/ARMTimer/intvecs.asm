    .global isr_irq
;    .global isr_fiq
;    .global isr_swi
;    .global isr_reset
;    .global isr_pabt
;    .global isr_dabt

    .sect ".intvecs"
    B isr_irq ; reset interrupt
    .word 0; undefined instruction interrupt
    B isr_irq ; software interrupt
    B isr_irq ; abort (prefetch) interrupt
    B isr_irq ; abort (data) interrupt
    B isr_irq ; reserved
    B isr_irq ; IRQ interrupt
    B isr_irq ; FIQ interrupt
    ; check page 153 of spnu151n
