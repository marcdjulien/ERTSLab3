/** @file time.S
 *
 * @brief time sycall wrapper
 *
 * @author Marc-Daniel Julien <mjulien@andrew.cmu.edu>
 * @date 2014-11-27
 */

#include <asm.h>
#include <bits/swi.h>
	.file "task_create.S"

FUNC(task_create)
    swi CREATE_SWI @make the system call
    @ Should not return if success
    @ Store the absolute value of return value in errno
    neg r0, r0
    ldr r1, =errno
    str r0, [r1]

    @ Store -1 in return value
    mov r0, #-1
    mov pc, lr
