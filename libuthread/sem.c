#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	/* TODO Phase 3 */
	int count;
	queue_t semQueue;
}semaphore;

sem_t sem_create(size_t count)
{
	//mem for sem
	sem_t sem = malloc(sizeof(struct semaphore));
	if(sem == NULL){
		perror("no mem for semaphore");
		return NULL;
	}
	//set count
	sem->count = count;
	//make thread queue
	sem->semQueue = queue_create();
	if (sem->semQueue == NULL){
		perror("sem queue not created");
		return NULL;
	}

	return sem;

}


int sem_destroy(sem_t sem)
{
	// check if null 
	if(sem == NULL){
		return -1;
	}

	// not more threads
	if(queue_length(sem->semQueue) > 0){
		perror("still waiting threads; unsafe to destroy sem");
		return -1;
	}
	//cleanup
	queue_destroy(sem->semQueue);
	free(sem);

	return 0;

}

int sem_down(sem_t sem)
{
	/* TODO Phase 3 */
	if(sem == NULL){
		return -1;
	}
	
	//free
	if(sem->count > 0){
		//make it locked
		sem->count--;
	}else{
		struct uthread_tcb *curr_thread = uthread_current();
		int enqueue_res = queue_enqueue(sem->semQueue, curr_thread);
		if (enqueue_res != 0) {
			perror("Enqueue failed\n");
			return -1;
    	}
		uthread_block();
	}

	return 0;

}

int sem_up(sem_t sem)
{
	/* TODO Phase 3 */
	if(sem == NULL){
		return -1;
	}
	//release sem
	
	// Enter critical section (lock)
    if (queue_length(sem->semQueue) > 0) {
        // Dequeue and unblock the next waiting thread
        struct uthread_tcb *wait_thread;
        int dequeue_res = queue_dequeue(sem->semQueue, (void **)&wait_thread);
        if (dequeue_res == 0) {
            uthread_unblock(wait_thread);
            // Do not increment sem->count here, as the unblocked thread will consume the semaphore
        } else {
            perror("Queue dequeue failed\n");
            // Exit critical section (unlock)
            return -1;
        }
    } else {
        sem->count++; // Only increment if no threads were waiting
    }

	return 0;
}

