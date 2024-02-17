# Project 2 Report
## Phase 1: Queue
### High Level Desciption
- Linked list implementation using void * to store data of any type
- Used to store threads
### Design Choices and Implementation Details
- We use pointers to the front and rear of the queue to avoid having to loop 
    through the queue (results in O(1) implementation of many functions)
- Create: To create the pointer to the queue (type queue_t), we malloc and 
    initialize the queue attributes length (0), front, and rear (both NULL)
- Enqueueing:
    - We malloc for a node pointer. If this is the first new node, we set front 
        and rear to this node. Otherwise, we add the node and reset the rear 
        pointer.
- Dequeueing:
    - We grab the front node and insert its data into *data. If the queue is 
    now empty, we set the rear to NULL (front would become NULL when we move 
    it).
- Delete:
    - We conduct a linear scan for the node to delete, keeping track of the 
    previous and next nodes. 
    - If the front or rear is a match, we move the respective pointer. 
    - Otherwise, we bridge the previous and next nodes.
- Iterate:
    - We iterate from front to rear and call the function with each node's data.
    - We use next node similarly to previous/next nodes in Delete, for the 
    delete case - we have to set current node to next node if current is 
    deleted, since we won't be able to get currentNode->next.
- We also added functions for queue printing and a quick function just to do    
    something with the queue variable, for testing purposes (not included in 
    the .h).
### Testing
- We included Professor Porquet's testers, and added our own to test the    
    remaining functions (iteration, destruction, and deletion.)

## Phase 2: Uthread Library
This section won't include many details on semaphores or preemption scheduling  
    (see the sections for Phases 3 and 4 below).
### High Level Desciption
- Way to allow for concurrent (NOT parallel) execution of a task
- Attempts to replace kernel-level threads and scheduling
### Design Choices and Implementation Details
- We used 2 global queues (readyQ and finishedQ) for holding ready and      
    completed tasks respectively. We did not see a need for a running queue or 
    a blocked queue at the time of writing phase 2. The finishedQ helps 
    differentiate between running and exited tasks.
- We originally had an enum for Thread States (e.g. Ready/Done/Running), but 
    since our implementation only needed to interact with the queues 
    themselves, we saw no need for a state tracker.
    - Thus, our tcb only contained a context and a stack pointer.
- We use 3 global thread pointers for the main (idle) thread, current thread, 
    and previous thread. This made it easier to change the current and previous 
    threads within various functions.
- Create
    - We malloc'd space for a new tcb pointer, then use uthread_ctx functions 
        to allocate space for the stack pointer and initialize the context.
    - The new thread is ready, so we enqueue it to the readyQ.
- Run
    - We initialize the queues, then create the main thread using 
        uthread_create. This main thread is queued into readyQ by default.
    - To avoid a loop where the only running thread is the idle thread, we 
        immediately dequeue it, which also gives us a pointer to the thread 
        that we can use for cleanup later.
    - We initialize current and previous global thread pointers to the idle 
    thread.
    - The main loop will continuously yield to other threads until only the 
    idle thread remains (based on readyQ length). Then, the loop breaks and we 
    enter the cleanup stage (see below) before returning.
- Cleanup
    - We dequeue and delete nodes from our finishedQ (these threads called exit 
        at some point), making sure to destroy the threads' stacks as well.
    - We then destroy both queues and deallocate the main thread and its stack.
- Yield/Exit 
    - These operate similarly - we use the previous global thread pointer 
    to "save" the current thread, before assigning a new ready thread to 
    current.
        - This signifies a new thread has transitioned from ready to running.
    - For yield, the old thread is enqueued into readyQ to be resumed at some 
    point, so we use uthread_ctx_switch since we intend to save its context.
    - For exit, the old thread is finished, so it is enqueued into finishedQ 
    instead. We don't need to save its context so we use setcontext() instead 
    of ctx_switch.
- We included block/unblock functions and a current thread getter, all to be    
    used in the semaphore API (phase 3). The block and unblock functions use 
    the readyQ to signify transitions between the ready, blocked, and running 
    states.

## Phase 3: Semaphore API
### High Level Desciption
- Way to control access to common resources by multiple threads 
- Implementation has structure which has a counter and a queue which holds 
    threads waiting to run 
- Semaphore allows multiple threads to access finite number of resources
### Design Choices and Implementation Details
#### Semaphore Structure 
- Count Variable:
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
            queue. This ensures the thread is considered for resource 
            allocation once a resource becomes available again.
        - FIFO Order: The queue operates on a First In, First Out (FIFO) basis,
            meaning threads are dequeued and given access to resources in the 
            order they were enqueued. This promotes fairness, ensuring no 
            thread is starved or indefinitely blocked.
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
    - With a higher number of threads, managing the queue will become costly
- Priority 
    - Since we have a FIFO queue, higher priority threads wait for lower ones

## Phase 4: Preemption 
### High Level Description
- Allows threads to preemptively switch to ensure fair CPU time. 
- Uses signals (SIGVTALRM) and virtual timers to periodically interrupt thread 
exec, which enables scheduling. 
### Design Choices and Implementation Details
- Signal Handling 
    - Signal setup (in preempt_start):
        - configure 'SIGVTALRM' to trigger the signal_handler_function which is 
        linked to uthread_yield(). Thus, when 'SIGVTALRM' is triggered, the 
        thread yields and we can go to exec the next thread
        - Rational:
            - Provide a controlled way to interrupt the threads using the 
            'SIGVTALRM' signal to enforce context switching. 
            - More fair allocation. 
- Timer configuration (in preempt_start):
    - Virtual Timer
        - intializes a virtual timer (ITIMER_VIRTUAL) to generate SIGVTALRM 
        signals at a frequency defined by HZ.
        - Rational:
            - This ensures preemption as threads are constantly yielding with 
            the SIGVTALRM signal. Allows systen to freq release thread running. 
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

# Works Cited
https://www.baeldung.com/cs/semaphore
https://www.geeksforgeeks.org/semaphores-in-process-synchronization/
https://www.youtube.com/watch?v=ukM_zzrIeXs
https://www.youtube.com/watch?v=4DhFmL-6SDA
http://courses.cms.caltech.edu/cs124/lectures-wi2016/CS124Lec15.pdf
https://man7.org/linux/man-pages/man2/sigprocmask.2.html
https://pubs.opengroup.org/onlinepubs/7908799/xsh/sigprocmask.html
https://man7.org/linux/man-pages/man2/sigaction.2.html
https://www.scaler.com/topics/c/implementation-of-queue-using-linked-list/
https://man7.org/linux/man-pages/man3/sigemptyset.3p.html
https://man7.org/linux/man-pages/man3/sigemptyset.3p.html
https://pubs.opengroup.org/onlinepubs/007904875/functions/sigaction.html
https://www.informit.com/articles/article.aspx?p=23618&seqNum=14
https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-setitimer-set-value-interval-timer
https://man7.org/linux/man-pages/man2/setitimer.2.html
https://stackoverflow.com/questions/10330837/catching-sigvtalrm-signal-cp
https://www.gnu.org/software/libc/manual/html_node/Setting-an-Alarm.html
