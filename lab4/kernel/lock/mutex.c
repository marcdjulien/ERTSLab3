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
#include <config.h>
#include <task.h>
#include <sched.h>
#include <bits/errno.h>
#include <arm/psr.h>
#include <arm/exception.h>
//#ifdef DEBUG_MUTEX
#include <exports.h> // temp
//#endif

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

	mutex_t *m = &(gtMutex[mutex-1]);
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
		if(m->pSleep_queue == NULL) /* First on queue */
			m->pSleep_queue = cur_tcb;
		else /* Traverse queue until last tcb */
		{
			tcb_t *temp = m->pSleep_queue;
			while(temp->sleep_queue != NULL)
				temp = temp->sleep_queue;
			/* Found last tcb */
			temp->sleep_queue = cur_tcb;
		}

		dispatch_sleep();
	}
	m->bLock = TRUE;
	cur_tcb->holds_lock++;
	m->pHolding_Tcb = cur_tcb;
	
	if(cur_tcb->cur_prio != HIGHEST_PRIO)
	{
		cur_tcb->cur_prio = HIGHEST_PRIO;
		runqueue_add(cur_tcb, cur_tcb->cur_prio);
		dispatch_save();
	}

	return 0;
}

int mutex_unlock_handler(int mutex  __attribute__((unused)))
{
	/* Invalid number */
	if((mutex >= OS_NUM_MUTEX) || (mutex <= 0))
		return -EINVAL;

	mutex_t *m = &(gtMutex[mutex-1]);
	tcb_t *cur_tcb = get_cur_tcb();

	/* Mutex not created */
	if(!m->bAvailable)
		return -EINVAL;

	/* Current task is not holding the mutex*/
	if(m->pHolding_Tcb != cur_tcb)
		return -EPERM;

	if(m->pSleep_queue == NULL){
		m->bLock = 0;
		m->pHolding_Tcb = NULL;	
	}
	else
	{
		/* Wake up waiting task */
		runqueue_add(m->pSleep_queue, m->pSleep_queue->cur_prio);
		/* Remove from sleep queue by setting it to next tcb */
		tcb_t *temp = m->pSleep_queue->sleep_queue;
		m->pSleep_queue->sleep_queue = NULL;
		m->pSleep_queue->holds_lock--;
		m->pSleep_queue = temp;
		
	}

	cur_tcb->holds_lock--;
	if(cur_tcb->holds_lock == 0)
	{
		cur_tcb->cur_prio = cur_tcb->native_prio;
		runqueue_add(cur_tcb, cur_tcb->cur_prio);
		dispatch_save();
	}
	return 0;
}

