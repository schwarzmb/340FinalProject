CC=gcc
CFLAGS=-c -O2 -Wall -g

all: smokers_problem

smokers_problem: binary_semaphore.o smokers_problem.o
	$(CC) binary_semaphore.o smokers_problem.o -o smokers_problem -lpthread

binary_semaphore.o: binary_semaphore.c
	$(CC) $(CFLAGS) binary_semaphore.c

smokers_problem.o: smokers_problem.c
	$(CC) $(CFLAGS) smokers_problem.c

clean:
	/bin/rm -f smokers_problem *.o *.gz

run:
	./smokers_problem 3 5 7

tarball:
	# put your tar command here
	# tar -cvzf schwarz.tar.gz hw4.c
