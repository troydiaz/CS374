CC = gcc
CFLAGS = -Wall -g
OBJ = main.o movies.o
DEPS = movies.h

all: main

main: $(OBJ)
	$(CC) $(CFLAGS) -o main $(OBJ)

main.o: main.c $(DEPS)
	$(CC) $(CFLAGS) -c main.c

movies.o: movies.c $(DEPS)
	$(CC) $(CFLAGS) -c movies.c

clean:
	rm -f *.o main
