@ swi_handler.S: SWI Handler
@
@ Author: Harry Q Bovik <hqbovik@andrew.cmu.edu>
@ Modified by: 
@ Marc-Daniel Julien <mjulien@andrew.cmu.edu>
@ Jeffery He <jmhe@andrew.cmu.edu>
@ Date:   Tue, 23 Oct 2007 11:20:33 -0400
@
@ Hint: To change the SWI handler in class, we need to accommodate a return
@   value from the C_SWI_Handler.

    .extern C_SWI_Handler

    .file   "swi_handler.S"
    .text

    .global swi_handler
swi_handler:

    @ Switch to this tasks kstack
    ldr ip, =cur_kstack
    ldr sp, [ip]

    @ save user stuff
    @ '^' means user mode registers
    mrs   ip, spsr
    stmfd sp, {ip, sp, lr}^
    sub   sp, sp, #12

    @ Store reg contex and leave room for spsr 
    sub     sp, sp, #4
    stmfd   sp!, {r0-r12, lr}

    @ get spsr into general purpose (gp) registers
    mrs     r2, spsr

    @ store spsr above gp registers
    str     r2, [sp, #14*4]

    @ restore r8
    ldr     ip, =global_data
    ldr     r8, [ip]

    @ enable interrupts after saving regs
    mrs     r4, cpsr
    bic     r4, r4, #0x080  @ Enable IRQs
    msr     cpsr, r4

    @ Set Param 1: extract SWI and get SWI no. by bit-masking
    ldr     r0, [lr, #-4]
    bic     r0, r0, #0xff000000

    @ Set Param 2: pointer to parameters on stack
    mov     r1, sp

    @ go to handler
    bl      C_SWI_Handler

    @ restore spsr
    ldr     r2, [sp, #14*4]
    msr     spsr, r2

    @ Discard the r0 on the stack (r0 has ret value).
    add     sp, sp, #4
    ldmfd   sp!, {r1-r12, lr}

    @ remove space used to store spsr
    add     sp, sp, #4

    @ restore user mode stuff
    ldmfd sp, {ip, sp, lr}^
    msr   spsr, ip
    add   sp, sp, #12

    movs    pc, lr
