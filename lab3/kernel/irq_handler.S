@ irq_handler.S: irq Handler
@
@ Author: Marc-Daniel Julien <mjulien@andrew.cmu.edu>
@ Date:   Tue, 6 Nov 2014 11:20:33 -0400


    .extern C_IRQ_Handler

    .file   "irq_handler.S"
    .text

    .global irq_handler
irq_handler:
    mov     sp, #0xA2000000
    sub     sp, sp, #4
    stmfd   sp!, {r0-r12, lr}

    @ get spsr into general purpose (gp) registers
    mrs     r2, spsr

    @ store spsr above gp registers
    str     r2, [sp, #14*4]

    @ go to handler
    bl      C_IRQ_Handler

    @ restore spsr
    ldr     r2, [sp, #14*4]
    msr     spsr, r2

    ldmfd   sp!, {r0-r12, lr}

    @ remove space used to store spsr
    add     sp, sp, #4
    movs    pc, lr
