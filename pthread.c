#include <stdio.h>
#include <stdlib.h>
#include "schedular.c"

// typedef unsigned long int pthread_t;

// Global Vars
int schedularCreated = 0; // Flag set to 1 if schedular has been created
Schedular *schedular; // Schedular Object

// Schedular's context stack 
char sched_stack[16384];


struct Schedular * makeSchedular(TCB * main_block);

// Creates a user level thread
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *) , void *arg) {

	//printf("create\n");
	// Check flag to see if the schedular has been created. If not, create it.
	if (schedularCreated == 0) {

		// Create TCB for main
		TCB * main_block =  (TCB *) malloc(sizeof(TCB));

		schedular = makeSchedular(main_block);
		schedularCreated = 1;

		if(schedular->head == NULL) printf("sched head null\n");
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
	makecontext(&new_thread->thread_context, start_routine, 1, arg);

	// Add this to the ready queue
	addThread(thread, schedular, new_thread);

	return 0;

}


// Terminate the calling thread. Return value set that can be used by the calling thread when calling pthread_join
void pthread_exit(void *value_ptr) { 
	
	// Set schedularAction flag to 0 
	schedular->action = 0;
	schedular->voluntaryExit = 1;

	// Set the exit val
	joinVals[schedular->head->thread_cb->thread_id] = *((int*)value_ptr);

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
	//printf("join on thread %d\n",thread);
	//printf("j1\n");
	// Set schedular action flag to 2 
	schedular->action = 2;

	// Set schedular joining flag
	schedular->join_id = thread;

	//printf("j2\n");

	// swap to schedular context to perform join
	swapcontext(&schedular->head->thread_cb->thread_context, &schedular->sched_context);

	//printf("j3\n");

	// Set the join val
	*(*value_ptr*) = &joinVals[thread];

	//printf("j4\n");

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

			// Join to another thread
			join(schedular);

		} else if (schedular->action == 3) {
			// Add the current thread to the queue of the specified cond. variable
			waitOnCond(schedular);

		} else if (schedular->action == 4) {
			// Take one thread off the waiting queue and add it to the ready queue
			sig(schedular);
		} else if (schedular->action == 5) {

			// Add all threads waiting on a specific cond. variable to the back of the queue
			broadcast(schedular);
		}else if (schedular->action == 6) {
			// Take one thread off the mutex waiting queue and add it to the ready queue
			unlock(schedular);
		} else if (schedular->action == 7) {
			// Add all threads waiting on a specific mutex to the back of the queue
			lock(schedular);
		}
	
	} 


	//printf("done\n");

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
	s->voluntaryExit = 0;
	s->nextCondId = 0;
	s->nextMutexId = 0;

	// Initialise the schedular context. uc_link points to main_context
	getcontext(&s->sched_context);
    (s->sched_context).uc_link          = 0; // Schedular doesn't link back to any particular thread
    (s->sched_context).uc_stack.ss_sp   = sched_stack;
    (s->sched_context).uc_stack.ss_size = sizeof(sched_stack);
 
 	// Create the context for the schedular
	makecontext(&s->sched_context, schedule, 0);

	// dummy pthread_t for the main
	pthread_t thread;

	// Seth the link back to schedular when the main terminates
	getcontext(&main_block->thread_context);
	(main_block->thread_context).uc_link = &s->sched_context;

	// Add the main context to the head of the run queue list == it is running
	addThread(&thread, s, main_block);

	// Return the initialized queue
	return s;
}




/************************ SYNCHRONIZATION ****************************/


//// Mutex //////


// Initialize the mutex
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr) {

	// count as index
	// lock is either 0(free) or 1(locked)

	// Check if the schedular has been built. If not build it
	if (schedularCreated == 0) {

		// Create TCB for main
		TCB * main_block =  (TCB *) malloc(sizeof(TCB));

		schedular = makeSchedular(main_block);
		schedularCreated = 1;
	}

	// Check if you can create another mutex
	if (schedular->nextMutexId == MAX_NUM_MUTEX_VARS) return -1;

	// Create the queue for this cond. var

	// Set the index(count) for the mutex. for where it is in the queue array
	mutex->__data.__count = schedular->nextMutexId++;
	mutex->__data.__lock = 0;

	return 0;

}

// Destroy the mutex
int pthread_mutex_destroy(pthread_mutex_t *mutex) {
	
	free(mutex);
	return 0;

}

// Lock the mutex
int pthread_mutex_lock(pthread_mutex_t *mutex) {

	if(mutex->__data.__lock == 1) {
		schedular->action = 7;
		schedular->currMutexVarId = mutex->__data.__count;
		swapcontext(&schedular->head->thread_cb->thread_context, &schedular->sched_context);
	}
	mutex->__data.__lock = 1;
	return 0;

}

// Unlock the mutex
int pthread_mutex_unlock(pthread_mutex_t *mutex) {

	schedular->action = 6;
	schedular->currMutexVarId = mutex->__data.__count;
	swapcontext(&schedular->head->thread_cb->thread_context, &schedular->sched_context);
	mutex->__data.__lock = 0;
	return 0;

}


/////////// Conditional Vars /////////////


// Initialize the conditional variable
int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr) {

	// Check if the schedular has been built. If not build it
	if (schedularCreated == 0) {

		// Create TCB for main
		TCB * main_block =  (TCB *) malloc(sizeof(TCB));

		schedular = makeSchedular(main_block);
		schedularCreated = 1;
	}

	// Check if you can create another conditional variable
	if (schedular->nextCondId == MAX_NUM_COND_VARS) return -1;


	// Set the index(lock) for the cond. var. for where it is in the queue array
	cond->__data.__lock = schedular->nextCondId++;

	return 0;
}

// Destroy the conditional variable
int pthread_cond_destroy(pthread_cond_t *cond) {

	// Delete the memory allocated for the conditional variable 
	free(cond);

	return 0;
}


// Wait until another thread wakes up this one
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {

	// Give up the mutex lock
	pthread_mutex_unlock(mutex);

	// Set the schedular cond. var index(lock) to the correct value
	schedular->currCondVarId = cond->__data.__lock;

	// Set the correct action for the schedular
	schedular->action = 3;

	// Add the current running thread to the queue of the cond. var(context switch)
	swapcontext(&schedular->head->thread_cb->thread_context, &schedular->sched_context);

	// Reaquire the mutex 
	pthread_mutex_lock(mutex);

	return 0;
}

// Wake up the next thread waiting on the conditional variable 
int pthread_cond_signal(pthread_cond_t *cond) {

	// Set the schedular cond. var index to the correct value
	schedular->currCondVarId = cond->__data.__lock;

	// Set the correct action for the schedular 
	schedular->action = 4;

	// Take off the first thread from the queue of the cond. var and add to the ready queue(context switch)
	swapcontext(&schedular->head->thread_cb->thread_context, &schedular->sched_context);


	return 0;
}


// Wake up all threads waiting on the conditional variable 
int pthread_cond_broadcast(pthread_cond_t *cond) {

	// Set the schedular cond. var index to the correct value
	schedular->currCondVarId = cond->__data.__lock;

	// Set the correct action for the schedular 
	schedular->action = 5;

	// Take off the each thread from the queue of the cond. var and add to the ready queue(context switch)
	swapcontext(&schedular->head->thread_cb->thread_context, &schedular->sched_context);

	return 0;
}
