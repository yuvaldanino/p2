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

typedef struct uthread_tcb {
	// define TCB block:
	//to save exec content (backup of CPU register )
	uthread_ctx_t context;
	//threads stack pointer 
	void* stack;
} uthread_tcb;

// global ptrs for thread switching
uthread_tcb *main_tcb,
            *current_thread_tcb,
            *previous_thread_tcb;

/*
 * Getter used in semaphore API
 */
uthread_tcb* uthread_current()
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

    // Perform the context switch
    uthread_ctx_switch(&previous_thread_tcb->context, &current_thread_tcb->context);
}

// Same as yield(), but the old process is done, not ready, so it goes in a different queue
void uthread_exit(void)
{
    // save current thread context to ensure we can resume it later
    // assuming current_thread was properly initialized:
    previous_thread_tcb = current_thread_tcb;

    // New thread will transition from ready->running
    int dequeue_res = queue_dequeue(readyQ, (void **)&current_thread_tcb);
    if (dequeue_res != 0) {
        perror("Yield: Queue is empty, no thread to switch to\n");
        return;
    }

    // Old thread will transition from running->finished
    int enqueue_res = queue_enqueue(finishedQ, previous_thread_tcb);
    if (enqueue_res != 0) {
        perror("Enqueue failed\n");
        return;
    }

    // Perform the context switch
    // No need to save the current context since we're not coming back
    setcontext(&current_thread_tcb->context);
}

/*
 * Create, initialize, and enqueue a thread
 */
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

	// initialize context
	int init_thread_val = uthread_ctx_init(&tcb->context, tcb->stack, func, arg);
	if (init_thread_val != 0) {
        uthread_ctx_destroy_stack(tcb->stack);
        free(tcb);
        perror("failed to init thread context");
        return -1;
    }

	// Schedule to ready queue
	int enqueue_res = queue_enqueue(readyQ, tcb);
    if (enqueue_res == -1) {
        free(tcb);
        uthread_ctx_destroy_stack(tcb->stack);
		perror("failed to add thread to queue");
        return -1;
	}

	return 0;
}

// Helper func to destroy finishedQ and readyQ
void cleanup()
{
    // Deallocate all the parts of the finishedQ (readyQ should be empty already)
    struct uthread_tcb *finished_thread = NULL;
    while(queue_length(finishedQ) > 0) {
        int dequeue_res = queue_dequeue(finishedQ, (void **)&finished_thread);
        if (dequeue_res != 0) {
            perror("Dequeue failed");
            return;
        }
        uthread_ctx_destroy_stack(finished_thread->stack);
        free(finished_thread);
    }
    // Destroy all queues
    queue_destroy(readyQ);
    queue_destroy(finishedQ);

    uthread_ctx_destroy_stack(main_tcb->stack);
    free(main_tcb);

    return;
}

/*
 * Create main/idle thread, 1st thread, then yield until threads complete
 */
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
    if (threadVal != 0) {
        perror("cant create idle thread");
        return -1;
    }
    // Must dequeue main thread to avoid an en/dequeue loop with only the main thread
    int dequeue_res = queue_dequeue(readyQ, (void **)&main_tcb);
    if (dequeue_res != 0) {
        perror("Dequeue failed]n");
        return -1;
    }

    current_thread_tcb = main_tcb;
    previous_thread_tcb = current_thread_tcb;

	// create the first thread
	threadVal = uthread_create(func, arg);
	if (threadVal != 0) {
		perror("cant create new thread");
		return -1;
	}

	// enable preemptive scheduling
    preempt_start(preempt);

	//while loop with original thread which runs as idle thread 
	while(true) {
        // break if only idle thread is left
        if (queue_length(readyQ) <= 0)
            break;

        // otherwise, yield to first ready thread
        uthread_yield();
	}

    cleanup();

	return 0;
}

// For semaphore api
void uthread_block(void)
{
    // save current thread context to ensure we can resume it later
    // assuming current_thread was properly initialized:
    previous_thread_tcb = current_thread_tcb;

    // New thread will transition from ready->blocked
    int dequeue_res = queue_dequeue(readyQ, (void **)&current_thread_tcb);
    if (dequeue_res == -1) {
        perror("Yield: Queue is empty, no thread to switch to\n");
        return;
    }

    // Perform the context switch
    uthread_ctx_switch(&previous_thread_tcb->context, &current_thread_tcb->context);
}

// For semaphore api
void uthread_unblock(struct uthread_tcb *uthread)
{
    // Old thread will transition from running->ready
    int enqueue_res = queue_enqueue(readyQ, uthread);
    if (enqueue_res != 0) {
        perror("Enqueue failed\n");
        return;
    }
}

