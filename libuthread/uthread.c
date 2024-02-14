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

// global queues for scheduling
queue_t readyQ;
queue_t finishedQ;

// global ptrs for thread switching
uthread_tcb *main_tcb,
            *current_thread_tcb,
            *previous_thread_tcb,
            = NULL, NULL, NULL;

typedef enum {
	//states of threads
	UTHREAD_RUNNING,
	UTHREAD_READY,
    UTHREAD_BLOCKED,
	UTHREAD_FINISHED,
} uthread_state;

struct uthread_tcb {
	// define TCB block:
	//to save exec content (backup of CPU register )
	uthread_ctx_t context;
	//threads stack pointer 
	void* stack;
	//current state of thread
	uthread_state state;
};

/*
 * uthread_yield - Yield execution
 *
 * This function is to be called from the currently active and running thread in
 * order to yield for other threads to execute.
 */
void uthread_yield(void)
{
	// save current thread context to ensure we can resume it later
	// assuming current_thread was properly initialized:
    previous_thread_tcb = current_thread_tcb;

    // New thread will transition from ready->running
    int dequeue_res = queue_dequeue(readyQ, (void **)&current_thread_tcb);
    if (dequeue_res == -1){
		perror("Yield: Queue is empty, no thread to switch to\n");
        return;
	}

    // Old thread will transition from running->ready
    int enqueue_res = queue_enqueue(readyQ, previous_thread_tcb);
    if (enqueue_res == -1) {
        perror("Enqueue failed");
        return;
    }

    // Update global thread states
    previous_thread_tcb->state = UTHREAD_READY;
    current_thread_tcb->state = UTHREAD_RUNNING;

    // Perform the context switch
    uthread_ctx_switch(&previous_thread_tcb->context, &current_thread_tcb->context);

    // Never come here
    assert(0);
}

// Same as yield(), but the old process is done, not ready, so it goes in a different queue
void uthread_exit(void)
{
    // save current thread context to ensure we can resume it later
    // assuming current_thread was properly initialized:
    previous_thread_tcb = current_thread_tcb;

    // New thread will transition from ready->running
    int dequeue_res = queue_dequeue(readyQ, (void **)&current_thread_tcb);
    if (dequeue_res == -1) {
        perror("Yield: Queue is empty, no thread to switch to\n");
        return;
    }

    // Old thread will transition from running->finished
    int enqueue_res = queue_enqueue(finishedQ, previous_thread_tcb);
    if (enqueue_res == -1) {
        perror("Enqueue failed");
        return;
    }

    // Update global thread states
    previous_thread_tcb->state = UTHREAD_FINISHED;
    current_thread_tcb->state = UTHREAD_RUNNING;

    // Perform the context switch
    // No need to save the current context since we're not coming back
    setcontext(&current_thread_tcb->context);

//    uthread_ctx_switch(&previous_thread_tcb->context, &current_thread_tcb->context);

    // Never come here
    assert(0);
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

	printf("Setting up thread context to point to function: %p\n", (void *)func);

	//initialize context
	int init_thread_val = uthread_ctx_init(&tcb->context, tcb->stack, func, arg);
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
	int enqueue_res = queue_enqueue(readyQ, tcb);
    if (enqueue_res == -1) {
        free(tcb);
        uthread_ctx_destroy_stack(top_of_stack);
		perror("failed to add thread to queue");
        return -1;
	}

	return 0;
}


int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	// initialize ready and finished threads queues
	readyQ = queue_create();
    finishedQ = queue_create();
	if (readyQ == NULL || finishedQ == NULL){
		perror("failed to create ready or finished queue");
        return -1;
	}

    // create the main thread
    int threadVal = uthread_create(NULL, NULL);
    // TODO: edit here

	// create the first thread
	int threadVal = uthread_create(func, arg);
	if (threadVal == -1) {
		perror("cant create idle thread");
		return -1;
	}

	// enable preemptive scheduling
	// if(preempt){ /* do something ... */ }

	fprintf(stderr, "thread count: %d\n", active_thread_count);

	//while loop with original thread which runs as idle thread 
	while(true) {
        //yield execution to next ready thread
        //uthread_yield();
        uthread_yield();

        // break if only idle thread is left
        if (queue_length(readyQ) == 1)
            break;
	}
	
	fprintf(stderr, "thread count: %d\n", active_thread_count);

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

