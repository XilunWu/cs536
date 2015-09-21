CC=gcc
CFLAGS=-lpthread
hw1: clg.c myhttpd.c	
	$(CC) -o clg clg.c $(CFLAGS)
	$(CC) -o myhttpd myhttpd.c $(CFLAGS)

clean: 
	rm clg myhttpd
