/**
 * queue.c
 *
 * This file contains the implementation of the job queue
 */

#define MAX_NUM_NODES 1000



// TCB(Thread control Block)
typedef struct TCB {
	pthread_t thread_id;
	ucontext_t thread_context;
	int join_val;
} TCB;

// The Node for queue functionality in schedular
typedef struct Node {
	TCB  * thread_cb;
	Node * next;
	Node * prev;
	Node * join_list; // this is a list of all the threads joining on this thread
} Node;


// The Schedular Struct
typedef struct Schedular {
	struct Node * head; // The current executing context
	struct Node * tail; // Back of the queue
	int size;
	int maxSize;
	int exit_val;
	pthread_t numCreated;
	pthread_t join_id;
	ucontext_t sched_context;
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
			s->tail->next->prev = q->tail;
			s->tail = temp;
		}

		// Increment the size of the schedular queue
		s->size++;

	}
}


// Run the next thread in the ready queue
void runNextThread(Schedular * s) {

	// Add current TCB to the back of queue
	s->tail->next = s->head;
	s->tail->next->prev = s->tail;

	// Set the new head 
	s->head = s->head->next;
	q->head->prev = NULL;

	// Set the tail correctly
	s->tail = s->tail->next;
	s->tail->next = NULL;


	// Change context to new TCB context
	swapcontext(&s->sched_context,&s->head->thread_cb->thread_context);


}


// Exit the current running thread
void currExit(Schedular * s) {

	
	Node * temp = s->head->join_list;

	// Add list of joins from current TCB to back of ready queue
	while (temp != NULL) {
		s->tail->next = temp;

	}

		-detete memory of current TCB
			-stack of u context
			-TCB itself

		-sets head of ready queue to current
		- change context to current TCB context)
}

// Remove a job from the queue to be used by a threadpool
void removeJob(Schedular * q, dispatch_fn * func, void ** arg) {
	//fprintf(stdout,"removeJob\n");
	// Only remove a job if there is at least one already on the queue 
	if (q->size > 0) {

		Node * temp = q->head;

		// Set these pointers in the thread to the values of the job info
		*func = temp->func;
		*arg  = temp->func_arg;

		// Remove the last job on the queue
		if (q->head == q->tail) {
			q->head = NULL;
			q->tail = NULL;
			free(temp); // delete the memory of this node

		} else {
			q->head = q->head->next;
			q->head->prev = NULL;
			free(temp); // delete the memory of this node
		}

		// Decrement the size of the queue
		q->size--;

	}
		
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