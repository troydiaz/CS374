#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "otp.c"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Please enter a key length.\n");
        exit(1);
    }

    int length = atoi(argv[1]);

    char *nums = malloc(length + 1);
    memset(nums, '\0', length + 1);

    srand(time(NULL));

    int i = 0;
    while (i < length) {
        int num = rand() % 27;
        char c = num + 64;
        if (num == 0) {
            c = ' ';
        }
        nums[i] = c;
        i++;
    }

    printf("%s\n", nums);

    return 0;
}
