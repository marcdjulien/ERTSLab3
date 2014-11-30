/** @file sched.c
 * 
 * @brief Top level implementation of the scheduler.
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-11-20
 */

#include <types.h>
#include <assert.h>

#include <kernel.h>
#include <config.h>
#include "sched_i.h"

#include <arm/reg.h>
#include <arm/psr.h>
#include <arm/exception.h>
#include <arm/physmem.h>
#include <exports.h>

tcb_t system_tcb[OS_MAX_TASKS]; /*allocate memory for system TCBs */

/* Marc: Unused */
void sched_init(task_t* main_task  __attribute__((unused)))
{
	
}

/**
 * @brief This is the idle task that the system runs when no other task is runnable
 */
 
static void __attribute__((unused)) idle(void)
{
	enable_interrupts();
	while(1);
}

/**
 * @brief Allocate user-stacks and initializes the kernel contexts of the
 * given threads.
 *
 * This function assumes that:
 * - num_tasks < number of tasks allowed on the system.
 * - the tasks have already been deemed schedulable and have been appropriately
 *   scheduled.  In particular, this means that the task list is sorted in order
 *   of priority -- higher priority tasks come first.
 *
 * @param tasks  A list of scheduled task descriptors.
 * @param size   The number of tasks is the list.
 */
void allocate_tasks(task_t** tasks  __attribute__((unused)), size_t num_tasks  __attribute__((unused)))
{
	/* Initialize run queue */
	runqueue_init();

	/* Add tasks to queue */
	size_t i;
	for(i = 0; i < num_tasks; i++)
	{
		/* Get TCB Struct*/
		tcb_t *cur_tcb = (tcb_t *)(&(system_tcb[i]));
		/* Get Task Struct */
		task_t *cur_task = tasks[i];
		
		/* Set priorities (already in order) */
		cur_tcb->native_prio = i; 
		cur_tcb->cur_prio = i;    
		
		/* Initialize r4,r5,r6 for launch_task */
		cur_tcb->context.r4 = (uint32_t)cur_task->lambda;
		cur_tcb->context.r5 = (uint32_t)cur_task->data;
		cur_tcb->context.r6 = (uint32_t)cur_task->stack_pos;
		/* Clear other regs */
		cur_tcb->context.r7 = 0;
		cur_tcb->context.r8 = 0;
		cur_tcb->context.r9 = 0;
		cur_tcb->context.r10 = 0;
		cur_tcb->context.r11 = 0;
		/* Should run launch_task initially */
		cur_tcb->context.lr = (void *)launch_task;
		cur_tcb->context.sp = cur_tcb->kstack_high;

		cur_tcb->holds_lock = 0;
		cur_tcb->sleep_queue = NULL;

		/* Set task to be runnable by adding to run queue */
		runqueue_add(cur_tcb, cur_tcb->cur_prio);

	}

	/* Create and add idle task */
	tcb_t *idle_tcb = (tcb_t *)(&(system_tcb[num_tasks]));
	idle_tcb->context.r4 = (uint32_t)idle;
	idle_tcb->context.lr = (void *)launch_task;
	idle_tcb->context.sp = (void *)idle_tcb->kstack_high;
	idle_tcb->native_prio = IDLE_PRIO;
	idle_tcb->cur_prio = IDLE_PRIO;
	runqueue_add(idle_tcb, idle_tcb->cur_prio);

}

