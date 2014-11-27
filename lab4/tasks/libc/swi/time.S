/** @file time.S
 *
 * @brief time sycall wrapper
 *
 * @author Marc-Daniel Julien <mjulien@andrew.cmu.edu>
 * @author Jeffery He <jmhe@andrew.cmu.edu>
 * @date 7 Nov 2014
 */

#include <asm.h>
#include <bits/swi.h>

	.file "time.S"

FUNC(time)
	swi TIME_SWI
    mov pc, lr
