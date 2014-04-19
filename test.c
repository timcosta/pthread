#include <stdio.h>
#include <stdlib.h>

void * print_message() {
	printf("THREADING\n");
}

void main(void) {

	pthread_t t1;
	pthread_t t2;

	pthread_create(&t1, NULL, &print_message, NULL);
	pthread_create(&t2, NULL, &print_message, NULL);
	printf("Starting...\n");

	exit(0);
}

