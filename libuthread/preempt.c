#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

struct sigaction original_sigalarm_action;

//to gain a better understanding of preemption and some methods we should use i used these sources and this example vid:
// https://www.youtube.com/watch?v=4DhFmL-6SDA , http://courses.cms.caltech.edu/cs124/lectures-wi2016/CS124Lec15.pdf


void preempt_disable(void)
{
	// block SIGVTALRM alarm 
	//singal set
	sigset_t set;
	//new empty set 
	sigemptyset(&set);
	//adds SIGVTALRM to signal set 
	sigaddset(&set, SIGVTALRM);

	//block signal from deliv to thread-> disables preemption
	//got some help online : https://man7.org/linux/man-pages/man2/sigprocmask.2.html , https://pubs.opengroup.org/onlinepubs/7908799/xsh/sigprocmask.html
	sigprocmask(SIG_BLOCK, &set, NULL);
}

void preempt_enable(void)
{
	// unblock SIGVTALRM alarm 
	//singal set
	sigset_t set;
	//new empty set 
    sigemptyset(&set);
	//adds SIGVTALRM to signal set 
    sigaddset(&set, SIGVTALRM);
    // Unblock SIGVTALRM -> allows signals to deliver 
    sigprocmask(SIG_UNBLOCK, &set, NULL);
}

// this is the function that will run when we call the signal 
void signal_handler_function(){
	//yield to allow next thread to exec
	uthread_yield();
}

void preempt_start(bool preempt)
{
	//no preemption check
	if (preempt == false ){
		//printf("preeptive off\n");
		return;
	}else{
		//printf("preeptive on\n");
	}

	//signal action 
	// https://man7.org/linux/man-pages/man2/sigaction.2.html
	struct sigaction action;
	//clear signals 
	// at first i didnt use this and signals were getting mixed and multiple threads got blocked
	// looked online and asked chatGPT what is a valid solution to this and found this method.
	// https://man7.org/linux/man-pages/man3/sigemptyset.3p.html
	sigemptyset(&action.sa_mask);
	//clear flags
	action.sa_flags = 0;
	//signal handler function -> yield()
	action.sa_handler = signal_handler_function;
	
	//SIGVTALRM action is now handler function
	// https://pubs.opengroup.org/onlinepubs/007904875/functions/sigaction.html
	if (sigaction(SIGVTALRM, &action, &original_sigalarm_action) == -1) {
    	perror("Failed to set new signal handler");
	}

	//timer
	struct itimerval timer;
	timer.it_value.tv_sec = 0;
	//sets interval for timer 1000000 / HZ
	timer.it_value.tv_usec = 1000000 / HZ;  
	//sets the intervak var wutg the delay and interval 
	timer.it_interval = timer.it_value; 
	
	// found onlne about ITIMER_vitual : https://www.informit.com/articles/article.aspx?p=23618&seqNum=14 , https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-setitimer-set-value-interval-timer, 
	//timer -> SIGVTALRM when expires
	if (setitimer(ITIMER_VIRTUAL, &timer, NULL) == -1) {
		perror("Failed to start timer");
	}

}

void preempt_stop(void)
{
	// got some help online about this phase 
	// https://man7.org/linux/man-pages/man2/setitimer.2.html
	struct itimerval timer = { 0 };
	//cancel timer 	
	setitimer(ITIMER_VIRTUAL, &timer, NULL);

	// got help online for how to properly use : https://stackoverflow.com/questions/10330837/catching-sigvtalrm-signal-cp , https://www.gnu.org/software/libc/manual/html_node/Setting-an-Alarm.html
	//restore old SIGVTALRM action with original_sigalarm_action 
	if(sigaction(SIGVTALRM, &original_sigalarm_action, NULL) == -1){
		perror("cant restore handler");
	}
}
