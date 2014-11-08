/** @file time.S
 *
 * @brief time sycall wrapper
 *
 * @author Marc-Daniel Julien <mjulien@andrew.cmu.edu>
 * @date 7 Nov 2014
 */

#include <asm.h>
#include <bits/swi.h>

	.file "time.S"

FUNC(time)
	@ Add code here.  Put your name at the top.
	swi SLEEP_SWI

    @ Store the absolute value of return value in errno
    neg r0, r0
    ldr r1, =errno
    str r0, [r1]

    @ Store -1 in return value
    mov r0, #-1
    mov pc, lr
