/** @file main.c
 *
 * @brief kernel main
 *
 * @author 
 *	   
 *	   
 * @date   
 */
 
#include <kernel.h>
#include <task.h>
#include <sched.h>
#include <device.h>
#include <assert.h>
#include <types.h>

/* Added by Marc */
#include <exports.h>
#include <arm/psr.h>
#include <arm/exception.h>
#include <arm/interrupt.h>
#include <arm/timer.h>
#include <arm/reg.h>
#include <bits/errno.h>
#include <bits/fileno.h>
#include <bits/swi.h>
#include "globals.h"
#include "swi_handler.h"
#include "irq_handler.h"
#include "user_setup.h"
#include "exit_handler.h"

/* Changed to bool_e in lab4 starter code*/
/*typedef enum {FALSE, TRUE} bool;*/

/* Returns 1 if bit number i in val is set */
#define bit_isset(val, i) (!(((val) & (1<<(i))) == 0))

#define SWI_VECT_ADDR 0x08
#define IRQ_VECT_ADDR 0x18
#define PC_OFFSET 0x08
#define INTERVAL 32500
#define OSTMR_0_BIT 0x04000000
/**Cannot write to this address. kernel.bin loaded here. Stack grows down.*/
#define USER_STACK_TOP 0xa3000000
/* (LDR pc, [pc, 0x000]) 0xe59ff000 --> 0x000 through masking*/
#define LDR_PC_PC_INSTR 0xe59ff000
#define LDR_SIGN_MASK 0x00800000
#define BAD_CODE 0x0badc0de
#define SFROM_START 0x00000000
#define SFROM_END 0x00ffffff
#define SDRAM_START 0xa0000000
#define SDRAM_END 0xa3ffffff

uint32_t global_data;


/* Checks the SWI Vector Table. */
bool_e check_exception_vector(int vector_address) 
{
    int vector_instr = *((int *)vector_address);

    // Check if the offset is negative.
    if ((vector_instr & LDR_SIGN_MASK) == 0) {
        return FALSE;
    }

    // Check that the instruction is a (LDR pc, [pc, 0x000])
    if ((vector_instr & 0xFFFFF000) != LDR_PC_PC_INSTR) {
        return FALSE;
    }

    return TRUE;
}

void wire_handler(int vector_address, void *new_handler, int *oldinstr_1, int *oldinstr_2)
{
    // Jump offset already incorporates PC offset. Usually 0x10 or 0x14.
    int jmp_offset = (*((int *) vector_address))&(0xFFF);

    // &S_Handler" in Jump Table.
    int *handler_addr = *(int **)(vector_address + PC_OFFSET + jmp_offset);

    // Save original Uboot handler instructions.
    *oldinstr_1 = *handler_addr;
    *oldinstr_2 = *(handler_addr + 1);

    // Wire in our own: LDR pc, [pc, #-4] = 0xe51ff004
    *handler_addr = 0xe51ff004;
    *(handler_addr + 1) = (int)new_handler; // New  handler.

    return;

}

void restore_handlers(int vector_address, int instr_1, int instr_2)
{
    // Jump offset already incorporates PC offset. Usually 0x10 or 0x14.
    int jmp_offset = (*((int *) vector_address))&(0xFFF);

    // &Handler in Jump Table.
    int *handler_addr = *(int **)(vector_address + PC_OFFSET + jmp_offset);

    *handler_addr = instr_1;
    *(handler_addr + 1) = instr_2;

    return;
}


/* Verifies that the buffer is entirely in valid memory. */
int check_mem(char *buf, int count, unsigned start, unsigned end) {
    unsigned start_buf = (unsigned) buf;
    unsigned end_buf = (unsigned)(buf + count);

    if ( (start_buf < start) || (start_buf > end) ) {
        return FALSE;
    }
    if ( (end_buf < start) || (end_buf > end) ) {
        return FALSE;
    }
    // Overflow case.
    if ( start_buf >= end_buf ) {
        return FALSE;
    }

    return TRUE;
}

// write function to replace the system's write function
ssize_t write_handler(int fd, const void *buf, size_t count) {

    // Check for invalid memory range or file descriptors
    if (check_mem((char *) buf, (int) count, SDRAM_START, SDRAM_END) == FALSE &&
        check_mem((char *) buf, (int) count, SFROM_START, SFROM_END) == FALSE) {
        exit_handler(-EFAULT);
    } else if (fd != STDOUT_FILENO) {
        exit_handler(-EBADF);
    }

    char *buffer = (char *) buf;
    size_t i;
    char read_char;
    for (i = 0; i < count; i++) {
        // put character into buffer and putc
        read_char = buffer[i];
        putc(read_char);
    }
    return i;
}


// read function to replace the system's read function
ssize_t read_handler(int fd, void *buf, size_t count) {
    // Check for invalid memory range or file descriptors
    if (check_mem((char *) buf, (int) count, SDRAM_START, SDRAM_END) == FALSE) {
        exit_handler(-EFAULT);
    } else if (fd != STDIN_FILENO) {
        exit_handler(-EBADF);
    }

    size_t i = 0;
    char *buffer = (char *) buf;
    char read_char;

    while (i < count) {
        read_char = getc();

        if (read_char == 4) { //EOT character
            return i;
        } else if (((read_char == 8) || (read_char == 127))) { // backspace or DEL character
            buffer[i] = 0; // '\0' character
            if(i > 0) {
                i--;
                puts("\b \b");
            }
        } else if ((read_char == 10) || (read_char == 13)) { // '\n' newline or '\r' carriage return character
            buffer[i] = '\n';
            putc('\n');
            return (i+1);
        } else {
            // put character into buffer and putc
            buffer[i] = read_char;
            i++;
            putc(read_char);
        }
    }

    return i;
}

