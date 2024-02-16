#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	//num resources 
	int count;
	//threads waiting for sem
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
	//set count to number of avilable resources 
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
	if(sem == NULL){
		return -1;
	}
	//free resource
	if(sem->count > 0){
		//lock the resource and exec
		sem->count--;
	}else{
		//when thread wants resource but cant access because already used 
		struct uthread_tcb *curr_thread = uthread_current();
		//add thread to waiting for sem queue
		int enqueue_res = queue_enqueue(sem->semQueue, curr_thread);
		if (enqueue_res != 0) {
			perror("Enqueue failed\n");
			return -1;
    	}
		//block the thread as it wait for resource
		uthread_block();
	}

	return 0;

}

int sem_up(sem_t sem)
{
	if(sem == NULL){
		return -1;
	}
	//release sem
	// if there are sem to execute 
    if (queue_length(sem->semQueue) > 0) {
        // Dequeue and unblock the next waiting thread
        struct uthread_tcb *wait_thread;
        int dequeue_res = queue_dequeue(sem->semQueue, (void **)&wait_thread);
        if (dequeue_res == 0) {
            uthread_unblock(wait_thread);
        } else {
            perror("Queue dequeue failed\n");
            return -1;
        }
    } else {
		//if nothing to run then indicate resources are avilable 
        sem->count++; 
    }

	return 0;
}

