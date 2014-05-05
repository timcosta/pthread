#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t mutex;
pthread_cond_t wrt;
int readCount = 0;

void reader() {
	printf("reading");
}

void writer() {

	printf("writing");

}

void * first_message() {
	printf("First\n");
	pthread_yield();
	printf("Third\n");
	int val = 1;
	pthread_exit(&val);
}

void * second_message() {
	printf("Second\n");
	pthread_yield();
	printf("Fourth\n");
	int val = 2;
	pthread_exit(&val);
}

void main(void) {

	pthread_t t1;
	pthread_t t2;


	pthread_create(&t1, NULL, &first_message, NULL);
	pthread_create(&t2, NULL, &second_message, NULL);
	printf("Starting...\n");
	int* val1;
	pthread_join(t1,(void**)&(val1));
	printf("val from 1: %d\n",val1);
	int* val2;
	pthread_join(t2,(void**)&(val2));
	printf("val from 2: %d\n",val2);
	printf("last action\n");
	exit(0);
}

