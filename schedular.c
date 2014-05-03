/**
 * queue.c
 *
 * This file contains the implementation of the job queue
 */
#include <ucontext.h>
#include <pthread.h>

// Constants
#define MAX_NUM_NODES 1000
#define MAX_NUM_COND_VARS 1000


// TCB(Thread control Block)
typedef struct TCB {
	pthread_t thread_id;
	ucontext_t thread_context;
	int join_val;
} TCB;

// The Node for queue functionality in schedular
typedef struct Node {
	TCB  * thread_cb;
	struct Node * next;
	struct Node * prev;
	struct Node * join_list; // this is a list of all the threads joining on this thread
} Node;


// Array of linked lists(map) for conditional variable queues(size of the max number set)
// Trade off: We are alocating this memory for improved speed when adding threads to the cond. var waiting queues
struct Node *condVarMap[MAX_NUM_COND_VARS];

// The Schedular Struct
typedef struct Schedular {

	struct Node * head; // The current executing context
	struct Node * tail; // Back of the queue
	int size;
	int maxSize;

	// Vals for thread lib
	int exit_val;
	int action;
	pthread_t numCreated;
	pthread_t join_id;
	ucontext_t sched_context;

	// Vals for synchronization
	int nextCondId; // Id of the next cond. var in the cond. var map
	int currCondVarId;  // Id of the cond. var under operation
} Schedular;


// Add a job to the queue
void addThread(pthread_t *thread, Schedular * s, TCB * block) {
	//fprintf(stdout,"addJob\n");

	// Add thread to ready queue if not full 
	if (canCreateThread(s)) {

		Node * temp = (Node *) malloc(sizeof(Node));

		// Thrad ID of the block
		block->thread_id = ++s->numCreated;
		*thread = s->numCreated;
		
		temp->thread_cb = block;
		temp->next = NULL;
		temp->join_list = NULL;

		// The first job added to the list
		if (s->head == NULL) {
			s->head = temp;
			s->tail = temp;
			s->head->prev = NULL;

		} else {
			s->tail->next = temp;
			s->tail->next->prev = s->tail;
			s->tail = temp;
		}

		// Increment the size of the schedular queue
		s->size++;

	}
}


// Run the next thread in the ready queue
void runNextThread(Schedular * s) {

	// if there is more than one node on the ready queue, move the head to the back
	if(s->tail != s->head) { 

		// Add current TCB to the back of queue
		s->tail->next = s->head;
		s->tail->next->prev = s->tail;

		// Set the new head 
		s->head = s->head->next;
		s->head->prev = NULL;

		// Set the tail correctly
		s->tail = s->tail->next;
		s->tail->next = NULL;
	}


	// If a thread terminates, this calls pthread exit for it 
	schedular->action = 0;

	// Change context to new TCB context
	swapcontext(&s->sched_context,&s->head->thread_cb->thread_context);


}


// Exit the current running thread
void currExit(Schedular * s) {

	
	Node * temp = s->head->join_list;

	// Add list of joins from current TCB to back of ready queue
	while (temp != NULL) {

		// Set the join val for all threads joining on the current exiting one, to the exit val of the exiting one
		temp->thread_cb->join_val = s->exit_val;

		// Add temp to the back of the queue
		s->tail->next = temp;
		s->tail->next->prev = s->tail;

		// Set temp to the next node in the joining list
		temp = temp->next;

		// Set tail to the end of ready queue
		s->tail = s->tail->next;
		s->tail->next = NULL;

	}

	
	temp = s->head; 

	// Set the next thread in the ready queue to the head
	if (s->head == s->tail) {
		s->head = NULL;
		s->tail = NULL;
	} else {
		s->head = s->head->next;
		s->head->prev = NULL;
	}

	free((temp->thread_cb->thread_context).uc_stack.ss_sp); // Delete memory from this TCB context stack
	free(temp->thread_cb); // Free the TCB itself 
	free(temp); // delete the memory of this node

	// Decrement the size of the queue
	s->size--;

	// If a thread terminates, this calls pthread exit for it 
	schedular->action = 0;

	// Unless the last thread has exited, swap back to user mode
	if (s->head != NULL) {
		// Change context to new TCB context
		swapcontext(&s->sched_context,&s->head->thread_cb->thread_context);
	} 

}

