#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t mutex;
pthread_cond_t wrt;
int readCount = 0;

void * reader() {
	int count = 0;
	printf("\tInitializing Reader...\n");
	do {
		pthread_mutex_lock(&mutex);
		printf("r1\n");
		readCount++;
		printf("r2\n");
		if(readCount==1) pthread_cond_wait(&wrt,&mutex);
		printf("r3\n");
		pthread_mutex_unlock(&mutex);
		printf("\treading...");
		pthread_mutex_lock(&mutex);
		printf("r4\n");
		readCount--;
		if(readCount==0) pthread_cond_signal(&wrt);
		printf("r5\n");
		pthread_mutex_unlock(&mutex);
		printf("r6\n");
		count++;
	} while(count<10);
}

void * writer() {
	int count = 0;
	printf("\tInitializing Writer...\n");
	do {
		pthread_cond_wait(&wrt,&mutex);
		printf("\twriting...");
		sleep(5);
		printf("\tdone writing.");
		pthread_cond_signal(&wrt);
		count++;
	} while(count < 5);
}

void * first_message() {
	printf("\tFirst\n");
	pthread_yield();
	printf("\tThird\n");
	int val = 1;
	pthread_exit(&val);
}

void * second_message() {
	printf("\tSecond\n");
	pthread_yield();
	printf("\tFourth\n");
	int val = 2;
	pthread_exit(&val);
}

int shared=0;
pthread_mutex_t pcm;

void * producer() {
	int count = 0;
	do {
		printf("producing: %d\n",shared);
		count++;
		pthread_mutex_lock(&pcm);
		shared += 2;
		pthread_mutex_unlock(&pcm);
	}while(count<10);
}

void * consumer() {
	int count = 0;
	do {
		printf("consuming: %d\n",shared);
		count++;
		pthread_mutex_lock(&pcm);
		shared--;
		pthread_mutex_unlock(&pcm);
	}while(count<10);
}

void main(void) {

	pthread_t t1,t2,w1,r1,r2,r3,r4,pct1,pct2;

	printf("Threading Proof of Concept\n");
	pthread_create(&t1, NULL, &first_message, NULL);
	pthread_create(&t2, NULL, &second_message, NULL);
	printf("\tStarting...\n");
	int* val1;
	pthread_join(t1,(void**)&(val1));
	printf("\tThread 1 val: %d\n",*val1);
	int* val2;
	pthread_join(t2,(void**)&(val2));
	printf("\tThread 2 val: %d\n",*val2);
	printf("Above, you should have seen Starting followed by First, Second, Third, and Fourth printed out in order.\n");
	printf("The expected values are 1 and 2, the same as their thread ids.\n");

	printf("\n\n\nThe Readers-Writers Problem\n");
	printf("This problem uses both a mutex and a conditional variable.\n");

	pthread_mutex_init(&pcm);

	pthread_create(&pct1,NULL,&producer,NULL);
	pthread_create(&pct2,NULL,&consumer,NULL);

	pthread_join(pct1,NULL);
	pthread_join(pct2,NULL);

	// pthread_cond_init(&wrt,NULL);
	// pthread_mutex_init(&mutex,NULL);

	// pthread_create(&w1, NULL, &writer, NULL);
	// pthread_create(&r1, NULL, &reader, NULL);
	// //pthread_create(&r2, NULL, &reader, NULL);
	// //pthread_create(&r3, NULL, &reader, NULL);
	// //pthread_create(&r4, NULL, &reader, NULL);

	// pthread_join(r1,NULL);
	// pthread_join(w1,NULL);
	//pthread_join(r2,NULL);
	//pthread_join(r3,NULL);
	//pthread_join(r4,NULL);

	printf("Readers-Writers problem completed.\n");
	printf("End of test sequence.\n");

}

