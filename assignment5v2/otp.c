/*********************************************************************** 
** Program Filename: otp.c
** Author: Troy Diaz
** Date: 
** Description: 
*********************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char normalize(char input_char);
char project(char input_char);
char encrypt_char(char plain_char, char key_char);
char decrypt_char(char cipher_char, char key_char);
char* encrypt_message(char* plaintext, char* key);
char* decrypt_message(char* ciphertext, char* key);

/********************************************************************* 
** Function: normalize
** Description: 
** Parameters: char input_char - character to normalize
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
char normalize(char input_char) {
    if (input_char == ' ') {
        return 0;
    } else {
        return input_char - 64;
    }
}

/********************************************************************* 
** Function: project
** Description: 
** Parameters: char input_char - character to project
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
char project(char input_char) {
    if (input_char == 0) {
        return ' ';
    } else {
        return input_char + 64;
    }
}

/********************************************************************* 
** Function: encrypt_char
** Description: 
** Parameters: char plain_char - character to encrypt,
**             char key_char - key character
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
char encrypt_char(char plain_char, char key_char) {
    char normalized_plain = normalize(plain_char);
    char normalized_key = normalize(key_char);
    char encrypted_normalized = (normalized_plain + normalized_key) % 27;
    return project(encrypted_normalized);
}

/********************************************************************* 
** Function: decrypt_char
** Description: 
** Parameters: char cipher_char - character to decrypt,
**             char key_char - key character
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
char decrypt_char(char cipher_char, char key_char) {
    char normalized_cipher = normalize(cipher_char);
    char normalized_key = normalize(key_char);
    char decrypted_normalized = (normalized_cipher - normalized_key) % 27;
    if (decrypted_normalized < 0) {
        decrypted_normalized += 27;
    }
    return project(decrypted_normalized);
}

/********************************************************************* 
** Function: encrypt_message
** Description: 
** Parameters: char* plaintext - message to encrypt,
**             char* key - encryption key
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
char* encrypt_message(char* plaintext, char* key) {
    int plaintext_length = strlen(plaintext);
    int key_length = strlen(key);

    char* encrypted_message = malloc(plaintext_length + 1);
    memset(encrypted_message, '\0', plaintext_length + 1);

    int index = 0;
    while (index < plaintext_length) {
        char plain_char = plaintext[index];
        char key_char = key[index];
        encrypted_message[index] = encrypt_char(plain_char, key_char);
        index++;
    }

    return encrypted_message;
}

/********************************************************************* 
** Function: decrypt_message
** Description: 
** Parameters: char* ciphertext - message to decrypt,
**             char* key - decryption key
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
char* decrypt_message(char* ciphertext, char* key) {
    int ciphertext_length = strlen(ciphertext);
    int key_length = strlen(key);

    if (ciphertext_length > key_length) {
        fprintf(stderr, "Key is too short.\n");
        exit(1);
    }

    char* decrypted_message = malloc(ciphertext_length + 1);
    memset(decrypted_message, '\0', ciphertext_length + 1);

    int index = 0;
    while (index < ciphertext_length) {
        char cipher_char = ciphertext[index];
        char key_char = key[index];
        decrypted_message[index] = decrypt_char(cipher_char, key_char);
        index++;
    }
    return decrypted_message;
}