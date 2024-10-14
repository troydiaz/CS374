# Variables
CC = gcc
CFLAGS = -Wall -g
OBJ = main.o movies.o
DEPS = movies.h

# Default rule to build the executable
all: main

# Rule to link object files and create the executable
main: $(OBJ)
	$(CC) $(CFLAGS) -o main $(OBJ)

# Rule to compile main.c into main.o
main.o: main.c $(DEPS)
	$(CC) $(CFLAGS) -c main.c

# Rule to compile movies.c into movies.o
movies.o: movies.c $(DEPS)
	$(CC) $(CFLAGS) -c movies.c

# Clean rule to remove compiled files
clean:
	rm -f *.o main
