/** @file event_wait.S
 *
 * @brief time sycall wrapper
 *
 * @author Marc-Daniel Julien <mjulien@andrew.cmu.edu>
 * @date 2014-11-27
 */

#include <asm.h>

	.file "event_wait.S"

FUNC(event_wait)
    swi  EVENT_WAIT @make the system call

    @ Return on success
    cmp r0, #0 @success
    movpl pc, lr @pl means >= 0

    @ Store the absolute value of return value in errno
    neg r0, r0
    ldr r1, =errno
    str r0, [r1]

    @ Store -1 in return value
    mov r0, #-1
    mov pc, lr
