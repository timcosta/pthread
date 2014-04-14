#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

// A thread representation object that keeps track of 
// the different contexts
typedef struct pthread_t {
	int val;
	ucontext_t main_context, thread_context;
} pthread_t;


/**********************************************************************************************/
/********* Tim I THINK WE NEED A GLOBAL STRUCT(w/ queue of structs that has val and context)***/
/**********************************************************************************************/

// Creates a user level thread
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg) {

	// Stack for the thread being created
	char thread_stack[SIGSTKSZ];

	// Initialize the context for the new thread
	getContext(&thread->thread_context);
	(thread->thread_context).uc_link = &thread->main_context;
	(thread->thread_context).uc_stack.ss_sp = thread_stack;
	(thread->thread_context).uc_stack.ss_size = sizeof(thread_stack);

	// Bind the function and arguments to be used for the next context switch
	makecontext(&thread->thread_context,start_routine,arg); 

	// Start executing the new thread work
	swapcontext(&thread->main_context, &thread->thread_context);

	return 0;

}


// Finish execution of the target thread before finishing execution of the calling thread
int pthread_join(pthread_t thread, void **value_ptr {

	if (value_ptr != NULL) value_ptr = thread.val;

	//  Finish the execution of the target thread
	swapcontext(&(thread.main_context), &(thread.thread_context));

	return 0;
}


// Terminate the calling thread. Return value set that can be used by the calling thread when calling pthread_join
void pthread_exit(void *retval) { 
	thread->val = *retval;

	//  Give up the CPU to the main thread
	swapcontext(&(thread.thread_context), &(thread.main_context));
}

// Calling thread gives up the CPU
int pthread_yield(void) {

	//  Give up the CPU to the main thread
	swapcontext(&(thread.thread_context), &(thread.main_context));

	return 0;
}