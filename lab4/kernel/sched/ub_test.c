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
}

int ubTest(task_t *tasks, size_t numOfTasks)
{
    float taskUtilization = 0.0;
    float maxUtilization = 0.693;
    float maxUtilizationL[64];
    maxUtilizationL[1] =  1; maxUtilizationL[2] =  0.828427125; maxUtilizationL[3] =  0.77976315;
    maxUtilizationL[4] =  0.75682846; maxUtilizationL[5] =  0.743491775; maxUtilizationL[6] =  0.73477229;
    maxUtilizationL[7] =  0.728626596; maxUtilizationL[8] =  0.724061861; maxUtilizationL[9] =  0.72053765;
    maxUtilizationL[10] =  0.717734625; maxUtilizationL[11] =  0.715451984; maxUtilizationL[12] =  0.713557132;
    maxUtilizationL[13] =  0.711958994; maxUtilizationL[14] =  0.710592941; maxUtilizationL[15] =  0.709411842;
    maxUtilizationL[16] =  0.708380519; maxUtilizationL[17] =  0.707472181; maxUtilizationL[18] =  0.706666069;
    maxUtilizationL[19] =  0.705945844; maxUtilizationL[20] =  0.705298477; maxUtilizationL[21] =  0.704713443;
    maxUtilizationL[22] =  0.704182154; maxUtilizationL[23] =  0.703697529; maxUtilizationL[24] =  0.703253679;
    maxUtilizationL[25] =  0.702845666; maxUtilizationL[26] =  0.702469318; maxUtilizationL[27] =  0.702121087;
    maxUtilizationL[28] =  0.701797935; maxUtilizationL[29] =  0.701497247; maxUtilizationL[30] =  0.70121676;
    maxUtilizationL[31] =  0.700954504; maxUtilizationL[32] =  0.700708757; maxUtilizationL[33] =  0.700478008;
    maxUtilizationL[34] =  0.700260926; maxUtilizationL[35] =  0.70005633; maxUtilizationL[36] =  0.699863173;
    maxUtilizationL[37] =  0.699680523; maxUtilizationL[38] =  0.699507544; maxUtilizationL[39] =  0.699343489;
    maxUtilizationL[40] =  0.699187684; maxUtilizationL[41] =  0.699039522; maxUtilizationL[42] =  0.698898454;
    maxUtilizationL[43] =  0.698763983; maxUtilizationL[44] =  0.698635657; maxUtilizationL[45] =  0.698513063;
    maxUtilizationL[46] =  0.698395826; maxUtilizationL[47] =  0.698283602; maxUtilizationL[48] =  0.698176077;
    maxUtilizationL[49] =  0.698072961; maxUtilizationL[50] =  0.69797399; maxUtilizationL[51] =  0.697878916;
    maxUtilizationL[52] =  0.697787516; maxUtilizationL[53] =  0.69769958; maxUtilizationL[54] =  0.697614915;
    maxUtilizationL[55] =  0.697533342; maxUtilizationL[56] =  0.697454694; maxUtilizationL[57] =  0.697378816;
    maxUtilizationL[58] =  0.697305566; maxUtilizationL[59] =  0.697234808; maxUtilizationL[60] =  0.697166418;
    maxUtilizationL[61] =  0.697100279; maxUtilizationL[62] =  0.697036281; maxUtilizationL[63] =  0.696974323;

    if (numOfTasks == 0)
        return FALSE;
    else if(numOfTasks < 64)
        maxUtilization = maxUtilizationL[numOfTasks];
    //If numOfTasks is 64 or more, it will have default initialization of 0.69

    unsigned int i;
    for(i = 0; i < numOfTasks-1; i++){
        taskUtilization += tasks[i].C/tasks[i].T;
    }
    taskUtilization += (tasks[numOfTasks].C + tasks[numOfTasks].B)/tasks[numOfTasks].T;

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
    /* Sort tasks by priority - RMA */
    task_sort(tasks, num_tasks);

    int scheduleable;
    scheduleable = ubTest(tasks, num_tasks);

    printf("the task set is %d\n", scheduleable);

    return scheduleable;

}
	


