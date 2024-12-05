#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    // Check if the user provided a key length
    if (argc != 2) {
        printf("Usage: keygen keylength");
    }

    // Convert arg to int
    int keylength = atoi(argv[1]);
    if (keylength <= 0) {
        fprintf(stderr, "Error: Key length must be a positive integer.\n");
        exit(1);
    }

    // Array of valid characters: A-Z and space
    char characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

    // Seed the random number generator
    srand((unsigned int)time(NULL));

    // Generate key
    for (int i = 0; i < keylength; i++) {
        printf("%c", characters[rand() % 27]);
    }

    // Add newline
    printf("\n");

    return 0;
}
