#include <stdio.h>
#include <stdlib.h>

void * first_message() {
	printf("First\n");
	pthread_yield();
	printf("Third\n");

}

void * second_message() {
	printf("Second\n");
	pthread_yield();
	printf("Fourth\n");
	int val = 5;
	pthread_exit(&val);
}

void main(void) {

	pthread_t t1;
	pthread_t t2;


	pthread_create(&t1, NULL, &first_message, NULL);
	pthread_create(&t2, NULL, &second_message, NULL);
	printf("Starting...\n");
	pthread_join(t1,NULL);
	int* val;
	pthread_join(t2,(void**)&(val));
	printf("val from 2: %d\n",val);
	printf("last action\n");
	exit(0);
}

