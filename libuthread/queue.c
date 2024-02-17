#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

typedef struct Node {
    // Linked list implementation
    void *data; // Can handle any data type
    struct Node *next;
} qNode;

struct queue {
    int length;
    // Front and rear used for O(1) implementation (no looping thru queue)
    qNode *front;
    qNode *rear;
};

queue_t queue_create(void)
{
    // queue_t queue should now point to a spot in the heap
    queue_t queue = malloc(sizeof(*queue));
    if (queue == NULL)
        return NULL;

    // Initialize vars
    queue->length = 0;
    queue->front = NULL;
    queue->rear = NULL;

    return queue;
}

int queue_destroy(queue_t queue)
{
	if (queue == NULL || queue->length > 0)
        return -1;

    free(queue);
    return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
    if (queue == NULL || data == NULL)
        return -1;

    // Allocate for new node
    qNode *newNode = malloc(sizeof(qNode));
    if (newNode == NULL)
        return -1;
    newNode->data = data;
    newNode->next = NULL;

    // Size 0: front = rear
    // Size >= 1: front != rear
    if (queue->length == 0) {
        queue->front = newNode;
        queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
        // TODO: might need to set newNode->next, but I don't think it's needed
    }
    queue->length++;

    return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
    /*
     * For my reference: void **data is used to pass by reference,
     * so that the &ptr passed in can be modified directly,
     * instead of just doing ptr=data which would pass by value
     */
    if (queue == NULL || data == NULL || queue->length == 0){
        return -1;
    }
        

	qNode *oldestNode = queue->front; // Since queue->front points to a node in the heap

    *data = oldestNode->data; // Deref to modify ptr by ref instead of value (data)

    queue->front = queue->front->next;
    queue->length--;
    // Rear is still pointing to the removed node in this case below
    if (queue->length == 0){
        queue->rear = NULL;
    }
        

    // *data has the data, this node is no longer needed
    free(oldestNode);

    return 0;
}

int queue_delete(queue_t queue, void *data)
{
    if (queue == NULL || data == NULL)
        return -1;

    // iterate thru queue until match
    qNode *currNode = queue->front;
    qNode *prevNode = NULL; // for rewiring

    while(currNode != NULL) {
        if (currNode->data == data) {
            // if front is a match, if rear is a match, if middle is a match
            if (prevNode == NULL) {
                queue->front = queue->front->next;
            } else if (currNode == queue->rear) {
                queue->rear = prevNode;
                prevNode->next = NULL; // TODO: Is this line needed? Make a tester to figure this out
            } else {
                prevNode->next = currNode->next;
            }
            free(currNode);
            queue->length--;
            return 0;
        }
        prevNode = currNode;
        currNode = currNode->next;
    }

    return -1;
}

int queue_iterate(queue_t queue, queue_func_t func)
{
    if (queue == NULL || func == NULL)
        return -1;

    qNode *currNode = queue->front;
    // curr->next is inaccessible if it's deleted, so we need to save nextNode beforehand
    qNode *nextNode;
    while(currNode != NULL) {
        nextNode = currNode->next;
        func(queue, currNode->data); // use queue not &queue since queue is already a ptr
        currNode = nextNode;
    }

    return 0;
}

int queue_length(queue_t queue)
{
    if (queue == NULL){
        return -1;
    }
        

	return queue->length;
}

// Helper function for debugging
int queue_print(queue_t queue)
{
    if (queue == NULL) {
        perror("Queue null\n");
        return -1;
    }
    if (queue->length == 0) {
        perror("Queue empty\n");
        return -1;
    }

    int index = 0;
    qNode *currNode = queue->front;
    qNode *nextNode;
    while(currNode != NULL) {
        nextNode = currNode->next;

        currNode = nextNode;
        index++;
    }

    return 0;
}

// Helper function to use the queue in the testers
int use_q(queue_t queue)
{
    if (queue != NULL) {
        return 0;
    } else {
        return 1;
    }
}

