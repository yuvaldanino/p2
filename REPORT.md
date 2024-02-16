# Phase 1 

# Phase 2 

# Phase 3 Semaphore API
### High Level Desciption:
- Way to control access to common resources by multiple threds 
- Implementation has structure which has a counter and a queue which holds 
    threads waiting to run 
- Semaphore allows multiple threads to access finite number of resources
### Design Choices and Implementation Details:
- #### semaphore structure 
- count Variable:
    - Tracks the number of available resources.
    - A positive count indicates available resources; a non-positive count 
    requires threads to wait.
    - Rationale:
        - Ensures mutual exclusion and prevents race conditions by managing 
        access to shared resources.
        - Allows handling of multiple resources, extending beyond binary 
        semaphore functionality. 
- Thread Queue:
    - Holds threads that cannot currently acquire a resource.
    - Threads are enqueued when resources are unavailable and dequeued in 
    FIFO order when resources are released.
    - Rationale:
        - Maintains a fair and orderly mechanism for resource allocation among 
        waiting threads.
        - Guarantees that all waiting threads will eventually proceed, 
        preventing starvation and ensuring fairness.
    - Implementation and functionality:
        - Thread Enqueueing: When a thread attempts to acquire a resource 
        (sem_down) and finds none available (count <= 0), it is placed into the
            queue. This ensures the thread is considered for resource allocation 
            once a resource becomes available again.
        - FIFO Order: The queue operates on a First In, First Out (FIFO) basis,
            meaning threads are dequeued and given access to resources in the 
            order they were enqueued. This promotes fairness, ensuring no thread 
            is starved or indefinitely blocked.
- Sem Functions Functionality:
    - create -> Initializes a semaphore with a queue and a resource count.
    - destruct -> Destroys the queue, ensuring no threads are left waiting.
    - sem down -> Acquires a resource, or queues and blocks the thread if 
    unavailable.
    - sem up -> Releases a resource and unblocks the next waiting thread if any.
### Limitations 
- Potential for starvation 
    - Thread that require more resouces for longer hold timer may cause long 
    wait times for other threads.
- Scalability Concern
    - With a higher number of threads, managing the queue will become constly
- Priority 
    - Since we have a FIFO queue, higher priority threads wait for lower ones
# Phase 4 Preemption 
### High Level Desciption:
- Allows threads to preemptively siwtch to ensure fair CPU time. 
- Uses signals (SIGVTALRM) and virtual timers to periodically interrupt thread 
exec, which enables scheduling. 
### Design Choices and Implementation Details:
- Signal Handling 
    - Signal setup (in preempt_start):
        - configure 'SIGVTALRM' to trigger the singal_handler_function which is 
        linked to uthread_yield(). Thus when 'SIGVTALRM' is triggered, the 
        thread yields and we can go to exec the next thread
        - Rational:
            - Provide a controlled way to interuppt the threads using the 
            'SIGVTALRM' signal to enforce context switching. More fiar 
            allocation. 
- Timer configuration (in preempt_start):
    - Virtual Timer
        - intializes a virtual timer (ITIMER_VIRTUAL) to generate SIGVTALRM 
        signals at a frequency defined by HZ.
        - Rational:
            - This ensures preemption as threads are contantly yielding with the 
            SIGVTALRM signal. Allows systen to freq release thread running. 
- Preemption control:
    - disable preemption (in preempt_disable):  
        - blocks SIGVTALRM signal, thus preventing the yield action
    - enable preemption (in preempt_enable):
        - unblocks SIGVTALRM signal, allowing yielding
### Testing 
- Program creates 2 threads, each runs a busy-waiting loop to simulate work
- Expected output should show alternating messages from thread 1 and 2, 
indicating a context switch as preemption is enabled. 
- #### **Indicators of Effective Preemption**
    - Switch from thread 1 to 2 and back to 1 shows preemption is taking place, 
    as the threads are not running to completion in 1 uninterrupted sequence. 
    - Both threads are given CPU time and neither is starved, which shows the 
    schedulers fairness and responsiveness. 
    - Note: observed non-strict alternation in preemption test
        - Due to factors like timer resolution and scheduling policies, 
        and does not diminish the overall efficiency and fairness of the 
        preemption mechanism.






