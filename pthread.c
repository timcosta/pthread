#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <pthread.h>
#include "schedular.c"

// typedef unsigned long int pthread_t;


// Global Vars
int schedularCreated = 0; // Flag set to 1 if schedular has been created
Schedular *schedular; // Schedular Object



// Creates a user level thread
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg) {


	// Check flag to see if the schedular has been created. If not, create it.
	if (schedularCreated == 0) {

		// Create TCB for main
		TCB * main_block =  (TCB *) malloc(sizeof(TCB));

		schedular = makeSchedular(main_block);
		schedularCreated = 1;
	}

	// Dynamically create a new thread
	TCB * new_thread =  (TCB *) malloc(sizeof(TCB));

	// Thread's context stack 
	char * thread_stack = (char *) malloc(sizeof(char) * 16384);

	// Initialize this new context
	getcontext(&new_thread->thread_context);
	(new_thread->thread_context).uc_link          = &schedular->sched_context;
    (new_thread->thread_context).uc_stack.ss_sp   = thread_stack;
    (new_thread->thread_context).uc_stack.ss_size = sizeof(thread_stack);

    // Create the context for the new thread
	makecontext(&ew_thread->thread_context, start_routine, 1, arg);

	// Add this to the ready queue
	addThread(thread, schedular, new_thread);

	return 0;

}


// Terminate the calling thread. Return value set that can be used by the calling thread when calling pthread_join
void pthread_exit(void *value_ptr) { 
	
	// thread->val = *retval;

	
	// Set schedularAction flag to 0 
	schedular->action = 0;

	// swap to schedular context to perform exit
	swapcontext(&schedular->head->thread_cb->thread_context, &schedular->sched_context);

}

// Calling thread gives up the CPU
int pthread_yield(void) {
	
	// Set schedular action flag to 1 	
	schedular->action = 1;

	// swap to schedular context to perform yield
	swapcontext(&schedular->head->thread_cb->thread_context, &schedular->sched_context);


	return 0;
}


// Finish execution of the target thread before finishing execution of the calling thread
int pthread_join(pthread_t thread, void **value_ptr) {

	//if (value_ptr != NULL) value_ptr = thread.val;

	// Set schedular action flag to 2 
	schedular->action = 2;

	// Set schedular joining flag
	schedular->join_id = thread;

	// swap to schedular context to perform join
	swapcontext(&schedular->head->thread_cb->thread_context, &schedular->sched_context);
	
	return 0;
}

// Schecule the next task on the queue 
void schedule(void) {

	// While the schedular has threads that need executing
	while (!isEmpty(schedular)) { 


		if (schedular->action == 0) {

			// exit the current running thread
			currExit(schedular);

		} else if (schedular->action == 1) {

			// yield
			runNextThread(schedular);

		} else if (schedular->action == 2) {

	-join
		-search for joining TCB in ready queue(and joining queues)
		- add current TCB to back of its joining queue 
		-sets head of ready queue to current
		- change context to current TCB context


		}
	
	} 

}


// Create a new schedular
struct Schedular * makeSchedular(TCB * main_block) {

	// Allocate memory for Schedular
	Schedular * s = (Schedular *) malloc(sizeof(Schedular));

	// Initialize the variables
	s->size = 0;
	s->maxSize = MAX_NUM_NODES;
	s->numCreated = 0;
	s->head = NULL;
	s->tail = NULL;
	s->action = -1;

	// Schedular's context stack 
	char sched_stack[16384];

	// Initialise the schedular context. uc_link points to main_context
	getcontext(&s->sched_context);
    (s->sched_context).uc_link          = &main_block->thread_context;
    (s->sched_context).uc_stack.ss_sp   = sched_stack;
    (s->sched_context).uc_stack.ss_size = sizeof(sched_stack);
 
 	// Create the context for the schedular
	makecontext(&s->sched_context, schedule, 0);

	// dummy pthread_t for the main
	pthread_t thread;

	// Add the main context to the head of the run queue list == it is running
	addThread(&thread, s, main_block);

	// Return the initialized queue
	return s;
}