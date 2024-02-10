#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"
#include <stdio.h> // TODO: DELETE

typedef struct Node {
    // Linked list implementation
    void *data; // Can handle any data type // TODO: change to int if conceptually difficult to use
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
    queue_t queue = malloc(sizeof(queue));
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
    printf("############\n");
    printf("New node addr: %p\n", queue->rear->data);

    return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
    // TODO: DELETE PRINT STMTS
    printf("############\n");
    printf("Dequeue called with queue: %p, data: %p\n", (void*)queue, (void*)data);
    printf("Queue length before dequeue: %d\n", queue->length);

    /*
     * For my reference: void **data is used to pass by reference,
     * so that the &ptr passed in can be modified directly,
     * instead of just doing ptr=data which would pass by value
     */

    if (queue == NULL || data == NULL || queue->length == 0)
        return -1;

	qNode *oldestNode = queue->front; // Since queue->front points to a node in the heap
    printf("Oldest node address: %p, Oldest node data: %p\n", (void*)oldestNode, oldestNode->data);

    *data = oldestNode->data; // Deref to modify ptr by ref instead of value (data)
    printf("Data to be returned: %p\n", *data);

    queue->front = queue->front->next;
    queue->length--;
    // Rear is still pointing to the removed node in this case below
    if (queue->length == 0)
        queue->rear = NULL;
    printf("New queue front: %p, New queue rear: %p\n", (void*)queue->front, (void*)queue->rear);
    printf("Queue length after dequeue: %d\n", queue->length);
    printf("############\n");

    // *data has the data, this node is no longer needed
    free(oldestNode);

    return 0;
}

int queue_delete(queue_t queue, void *data)
{
    if (queue == NULL || data == NULL)
        return -1;

	/* TODO Phase 1 */

    return 0;
}

int queue_iterate(queue_t queue, queue_func_t func)
{
    if (queue == NULL || func == NULL)
        return -1;

	/* TODO Phase 1 */

    return 0;
}

int queue_length(queue_t queue)
{
    if (queue == NULL)
        return -1;

	return queue->length;
}

