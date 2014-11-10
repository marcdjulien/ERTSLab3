@ user_setup.S: helper function written in asm called from kernel
@
@ Author: Anand Pattabiraman <apattabi@andrew.cmu.edu>
@ Modified by: 
@ Marc-Daniel Julien <mjulien@andrew.cmu.edu>
@ Jeffery He <jmhe@andrew.cmu.edu>
@ Date:   Wed, 29 Oct 2014
@
@ Hint:
@   CPSR:
@       N|Z|C|V| [27:9] |I|F|T| M[4:0]
@   I -> 1
@   F -> 1
@   T -> T
@   CPSR_SVC = 0x10011 -> CPSR_USER = 0x10000
#include <arm/timer.h>

    .extern user_setup_stack_ptr

    .file   "user_setup.S"
    .text

    .global user_setup
user_setup:
    @ Save gp registers, lr, and cpsr so that we can return to kernel main later.
    stmfd   sp!, {r1-r12, lr}
    mrs     r2, cpsr
    stmfd   sp!, {r2}

    @ Save the stack pointer in a global variable for exit_handler.S.
    ldr     r2, =user_setup_stack_ptr
    str     sp, [r2] @ *user_setup_stack_ptr = sp

    @ Save the address of r8/kernel global data structure thing 
    ldr     r2, =r8_address
    str     r8, [r2]

    @ change mode to USER
    mrs     r2, cpsr
    orr     r2, r2, #0x040  @ Mask FIQ flag bits.
    bic     r2, r2, #0x003  @ Change mode bits to user.
    bic     r2, r2, #0x080  @ Enable IRQs
    msr     cpsr, r2

    @ set up user stack from spTop passed in from kernel.c
    mov     sp, r0

    @ go to user program
    ldr     pc, =0xa0000000

    @@@ exit syscall essentially returns through here!
