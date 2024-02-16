#include <stdio.h>
#include <uthread.h>

void thread_fn2(void *arg) {
    (void)arg; 
    for (int i = 0; i < 5; ++i) {
        printf("Thread 2: iteration %d\n", i);
        // "wait" -> simulates wrok 
        for (volatile int j = 0; j < 5000000; ++j); 
    }
}

void thread_fn1(void *arg) {
    (void)arg; 
    // 2nd thread 
    uthread_create(thread_fn2, NULL);
    //code to exec 
    for (int i = 0; i < 5; ++i) {
        printf("Thread 1: iteration %d\n", i);
        // "wait" -> simulates wrok 
        for (volatile int j = 0; j < 5000000; ++j); // Busy-wait to simulate work
    }
}

int main(void) {
    //start thread -> preemption enabled 
    uthread_run(true, thread_fn1, NULL);
    return 0;
}

// supposed to see threads schudling running
// one runs then siwtches to other and so one until theyre done executing 
