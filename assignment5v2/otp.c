#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char normalize(char c) {
    if (c == ' ') {
        return 0;
    } else {
        return c - 64;
    }
}

char project(char c) {
    if (c == 0) {
        return ' ';
    } else {
        return c + 64;
    }
}

char encrypt_char(char c, char k) {
    char n_c = normalize(c);
    char n_k = normalize(k);
    char n_e = (n_c + n_k) % 27;
    return project(n_e);
}

char decrypt_char(char e, char k) {
    char n_e = normalize(e);
    char n_k = normalize(k);
    char n_c = (n_e - n_k) % 27;
    if (n_c < 0) {
        n_c += 27;
    }
    return project(n_c);
}

char* encrypt_message(char* message, char* key) {
    int message_size = strlen(message);
    int key_size = strlen(key);

    char* encrypted = malloc(message_size + 1);
    memset(encrypted, '\0', message_size + 1);

    int i = 0;
    while (i < message_size) {
        char c = message[i];
        char k = key[i];
        encrypted[i] = encrypt_char(c, k);
        i++;
    }

    return encrypted;
}

char* decrypt_message(char* encrypted, char* key) {
    int message_size = strlen(encrypted);
    int key_size = strlen(key);

    if (message_size > key_size) {
        fprintf(stderr, "Key is too short.\n");
        exit(1);
    }

    char* decrypted = malloc(message_size + 1);
    memset(decrypted, '\0', message_size + 1);

    int i = 0;
    while (i < message_size) {
        char e = encrypted[i];
        char k = key[i];
        decrypted[i] = decrypt_char(e, k);
        i++;
    }
    return decrypted;
}
