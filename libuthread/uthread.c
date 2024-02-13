#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

//so i can use ucontext
#include <ucontext.h>

//stack size for storing threads info 
#define STACK_SIZE 1024*64 // Define a suitable stack size

//global queue for scheudling 
queue_t qSched;

//global var which counts the number of running threads 
// used for while loop in run function
int active_thread_count = 0;


typedef enum {
	//states of threads
	UTHREAD_RUNNING,
	UTHREAD_READY,
    UTHREAD_BLOCKED,
	UTHREAD_FINISHED,
}uthread_state;

struct uthread_tcb {
	/* TODO Phase 2 */ 
	
	// define TCB block:
	//to save exec content (backup of CPU register )
	uthread_ctx_t context;
	//threads stack pointer 
	void* stack;
	//current state of thread
	uthread_state state;
	

};

struct uthread_tcb *uthread_current(void)
{
	/* TODO Phase 2/3 */
}


/*
 * uthread_yield - Yield execution
 *
 * This function is to be called from the currently active and running thread in
 * order to yield for other threads to execute.
 */
void uthread_yield(void)
{
	/* TODO Phase 2 */

	//save current thread context to ensure we can resume it later 
	//need to first get current thread (function above)
	struct uthread_tcb *current_thread = uthread_current();
	getcontext(&current_thread->context);

	//update the current thread state as its no longer running but ready to run 
	current_thread->state = UTHREAD_READY;

	//select the next thread to run 
	struct uthread_tcb *next_thread = NULL;

	int dequeVal = queue_dequeue(qSched, (void **)&next_thread);
	
	/*
	if(dequeVal != 0){
		//
		printf("error dequeuing thread\n");
	}
	*/

	//swicth to context of next thread 
	if(dequeVal == 0 && next_thread!= NULL){
		swapcontext(&current_thread->context, &next_thread->context);
	}


	//update scheduler 
	// add saved state to the queue if its ready 
	if (current_thread->state == UTHREAD_READY) {
        queue_enqueue(qSched, current_thread);
    }




}

void uthread_exit(void)
{
	/* TODO Phase 2 */
}

int uthread_create(uthread_func_t func, void *arg)
{
	/* TODO Phase 2 */
	// implement 1st 
	/*idea:
		- create new TCB
		- intialize the new thread content in TCB
		- allocaye mem for thread stack and connect to thread context 
		- set new thread context 
		- add thread to read queue as its ready for execution 
		- return:
			- 0 success 
			- -1 error 
	*/


	// not sure if i need to allocate mem 
	//create new tcb
	struct uthread_tcb *tcb = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
	if (tcb == NULL){
		perror("failed new tcb");
		return -1;
	}

	//allocate stack mem
	tcb->stack = uthread_ctx_alloc_stack();

	//initialize 
	//pointer to top of stack
	// need to change stack size 
	void *top_of_stack = tcb->stack + STACK_SIZE;

	//initialize context
	int init_thread_val = uthread_ctx_init(&tcb->context, top_of_stack, func, arg);
	if (init_thread_val!= 0){
        perror("failed to init thread context");
        return -1;
    }

	//HERE IF: we successfully initizalized thread
	//change thread to ready
	tcb->state = UTHREAD_READY;

	//add to scheduler queue 
	// not sure if i should do this now or after 
	// we dont have a queue anywhere, do we start queue in the run function 
	//i stopped here to create uthread_run which has the queue in it(qSchd) 
	if(queue_enqueue(qSched, tcb) < 0){
		perror("failed to add thread to queue");
        return -1;
	}
	//add to active_thread_coiunt to show we have a new active thread 
	active_thread_count++;

	//return 0 success 
	return 0;

}

/*
 * uthread_run - Run the multithreading library
 * @preempt: Preemption enable
 * @func: Function of the first thread to start
 * @arg: Argument to be passed to the first thread
 *
 * This function should only be called by the process' original execution
 * thread. It starts the multithreading scheduling library, and becomes the
 * "idle" thread. It returns once all the threads have finished running.
 *
 * If @preempt is `true`, then preemptive scheduling is enabled.
 *
 * Return: 0 in case of success, -1 in case of failure (e.g., memory allocation,
 * context creation).
 */
int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	/* TODO Phase 2 */
	//intialize the queue for the threads 
	qSched = queue_create();

	//create the first thread 
	int threadVal = uthread_create(func, arg);
	if(threadVal < 0){
		perror("cant create idle thread");
		return -1;
	}

	//enable preemptive scheduling 
	if(preempt){
		//do preempt sched
	}

	//while loop with original thread which runs as idle thread 
	while(active_thread_count > 0){

		//yield execution to next ready thread


	}
	
	//cleanup?


	return 0;




}

void uthread_block(void)
{
	/* TODO Phase 3 */
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	/* TODO Phase 3 */
}

