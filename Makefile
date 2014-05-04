CC=gcc
AR = ar
ARFLAGS = ru
RANLIB = ranlib
CFLAGS= -g
SRCS= pthread.c schedular.c

all:: test
	

test: test.o 
	$(CC) -o test pthread.o test.o

test.o: pthread
	$(CC) -c test.c -o test.o

pthread: schedular
	$(CC) -c pthread.c -o pthread.o

schedular: 
	$(CC) -c schedular.c -o schedular.o

clean: 
	rm *.o
