/** @file sleep.S
 *
 * @brief sleep sycall wrapper
 *
 * @author Marc-Daniel Julien <mjulien@andrew.cmu.edu>
 * @author Jeffery He <jmhe@andrew.cmu.edu>
 * @date 7 Nov 2014
 */

#include <asm.h>
#include <bits/swi.h>

	.file "sleep.S"

FUNC(sleep)
	swi SLEEP_SWI
    mov pc, lr
