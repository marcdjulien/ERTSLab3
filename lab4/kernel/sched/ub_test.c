/** @file ub_test.c
 * 
 * @brief The UB Test for basic schedulability
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-11-20
 */

//#define DEBUG 0

#include <sched.h>
//#ifdef DEBUG
#include <exports.h>
//#endif

void task_swap(task_t **tasks, int a, int b)
{
    task_t *tmp = tasks[a];
    tasks[a] = tasks[b];
    tasks[b] = tmp;
}

void task_sort(task_t **tasks, size_t n)
{
    size_t i,j;
    for(i = 0; i < n; i++)
        for(j = i+1; j < n; j++)
        {
            if(tasks[i]->T > tasks[j]->T)
                task_swap(tasks, i, j);
        }

    for(i = 0; i < n; i++){
    	printf("task priority %d has period %lu",(int)i,tasks[i]->T);
    }
}

int ubTest(task_t *tasks, size_t numOfTasks)
{
    float taskUtilization = 0.0;
    float maxUtilization = 0.0;

    switch(numOfTasks){
        case(0):
            return TRUE;
        case(1):
            maxUtilization = 1.0;
        case(2):
            maxUtilization = 0.828;
        case(3):
            maxUtilization = 0.779;
        case(4):
            maxUtilization = 0.756;
        case(5):
            maxUtilization = 0.743;
        case(6):
            maxUtilization = 0.734;
        case(7):
            maxUtilization = 0.728;
        case(8):
            maxUtilization = 0.724;
        case(9):
            maxUtilization = 0.720;
        default:
            maxUtilization = 0.693;
    }
    unsigned int i;
    for(i = 0; i < numOfTasks; i++){
        taskUtilization += tasks[i].C/tasks[i].T;
    }

    return (taskUtilization <= maxUtilization);

}
 
/**
 * @brief Perform UB Test and reorder the task list.
 *
 * The task list at the end of this method will be sorted in order is priority
 * -- from highest priority (lowest priority number) to lowest priority
 * (highest priority number).
 *
 * @param tasks  An array of task pointers containing the task set to schedule.
 * @param num_tasks  The number of tasks in the array.
 *
 * @return 0  The test failed.
 * @return 1  Test succeeded.  The tasks are now in order.
 */
int assign_schedule(task_t** tasks  __attribute__((unused)), size_t num_tasks  __attribute__((unused)))
{

	size_t i;
	for(i = 0; i < num_tasks; i++)
	{

        /*Todo: check if tasks are schedulable (UB test)*/
        //printf("Something\n");
	}

    /* Sort tasks by priority - RMA */
    task_sort(tasks, num_tasks);

    return 1;

}
	


