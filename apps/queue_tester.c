#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

/* Create */
void test_create(void)
{
    fprintf(stderr, "*** TEST create ***\n");

    TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
    int data = 3, *ptr;
    queue_t q;

    fprintf(stderr, "*** TEST queue_simple ***\n");

    q = queue_create();
    queue_enqueue(q, &data);
    queue_dequeue(q, (void**)&ptr);

    TEST_ASSERT(ptr == &data);
}

/* Callback function that increments items */
static void iterator_inc(queue_t q, void *data)
{
    int *a = (int*)data;

    if (*a == 42)
        queue_delete(q, data);
    else
        *a += 1;
}

void test_iterator(void)
{
    queue_t q;
    int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
    size_t i;

    fprintf(stderr, "*** TEST queue_iterator and queue_delete ***\n");

    /* Initialize the queue and enqueue items */
    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    /* Increment every item of the queue, delete item '42' */
    queue_iterate(q, iterator_inc);
    TEST_ASSERT(data[0] == 2);
    TEST_ASSERT(queue_length(q) == 9);
}

void test_queue_destroy(void)
{
    queue_t q, q2;

    fprintf(stderr, "*** TEST queue_destroy ***\n");

    q = queue_create();
    int destroy_res = queue_destroy(q);
    TEST_ASSERT(destroy_res == 0);

    q2 = queue_create();
    int data = 3;
    queue_enqueue(q2, &data);
    int destroy_res2 = queue_destroy(q2);
    TEST_ASSERT(destroy_res2 == -1);
}

// TODO: 100% coverage - Test all cases of returning -1 for enqueue, dequeue, iterate, length, delete
// TODO: 100% coverage - Test front and rear cases of delete()

int main(void)
{
    test_create();
    test_queue_simple();
    test_iterator();
    test_queue_destroy();

    return 0;
}