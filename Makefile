CC=gcc
CFLAGS=-c -O2 -Wall -g

all: hw4

hw4: binary_semaphore.o hw4.o
	$(CC) binary_semaphore.o hw4.o -o hw4 -lpthread

binary_semaphore.o: binary_semaphore.c
	$(CC) $(CFLAGS) binary_semaphore.c

hw4.o: hw4.c
	$(CC) $(CFLAGS) hw4.c

clean:
	/bin/rm -f hw4 *.o *.gz

run:
	./hw4 8 5 4

tarball:
	# put your tar command here
	 tar -cvzf schwarz.tar.gz hw4.c
