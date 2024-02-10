#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

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
	if (queue == NULL || queue->front != NULL)
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

    return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
    if (queue == NULL || data == NULL || queue->front != NULL)
        return -1;

	/* TODO Phase 1 */

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

