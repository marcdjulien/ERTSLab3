/** @file mtest.c
 *
 * @brief Creates two simple periodic tasks. and tests mutexes
 *
 * @author Marc-Daniel Julien <mjulien@andrew.cmu.edu>
 * @date   2008-11-30
 */
#include <stdio.h>
#include <task.h>
#include <unistd.h>

int m0, m1;

void panic(const char* str)
{
	puts(str);
	while(1);
}

void fun1(void* str)
{
	while(1)
	{	
		mutex_lock(m0);
		puts("mutex m0 locked in t1");
		sleep(2500);
		mutex_lock(m1);
		puts("mutex m1 locked in t1");
		putchar((int)str);
		mutex_unlock(m1);
		puts("mutex m1 unlocked in t1");
		mutex_unlock(m0);
		puts("mutex m0 unlocked in t1");
		if (event_wait(0) < 0)
			panic("Dev 0 failed");
		
	}
}

void fun2(void* str)
{
	while(1)
	{
		mutex_lock(m1);
		puts("mutex m1 locked in t2");
		sleep(2500);
		mutex_lock(m0);
		puts("mutex m0 locked in t2");
		putchar((int)str);
		mutex_unlock(m0);
		puts("mutex m0 unlocked in t2");
		mutex_unlock(m1);
		puts("mutex m1 unlocked in t2");
		if (event_wait(1) < 0)
			panic("Dev 1 failed");
		
	}
}

int main(int argc, char** argv)
{
	m0 = mutex_create();
	if(m0 < 0) 
		printf("Mutex error");

	m1 = mutex_create();
	if(m1 < 0) 
		printf("Mutex error");

	task_t tasks[2];
	tasks[0].lambda = fun1;
	tasks[0].data = (void*)'@';
	tasks[0].stack_pos = (void*)0xa2000000;
	tasks[0].C = 1;
	tasks[0].T = PERIOD_DEV0;

	tasks[1].lambda = fun2;
	tasks[1].data = (void*)'<';
	tasks[1].stack_pos = (void*)0xa1000000;
	tasks[1].C = 1;
	tasks[1].T = PERIOD_DEV1;

	task_create(tasks, 2);
	argc=argc; /* remove compiler warning */
	argv[0]=argv[0]; /* remove compiler warning */

	puts("Why did your code get here!\n");
	return 0;
}
