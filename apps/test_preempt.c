#include <stdio.h>
#include <uthread.h>

void thread_fn2(void *arg) {
    (void)arg; // Avoid unused parameter warning
    for (int i = 0; i < 5; ++i) {
        printf("Thread 2: iteration %d\n", i);
        for (volatile int j = 0; j < 5000000; ++j); // Busy-wait to simulate work
    }
}

void thread_fn1(void *arg) {
    (void)arg; // Avoid unused parameter warning
    uthread_create(thread_fn2, NULL); // Create the second thread from within the first thread
    for (int i = 0; i < 5; ++i) {
        printf("Thread 1: iteration %d\n", i);
        for (volatile int j = 0; j < 5000000; ++j); // Busy-wait to simulate work
    }
}

int main(void) {
    // Start the threading system with preemption enabled and create the first thread
    uthread_run(true, thread_fn1, NULL);
    return 0;
}
