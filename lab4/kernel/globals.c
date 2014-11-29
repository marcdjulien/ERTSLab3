/*
 * globals.c: Defines the global variables
 *
 * Author: Mike Kasick <mkasick@andrew.cmu.edu>
 * Modified by: 
 * Marc-Daniel Julien <mjulien@andrew.cmu.edu>
 * Jeffery He <jmhe@andrew.cmu.edu>
 * Date:   Sun, 07 Oct 2007 01:31:00 -0400
 */

int user_setup_stack_ptr;
volatile unsigned long global_time;
int r8_address;
int cur_kstack = 0xA2FF0000;
int cur_ustack;
