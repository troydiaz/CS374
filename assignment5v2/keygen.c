/*********************************************************************** 
** Program Filename: keygen.c
** Author: Troy Diaz
** Date: 
** Description: 
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "otp.c"

int main(int argc, char *argv[]);

/********************************************************************* 
** Function: main
** Description: 
** Parameters: int argc - argument count,
**             char *argv[] - argument vector
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Please enter a key length.\n");
        exit(1);
    }

    int key_length = atoi(argv[1]);

    char *key_characters = malloc(key_length + 1);
    memset(key_characters, '\0', key_length + 1);

    srand(time(NULL));

    int index = 0;
    while (index < key_length) {
        int random_value = rand() % 27;
        char random_char = random_value + 64;
        if (random_value == 0) {
            random_char = ' ';
        }
        key_characters[index] = random_char;
        index++;
    }

    printf("%s\n", key_characters);

    return 0;
}
