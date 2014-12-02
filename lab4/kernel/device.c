/**
 * @file device.c
 *
 * @brief Implements simulated devices.
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-12-01
 */

#include <types.h>
#include <assert.h>

#include <task.h>
#include <sched.h>
#include <device.h>
#include <arm/reg.h>
#include <arm/psr.h>
#include <arm/exception.h>

/**
 * @brief Fake device maintainence structure.
 * Since our tasks are periodic, we can represent 
 * tasks with logical devices. 
 * These logical devices should be signalled periodically 
 * so that you can instantiate a new job every time period.
 * Devices are signaled by calling dev_update 
 * on every timer interrupt. In dev_update check if it is 
 * time to create a tasks new job. If so, make the task runnable.
 * There is a wait queue for every device which contains the tcbs of
 * all tasks waiting on the device event to occur.
 */

struct dev
{
	tcb_t* sleep_queue;
	unsigned long   next_match;
};
typedef struct dev dev_t;

/* devices will be periodically signaled at the following frequencies */
/* Todo: Change periods back !!!! */
const unsigned long dev_freq[NUM_DEVICES] = {100, 200, 500, 50};//{500, 750, 2000, 5000};//;
static dev_t devices[NUM_DEVICES];

/**
 * @brief Initialize the sleep queues and match values for all devices.
 */
void dev_init(void)
{
   int i;
   for(i = 0; i < NUM_DEVICES; i++)
	{
		devices[i].next_match = dev_freq[i];
		devices[i].sleep_queue = NULL;
	}
}


/**
 * @brief Puts a task to sleep on the sleep queue until the next
 * event is signalled for the device.
 *
 * @param dev  Device number.
 */
void dev_wait(unsigned int dev __attribute__((unused)))
{
	dev_t *device = &(devices[dev]);
	if(device->sleep_queue == NULL) /* First on Q */
		device->sleep_queue = get_cur_tcb();
	else /* Traverse until last one is found */
	{
		tcb_t *temp = device->sleep_queue;
		while(temp->sleep_queue != NULL)
			temp = temp->sleep_queue;
		/* Found last tcb */
		temp->sleep_queue = get_cur_tcb();
	}
	/* Put to sleep */
	dispatch_sleep();

}


/**
 * @brief Signals the occurrence of an event on all applicable devices. 
 * This function should be called on timer interrupts to determine that 
 * the interrupt corresponds to the event frequency of a device. If the 
 * interrupt corresponded to the interrupt frequency of a device, this 
 * function should ensure that the task is made ready to run 
 */
void dev_update(unsigned long millis __attribute__((unused)))
{
	disable_interrupts();
	int i;
	int call_dispatch = 0;
	for(i = 0; i < NUM_DEVICES; i++)
	{
		dev_t *device = &(devices[i]);
		/* Match detected */
		if(millis > device->next_match)
		{

			/* Add all tasks back to run queue */
			tcb_t *tcb = device->sleep_queue;
			tcb_t *temp;
			while(tcb != NULL)
			{
				call_dispatch |= 1;
				runqueue_add(tcb, tcb->cur_prio);
				temp = tcb->sleep_queue;
				tcb->sleep_queue = NULL;
				tcb = temp;
			}
			/* Clear sleep queue */
			device->sleep_queue = NULL;
			/* Update next match */
			device->next_match += dev_freq[i];
		}
	}

	if(call_dispatch)
		dispatch_save();

}