/* Returns the time in milliseconds that have elapsed since the kernel booted up */
unsigned long time_handler()
{
    return global_time;
}

/* Suspends the excution of the current task for a given time */
void sleep_handler(unsigned long millisDelay)
{   
    unsigned long currentTime = global_time;
    while((currentTime + millisDelay) >= global_time);    
}

int task_create_handler(task_t *tasks, size_t n)
{
	tasks = tasks;
	n = n;
	printf("Creating %d tasks\n", (int)n);
	return 0;
}

/* Called when OSCR matches OSMR_0 (ie, every 10ms) */
void C_Timer_0_Handler()
{

    /* Set next interval time*/
    reg_write(OSTMR_OSMR_ADDR(0), reg_read(OSTMR_OSCR_ADDR)+INTERVAL); 

    /* Acknowledge match */
    reg_set(OSTMR_OSSR_ADDR, OSTMR_OSSR_M0);
    
    /* Increment global time */
    global_time += 10;
}

/* C_SWI_Handler uses SWI number to call the appropriate function. */
int C_SWI_Handler(int swiNum, int *regs) 
{    
    switch (swiNum) 
    {       
        // ssize_t read(int fd, void *buf, size_t count);
        case READ_SWI:
            return read_handler(regs[0], (void *) regs[1], (size_t) regs[2]);
        
        // ssize_t write(int fd, const void *buf, size_t count);
        case WRITE_SWI:
            return write_handler((int) regs[0], (void *) regs[1], (size_t) regs[2]);
        
        // void exit(int status);
        /* No longer implemented
        case EXIT_SWI:
            exit_handler((int) regs[0]); // never returns
            return -1;
        */

        // void sleep(unsigned long delay);
        case SLEEP_SWI:
            sleep_handler((unsigned long)regs[0]);
            return 0;
        
        // unsigned long time();
        case TIME_SWI:
            return time_handler();
        
        case CREATE_SWI:
        	return task_create_handler((task_t *)regs[0], (size_t)regs[1]);
        
        default:
            printf("Error in ref C_SWI_Handler: Invalid SWI number.");
            exit_handler(BAD_CODE); // never returns
    }

    return -1;
}

/* C IRQ Handler, called when and IRQ is asserted */
void C_IRQ_Handler() 
{
    /* Read Pending Interrupts Register */
    uint32_t icpr = reg_read(INT_ICPR_ADDR);

    /* Check if the 26th (Interrupt Match 0) bit is set */
    if(bit_isset(icpr, INT_OSTMR_0))
        C_Timer_0_Handler();

}

int kmain(int argc __attribute__((unused)), char** argv  __attribute__((unused)), uint32_t table)
{

	app_startup();
	global_data = table;


    /** Wire in the new handlers. **/
    int old_irq_instr_1, old_irq_instr_2, old_swi_instr_1, old_swi_instr_2;
    if (check_exception_vector(SWI_VECT_ADDR) == FALSE)
        return BAD_CODE;
    wire_handler(SWI_VECT_ADDR, &swi_handler, &old_swi_instr_1, &old_swi_instr_2);
    
    if (check_exception_vector(IRQ_VECT_ADDR) == FALSE)
        return BAD_CODE;
    wire_handler(IRQ_VECT_ADDR, &irq_handler, &old_irq_instr_1, &old_irq_instr_2);

    // Copy argc and argv to user stack in the right order.
    int *spTop = ((int *) USER_STACK_TOP) - 1;
    int i = 0;
    for (i = argc-1; i >= 0; i--) {
        *spTop = (int)argv[i];
        spTop--;
    }
    *spTop = argc;

    /* Configure IRQ for timer */
    uint32_t old_iclr = reg_read(INT_ICLR_ADDR);
    reg_clear(INT_ICLR_ADDR, OSTMR_0_BIT);  /* Clear bit to generate irq's */
    reg_write(INT_ICMR_ADDR, OSTMR_0_BIT);    /* Set bit to enable irq's */

    /* Set up timer */    
    reg_write(OSTMR_OSMR_ADDR(0), reg_read(OSTMR_OSCR_ADDR)+INTERVAL); /* Set interval */
    reg_set(OSTMR_OIER_ADDR, OSTMR_OIER_E0); /* Enable match 0 */



    /** Jump to user program. **/
    global_time = 0;
    printf("Starting User Mode with user_stack = %p\n", spTop);
    user_setup(spTop);


    /* Leftover from Lab 3 -- this code should never execute */
    /** Restore SWI Handler. **/
    restore_handlers(SWI_VECT_ADDR, old_swi_instr_1, old_swi_instr_2);
    restore_handlers(IRQ_VECT_ADDR, old_irq_instr_1, old_irq_instr_2);
    reg_write(INT_ICLR_ADDR, old_iclr);
    printf("global_time final = %ld\n", global_time);
	assert(0);  
}
