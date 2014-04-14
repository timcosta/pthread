#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

// A thread representation object that keeps track of 
// the different contexts
typedef struct pthread_t {
	void * val;
	ucontext_t main_context, thread_context;
} pthread_t;


// Creates a user level thread
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg) {

}


// Finish execution of the target thread before finishing ececutino of the calling thread
int pthread_join(pthread_t thread, void **value_ptr);


// Terminate the calling thread
void pthread_exit(void *retval)