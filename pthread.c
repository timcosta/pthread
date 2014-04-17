#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <pthread.h>
#include "queue.c"

// Global Vars
int schedularCreated = 0;

// Thread Control block struct
//		(define it in queue.c)

// Schedular Object
Queue *schedular;
// add pointer to queue of the current running TCB 
// Add int for thread-id to be joining
// A flag(schedularAction) for what the schedular is doing next 
//		exit 0, yield 1, join 2 






// Creates a user level thread
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg) {

	// Stack for the thread being created
	/*char thread_stack[SIGSTKSZ]; // or 16384

	// Initialize the context for the new thread
	getContext(&thread->thread_context);
	(thread->thread_context).uc_link = &thread->main_context;
	(thread->thread_context).uc_stack.ss_sp = thread_stack;
	(thread->thread_context).uc_stack.ss_size = sizeof(thread_stack);

	// Bind the function and arguments to be used for the next context switch
	makecontext(&thread->thread_context,start_routine,arg); 

	// Start executing the new thread work
	swapcontext(&thread->main_context, &thread->thread_context); */


	/*  - - - - - - - 

	Create Schedular

	Create Schedular context if schedularCreated flag is 0.
		-Can hard code stack creation
		-Use function schedule 
	

	Create new thread control block(make context)
		-use malloc for creation of TCB
			-use malloc for making new stack for the context
		-add to back of queue


	- - - - - - - - - */

	return 0;

}


// Terminate the calling thread. Return value set that can be used by the calling thread when calling pthread_join
void pthread_exit(void *retval) { 
	
	/*thread->val = *retval;

	//  Give up the CPU to the main thread
	swapcontext(&(thread.thread_context), &(thread.main_context));*/

	/*  - - - - - - - 
	
	- Set schedularAction flag to 0 

	- swap to schedular context to perform exit

	- - - - - - - - - */

}

// Calling thread gives up the CPU
int pthread_yield(void) {

	//  Give up the CPU to the main thread
	/*swapcontext(&(thread.thread_context), &(thread.main_context));
	*/

	/*  - - - - - - - 
	
	- Set schedularAction flag to 1 

	- swap to schedular context to perform yield

	- - - - - - - - - */


	return 0;
}


// Finish execution of the target thread before finishing execution of the calling thread
int pthread_join(pthread_t thread, void **value_ptr {

	/*if (value_ptr != NULL) value_ptr = thread.val;

	//  Finish the execution of the target thread
	swapcontext(&(thread.main_context), &(thread.thread_context));*/

	return 0;

	/*  - - - - - - - 
	
	- Set schedularAction flag to 2 

	- Set schedular joining flag

	- swap to schedular context to perform yield

	- - - - - - - - - */

}

// Schecule the next task on the queue 
void schedule() {

	/* - - - - - - - - - -

	-yield
		-adds current TCB to the back of queue
		-sets head of ready queue to current
		-change context to current TCB context


	-exit
		- add list of joins from current TCB to back of ready queue
		-detete memory of current TCB
			-stack of u context
			-TCB itself
		-sets head of ready queue to current
		- change context to current TCB context

	-join
		-search for joining TCB in ready queue(and joining queues)
		- add current TCB to back of its joining queue 
		-sets head of ready queue to current
		- change context to current TCB context


	*- - - - - - - - - - -/

}