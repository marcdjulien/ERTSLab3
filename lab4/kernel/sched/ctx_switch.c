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
void dispatch_init(tcb_t* idle __attribute__((unused)))
{
	printf("Dispatch init ...\n");
	cur_tcb = idle;
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
	//printf("Dispatch save4 ...\n");
	tcb_t *dest, *temp;
	temp = cur_tcb;
	uint8_t hp = highest_prio();
	if(hp == IDLE_PRIO)
		dest = cur_tcb;
	else
		dest = runqueue_remove(hp);

	cur_tcb = dest;
	cur_kstack = (int)dest->kstack_high;
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
	//printf("Dispatch nosave ...\n");
	/*
	   This executes the first task
	   This ends up being the high prio task
	 */
	cur_tcb = runqueue_remove(highest_prio());
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
	//printf("Dispatch sleep ...\n");
	tcb_t *dest, *temp;
	temp = cur_tcb;
	uint8_t cp = get_cur_prio(); /* of task that was just put to sleep */
	uint8_t hp = highest_prio();

	if(cp == hp)
		dest = runqueue_remove(IDLE_PRIO);
	else
		dest = runqueue_remove(hp);
	/* Todo: save temp/current task */
	cur_tcb = dest;
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
