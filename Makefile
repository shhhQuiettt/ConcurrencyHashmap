CC = gcc
CFLAGS = -Wall -Wextra -pthread

all: program

program: main.o hashmap.o
	$(CC) $(CFLAGS) -o program main.o hashmap.o

main.o: main.c hashmap.h
	$(CC) $(CFLAGS) -c main.c

hashmap.o: hashmap.c hashmap.h
	$(CC) $(CFLAGS) -c hashmap.c

clean:
	rm -f *.o program
