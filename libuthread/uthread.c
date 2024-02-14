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

#include <ucontext.h>

// stack size for storing threads info
#define STACK_SIZE 1024*64 // Define a suitable stack size

// global queue for scheduling
queue_t readyQ;
// TODO: running, ready, blocked, done queues

//global var which counts the number of running threads 
// used for while loop in run function
int active_thread_count = 0;

static struct uthread_tcb *current_thread_tcb = NULL;

typedef enum {
	//states of threads
	UTHREAD_RUNNING,
	UTHREAD_READY,
    UTHREAD_BLOCKED,
	UTHREAD_FINISHED,
}uthread_state;

struct uthread_tcb {
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
	return current_thread_tcb;
}


/*
 * uthread_yield - Yield execution
 *
 * This function is to be called from the currently active and running thread in
 * order to yield for other threads to execute.
 */
void uthread_yield(void)
{
	// save current thread context to ensure we can resume it later
	// need to first get current thread (function above)
	struct uthread_tcb *current_thread = uthread_current();
    // select the next thread to run
    struct uthread_tcb *next_thread;

    int dequeue_res = queue_dequeue(readyQ, (void **)&next_thread);
    if (dequeue_res == -1)
        return;

    // curr thread is ready -- enqueue and change state
    int enqueue_res = queue_enqueue(readyQ, current_thread);
    if (enqueue_res == -1)
        return;
	current_thread->state = UTHREAD_READY;

    // Switch context to the next thread
    uthread_ctx_switch(&current_thread->context, &next_thread->context);

    // After returning from context switch, update the global current thread and its state
    current_thread_tcb = next_thread;
    next_thread->state = UTHREAD_RUNNING;
}

void uthread_exit(void)
{
    struct uthread_tcb *current_thread = uthread_current();

    current_thread->state = UTHREAD_FINISHED;

    /*
     * TODO: Figure out, when should this cleanup below happen?
     * Before switch or after switch?
     * If the latter, when do we cleanup? Should we cleanup in the idle thread?
     */
    // curr thread stuff no longer needed
    uthread_ctx_destroy_stack(current_thread->stack);
    free(current_thread);


    // Select the next thread to run
    struct uthread_tcb *next_thread = NULL;
    int dequeue_res = queue_dequeue(readyQ, (void **)&next_thread);
    if (dequeue_res == -1) {
        perror("Dequeue failed, No other threads to run. Where is idle thread?\n");
        exit(-1);
    }

    current_thread_tcb = next_thread;
    next_thread->state = UTHREAD_RUNNING;

    // No need to save the current context since we're not coming back
    setcontext(&next_thread->context);
}

int uthread_create(uthread_func_t func, void *arg)
{
	//create new tcb
	struct uthread_tcb *tcb = malloc(sizeof(struct uthread_tcb));
	if (tcb == NULL){
		perror("failed new tcb");
		return -1;
	}

	// Allocate stack mem
	tcb->stack = uthread_ctx_alloc_stack();
    // This calls malloc so we need error mngmnt
    if (tcb->stack == NULL) {
        free(tcb);
        return -1;
    }

	// need to change stack size 
	void *top_of_stack = tcb->stack + STACK_SIZE;

	//initialize context
	int init_thread_val = uthread_ctx_init(&tcb->context, top_of_stack, func, arg);
	if (init_thread_val != 0) {
        uthread_ctx_destroy_stack(top_of_stack);
        free(tcb);
        perror("failed to init thread context");
        return -1;
    }

	// If init successful, change thread to ready
	tcb->state = UTHREAD_READY;

	//add to scheduler queue 
	// not sure if i should do this now or after 
	// we dont have a queue anywhere, do we start queue in the run function 
	//i stopped here to create uthread_run which has the queue in it(qSchd) 
	if (queue_enqueue(readyQ, tcb) < 0) {
        free(tcb);
        uthread_ctx_destroy_stack(top_of_stack);
		perror("failed to add thread to queue");
        return -1;
	}

	//add to active_thread_count to show we have a new active thread
	active_thread_count++;
	current_thread_tcb = tcb;

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
	// initialize ready threads queue
	readyQ = queue_create();
	if (readyQ == NULL){
		perror("failed to create queue");
        return -1;
	}

	//create the first thread 
	int threadVal = uthread_create(func, arg);
	if(threadVal == -1) {
		perror("cant create idle thread");
		return -1;
	}else {
		printf("finished uthread create\n");
	}

	// enable preemptive scheduling
	// if(preempt){ /* do something ... */ }

	fprintf(stderr, "thread count: %d\n", active_thread_count);

	//while loop with original thread which runs as idle thread 
	while(true) {
        // break if only idle thread is left
        if (queue_length(readyQ) == 1)
            break;

		//yield execution to next ready thread
		uthread_yield();
	}

    queue_destroy(readyQ);
    // TODO: Uncomment below? Probably?
//    free(tcb);
//    free(current_thread_tcb);
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

