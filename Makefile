CC=gcc
AR = ar
ARFLAGS = ru
RANLIB = ranlib
CFLAGS= -g
SRCS= pthread.c schedular.c

all:: test
	

test: pthread 
	$(CC) -o test pthread.o test.o

pthread: schedular
	$(CC) -c pthread.c -o pthread.o

schedular: 
	$(CC) -c schedular.c -o schedular.o

clean: 
	rm *.o
