/*********************************************************************** 
** Program Filename: keygen.c
** Author: Troy Diaz
** Date: 12/07/24
** Description: Generates a random key and is printed to stdout.
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "otp.c"

int main(int argc, char *argv[]);

/********************************************************************* 
** Function: main
** Description: Generates a random encryption key that is then printed to stdout.
** Parameters: int argc - argument count,
**             char *argv[] - argument vector
** Pre-Conditions: Key length is provided and must be positive integer.
** Post-Conditions: Key is printed to stdout.
*********************************************************************/
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "KEYGEN: Please enter a key length.\n");
        exit(1);
    }

    int key_length = atoi(argv[1]);
    char *key_characters = malloc(key_length + 1);
    memset(key_characters, '\0', key_length + 1);

    srand(time(NULL));

    int index = 0;
    while (index < key_length) {
        // Generate a random number
        int random_value = rand() % 27;

        // Map random value to character
        char random_char = random_value + 64;
        if (random_value == 0) {
            random_char = ' ';
        }

        // Store randomly generated character in the key
        key_characters[index] = random_char;

        index++;
    }

    // Print key
    printf("%s\n", key_characters);

    free(key_characters);
    
    return 0;
}
