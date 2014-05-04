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

}

void main(void) {

	pthread_t t1;
	pthread_t t2;


	pthread_create(&t1, NULL, &first_message, NULL);
	printf("created1\n");
	pthread_create(&t2, NULL, &second_message, NULL);
	printf("created2\n");
	printf("Starting...\n");
	pthread_join(&t1,NULL);
	pthread_join(&t2,NULL);
	exit(0);
}

