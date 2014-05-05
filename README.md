pthread
=======

Submission One

This project implements pthread_create, pthread_exit, pthread_yield, and pthread_join. We used makecontext and swapcontext to implement the context switching for these threads.

We used a queue to keep track of which context was supposed execute when, and simply iterated through this thread when pthread_yield was called or when a pthread terminated operation.

We included a file, test.c, that creates two threads that just print "THREADING" after a "Starting..." line which is after the thread lines in the code. This code works, which validates our implementation.

Submission Two

This portion of the project implements synchronization, and also includes a full test suite, which the first submission did not. 

The synchronization part of the project includes conditional variables and mutexes. The conditional variables are implemented using a waiting queue (linked list) in an array at an index stored on the pthread_cond_t struct. The mutexes use a lock and a linked list at an index stored on the pthread_mutex_t struct to keep track of the order in which locks were requested. 

The test suite includes a threading proof of concept that utilizes pthread_create(), pthread_join(), pthread_yield(), and pthread_exit(). It also includes a producer-consumer example, as well as an implementation of the readers writers problem that uses both mutexes and conditional variables. Run make, and then ./test to view the output of these tests. 

A few notes regarding our implementation...

We used maps for convenience. These maps allowed us to repurpose the variables used in the prebuilt pthread.h structs to reference queues that we implemented. 

These maps made it so we didnt have to recreate Nodes each time we wanted to do a context switch or anything like that. It made it easy to implement pthread_join() and pthread_cond_wait() as well. 

The issue with using the queues is that pthread_join() is a very complicated function that essentially performs a full tree traversal in search of the thread it is supposed to join on. 

The alarm functionality was implmented in the pthread.c file, and was initialized at the end of every pthread call. Initializing the alarm function at the end of every call allows up to perform the threading tasks without interruption, but allowed pthread_yield() to be triggered at any point while user defined functions were being executed. This means we used alarm(0) at the beginning of each pthread function to cancel preexisting alarms, and then alarm(1) at the end of each to set a 1 second alarm for round-robin preemptive scheduling. 