// Find node with TCB thread_id == id
Node * findTarget(Node * root, pthread_t id) {

	// Check if we hit a dead end
	if (root == NULL) return NULL;

	// Check if we have the correct node
	if (root->thread_cb->thread_id == id) return root;

	// Recursively search for the correct node
	Node* n = findTarget(root->next,id);
	Node* j = findTarget(root->join_list,id);

	// Return recursive result
	return (j==NULL) ? n : j;


}

// Join current running thread to another thread
void join(Schedular * s) {

	// Find the thread we are joing on
	Node * temp = findTarget(s->head, s->join_id);

	if (temp != NULL) {

		// Set temp to be the joining list
		temp = temp->join_list;

		// Add current TCB to back of its joining queue
		if (temp == NULL) {
			temp = s->head; 
		} else {
			while (temp->next != NULL) temp = temp->next;
			temp->next = s->head;
			temp = temp->next;
		}

		// Set head of ready queue to current
		s->head = s->head->next;
		s->head->prev = NULL;

		// Make the end of the join list NULL
		temp->next = NULL;

		// If a thread terminates, this calls pthread exit for it 
		schedular->action = 0;

		// Change context to current TCB context
		swapcontext(&s->sched_context,&s->head->thread_cb->thread_context);

	} else {

		// If a thread terminates, this calls pthread exit for it 
		schedular->action = 0;

		// Change context to current TCB context
		swapcontext(&s->sched_context,&s->head->thread_cb->thread_context);
	}
}

// Add the current thread to the correct conditional variable queue
void waitOnCond (Schedular *s) {

	// Get the first node in the queue 
	Node * temp = condVarMap[s->currCondVarId];

	// Add the current thread to the back of the list
	if (temp == NULL) {
		temp = s->head;
	} else {
		while(temp->next != NULL) temp = temp->next;
		temp->next = s->head;
		temp = temp->next;
	}

	// Set head of ready queue to current
	s->head = s->head->next;
	s->head->prev = NULL;

	// Make the end of the join list NULL
	temp->next = NULL;

	// If a thread terminates, this calls pthread exit for it 
	schedular->action = 0;

	// Change context to current TCB context
	swapcontext(&s->sched_context,&s->head->thread_cb->thread_context);

}

// Take the head of the cond. var queue and put it back on the ready queue
void signal(Schedular *s) {

	// Get the head of the queue
	Node * temp = condVarMap[s->currCondVarId];

	if (temp != NULL) {
		
		addToReadyTail(temp);
	}

	// If a thread terminates, this calls pthread exit for it 
	schedular->action = 0;

	// Change context to current TCB context
	swapcontext(&s->sched_context,&s->head->thread_cb->thread_context);
}

// Add all threads waiting on the cond. variable back on the ready queue
void broadcast (Schedular *s) {

	// Get the head of the queue
	Node * temp = condVarMap[s->currCondVarId];

	// Add all threads to the back of the ready queue
	while (temp != NULL) {
		
		addToReadyTail(temp);

		temp = condVarMap[s->currCondVarId];
	}

	// If a thread terminates, this calls pthread exit for it 
	schedular->action = 0;

	// Change context to current TCB context
	swapcontext(&s->sched_context,&s->head->thread_cb->thread_context);
}

// Adds a node from a cond. var queue to the back of the ready queue
void addToReadyTail(Node* n) {

	// Add this to the back of the ready queue
	s->tail->next = n;
	s->tail->next->prev = s->tail;
	s->tail = n;

	// Set the head of the queue to the next value
	condVarMap[s->currCondVarId] = n->next;

	// End of the list points to NULL
	s->tail->next = NULL;
}

// Have we reached the maximum number of threads
int canCreateThread(Schedular * s) {
	//fprintf(stdout,"canAddJob\n");
	return(s->size < s->maxSize);
}

// Does the schedular have any threads to run
int isEmpty(Schedular * s) {
  //fprintf(stdout,"isJobAvailable\n");
  return(s->head == NULL);
}