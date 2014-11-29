/*
 * globals.h: Defines global variables for the program.
 *
 * Author: Harry Q Bovik <hqbovik@andrew.cmu.edu>
 * Modified by: 
 * Marc-Daniel Julien <mjulien@andrew.cmu.edu>
 * Jeffery He <jmhe@andrew.cmu.edu>
 * Date:   Tue, 23 Oct 2007 11:20:33 -0400
 */

#ifndef GLOBALS_H
#define GLOBALS_H

extern int user_setup_stack_ptr;
extern volatile unsigned long global_time;
extern int r8_address;
extern int cur_kstack;
extern int cur_ustack;
#endif /* GLOBALS_H */
