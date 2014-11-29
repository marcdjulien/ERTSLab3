/**
 * @file mutex.c
 *
 * @brief Implements mutices.
 *
 * @author Harry Q Bovik < PUT YOUR NAMES HERE
 *
 * 
 * @date  
 */

//#define DEBUG_MUTEX

#include <lock.h>
#include <task.h>
#include <sched.h>
#include <bits/errno.h>
#include <arm/psr.h>
#include <arm/exception.h>
#ifdef DEBUG_MUTEX
#include <exports.h> // temp
#endif

mutex_t gtMutex[OS_NUM_MUTEX];
static int next_mutex;
void mutex_init()
{
	int i;
	for(i = 0; i < OS_NUM_MUTEX; i++)
	{
		gtMutex[i].bAvailable = FALSE;
		gtMutex[i].pHolding_Tcb = NULL;
		gtMutex[i].bLock = 0;
		gtMutex[i].pSleep_queue = NULL;
	}

	next_mutex = 0;
}

int mutex_create_handler(void)
{
	if(next_mutex >= 32)
		return -ENOMEM;
	else
	{
		gtMutex[next_mutex].bAvailable = TRUE;
		next_mutex++;
		return next_mutex;
	}
}

int mutex_lock_handler(int mutex  __attribute__((unused)))
{
	/* Invalid number */
	if((mutex >= OS_NUM_MUTEX) || (mutex <= 0))
		return -EINVAL;

	mutex_t *m = &(gtMutex[mutex]);
	tcb_t *cur_tcb = get_cur_tcb();
	/* Mutex not created */
	if(!m->bAvailable)
		return -EINVAL;

	/* Current task is holding the mutex already */
	if(m->pHolding_Tcb == cur_tcb)
		return -EDEADLOCK;

	/* Wait until acquired */
	if(m->bLock)
	{
		m->pSleep_queue = cur_tcb;
		dispatch_sleep();
	}
	cur_tcb->holds_lock = TRUE;
	return 0;
}

int mutex_unlock_handler(int mutex  __attribute__((unused)))
{
	/* Invalid number */
	if((mutex >= OS_NUM_MUTEX) || (mutex <= 0))
		return -EINVAL;

	mutex_t *m = &(gtMutex[mutex]);
	tcb_t *cur_tcb = get_cur_tcb();

	/* Mutex not created */
	if(!m->bAvailable)
		return -EINVAL;

	/* Current task is not holding the mutex*/
	if(m->pHolding_Tcb != cur_tcb)
		return -EPERM;

	if(m->pSleep_queue == NULL)
		m->bLock = 0;
	else
	{
		/* Wake up waiting task */
		runqueue_add(m->pSleep_queue, m->pSleep_queue->cur_prio);
		m->pSleep_queue = NULL;
	}

	cur_tcb->holds_lock = FALSE;
	return 0;
}

