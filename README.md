pthread
=======

Submission One

This project implements pthread_create, pthread_exit, pthread_yield, and pthread_join. We used makecontext and swapcontext to implement the context switching for these threads.

We used a queue to keep track of which context was supposed execute when, and simply iterated through this thread when pthread_yield was called or when a pthread terminated operation.

We included a file, test.c, that creates two threads that just print "THREADING" after a "Starting..." line which is after the thread lines in the code. This code works, which validates our implementation.
