CC=gcc
AR = ar
ARFLAGS = ru
RANLIB = ranlib
CFLAGS= -g
SRCS= pthread.c schedular.c

all:: pthread schedular
	strip pthread
	strip schedular

pthread: pthread.o
	$(CC) -o pthread pthread.o 

schedular: schedular.o
	$(CC) -o schedular schedular.o

schedular.o: schedular.c
	$(CC) -o schedular.o -c schedular.c

pthread.o: pthread.c
	$(CC) -o pthread.o -c pthread.c

clean: clean
	rm *.o
