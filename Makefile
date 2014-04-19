CC=gcc
AR = ar
ARFLAGS = ru
RANLIB = ranlib
CFLAGS= -g
SRCS= pthread.c schedular.c

all:: schedular.c test.c
	$(CC) -o test schedular.c test.c -pthread
	

test: test.o
	$(CC) test  -o test.o schedular.o pthread.o

test.o: test.c
	$(CC) -o test.o -c test.c

schedular.o: schedular.c
	$(CC) -o schedular.o -c schedular.c

pthread.o: pthread.c
	$(CC) -o pthread.o -c pthread.c

clean: clean
	rm *.o
