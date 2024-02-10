#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

typedef struct Node {
    // Linked list implementation
    void *data; // Can handle any data type // TODO: change to int if conceptually difficult to use
    struct Node *next;
} node;

struct queue {
    int numNodes;
    struct Node *front;
    struct Node *rear;
};

queue_t queue_create(void)
{
	queue_t queue = /* TODO: Malloc call based on queue size */

    // Initialize vars
    queue->numNodes = 0;
    queue->front = NULL;
    queue->rear = NULL;

    return queue;
}

int queue_destroy(queue_t queue)
{
	/* TODO: error check */
    free(queue);
    return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	/* TODO Phase 1 */
}

int queue_dequeue(queue_t queue, void **data)
{
	/* TODO Phase 1 */
}

int queue_delete(queue_t queue, void *data)
{
	/* TODO Phase 1 */
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	/* TODO Phase 1 */
}

int queue_length(queue_t queue)
{
    /* TODO: error check */
	return queue->numNodes;
}

