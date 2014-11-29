/** @file ctx_switch.c
 * 
 * @brief C wrappers around assembly context switch routines.
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-11-21
 */
 

#include <types.h>
#include <assert.h>

#include <config.h>
#include <kernel.h>
#include "sched_i.h"

#ifdef DEBUG_MUTEX
#include <exports.h>
#endif

extern int cur_kstack;

static __attribute__((unused)) tcb_t* cur_tcb; /* use this if needed */

/**
 * @brief Initialize the current TCB and priority.
 *
 * Set the initialization thread's priority to IDLE so that anything
 * will preempt it when dispatching the first task.
 */
/* Marc: Unused */
void dispatch_init(tcb_t* idle __attribute__((unused)))
{
	
}


/**
 * @brief Context switch to the highest priority task while saving off the 
 * current task state.
 *
 * This function needs to be externally synchronized.
 * We could be switching from the idle task.  The priority searcher has been tuned
 * to return IDLE_PRIO for a completely empty run_queue case.
 */
void dispatch_save(void)
{
	tcb_t *dest, *temp;
	temp = cur_tcb;
	uint8_t hp = highest_prio();

	/* If the idle task is he only next high prio task
	   that means we are already running idle task */
	if(hp == IDLE_PRIO)
		dest = cur_tcb;
	else
		dest = runqueue_remove(hp);

	/* Set cur_tcb to the task that we are about to run */
	cur_tcb = dest;

	/* Set the cur_kstack var of the task that we are about to run */
	cur_kstack = (int)dest->kstack_high;
	
	/* Add the task that we just switched from back to the queue */
	runqueue_add(temp, temp->cur_prio);

	ctx_switch_full(&(dest->context), &(temp->context));
}

/**
 * @brief Context switch to the highest priority task that is not this task -- 
 * don't save the current task state.
 *
 * There is always an idle task to switch to.
 */
void dispatch_nosave(void)
{
	/*
	   This executes the first task
	   This ends up being the initial high prio task
	 */

	/* Set cur_tcb to the task that we are about to run */
	cur_tcb = runqueue_remove(highest_prio());

	/* Set the cur_kstack var of the task that we are about to run */
	cur_kstack = (int)cur_tcb->kstack_high;

	ctx_switch_half(&(cur_tcb->context));
}


/**
 * @brief Context switch to the highest priority task that is not this task -- 
 * and save the current task but don't mark is runnable.
 *
 * There is always an idle task to switch to.
 */
void dispatch_sleep(void)
{
	tcb_t *dest, *temp;
	temp = cur_tcb;
	uint8_t cp = get_cur_prio(); /* i.e task that was just put to sleep */
	uint8_t hp = highest_prio(); /* next task to run */

	/* Run idle task if there are no other */
	if(cp == hp)
		dest = runqueue_remove(IDLE_PRIO);
	else
		dest = runqueue_remove(hp);

	/* Set cur_tcb to the task that we are about to run */
	cur_tcb = dest;
	
	/* Set the cur_kstack var of the task that we are about to run */
	cur_kstack = (int)dest->kstack_high;
	
	ctx_switch_full(&(dest->context), &(temp->context));
}

/**
 * @brief Returns the priority value of the current task.
 */
uint8_t get_cur_prio(void)
{
	return cur_tcb->cur_prio; 
}

/**
 * @brief Returns the TCB of the current task.
 */
tcb_t* get_cur_tcb(void)
{
	return cur_tcb;
}
