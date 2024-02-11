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


typedef enum {
	//states of threads
	UTHREAD_RUNNIG,
	UTHREAD_READY,
    UTHREAD_BLOCKED,
	UTHREAD_FINISHED,
}uthread_state;

struct uthread_tcb {
	/* TODO Phase 2 */ 
	
	// define TCB block:
	//to save exec content (backup of CPU register )
	ucontext_t context;
	//threads stack pointer 
	void* stack;
	//current state of thread
	uthread_state state;
	// thread function 
	uthread_func_t func;
	//thread argument
	void* arg;

};

struct uthread_tcb *uthread_current(void)
{
	/* TODO Phase 2/3 */
}

void uthread_yield(void)
{
	/* TODO Phase 2 */
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
	//allocate mem for thread
	struct uthread_tcb *new_TCB = malloc(sizeof(struct uthread_tcb));
	// error check (ai coding tool auto completed for me)
	if(new_TCB == NULL){
        return -1;
    }
	//initialize new thread context
	if( getcontext(&new_TCB->context) == -1){
		free(new_TCB);
        return -1;
	}

	//allocate stack
	new_TCB->stack = malloc(STACK_SIZE);
	// error check (ai coding tool auto completed for me)
	if(new_TCB->stack == NULL){
		free(new_TCB);
        return -1;
    }

	//intialize new thread context
	new_TCB->arg = arg;
	new_TCB->func = func;


	//new thread 
	configure_stack(&new_TCB->context, new_TCB->stack, STACK_SIZE);

	//emtry point for threads, gets func and args and executes func with args so thread does proper function 
	makecontext(&new_TCB->context, (void (*)(void))thread_start_wrapper, 1 , new_TCB);


}

void thread_start_wrapper(void){
	struct uthread_tcb *curr = uthread_current();
	uthread_func_t actual_func = curr->func;
	void *arg = curr->arg;

	actual_func(arg);

	//exit?
	//uthread_exit();
}

void configure_stack(ucontext_t *c, void *stack, size_t stack_size){
	//initialize new stack 
	// go some help online about using methods related to this: https://www.ibm.com/docs/en/zos/2.2.0?topic=functions-makecontext-modify-user-context , https://stackoverflow.com/questions/35047816/ucontext-h-and-uc-link-not-returning-from-main-thread
	//uc_link is what should be resumed, nothing to resume as we creating thread 
	c->uc_link = NULL;
	//pointer to new stack, ss_sp is the stack pointer linked to the new TCB stack 
	c->uc_stack.ss_sp = stack;
	//set stack size 
	c->uc_stack.ss_size = stack_size;
    //set stack flags to zero (a little confused about this): https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-swapcontext-save-restore-user-context
	c->uc_stack.ss_flags = 0;

}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	/* TODO Phase 2 */
}

void uthread_block(void)
{
	/* TODO Phase 3 */
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	/* TODO Phase 3 */
}

