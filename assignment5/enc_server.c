#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void setupAddressStruct(struct sockaddr_in* address, int portNumber) {
    memset((char*) address, '\0', sizeof(*address)); 
    address->sin_family = AF_INET;
    address->sin_port = htons(portNumber);
    address->sin_addr.s_addr = INADDR_ANY;
}

// Encrypt plaintext with the key using modular arithmetic (mod 27)
void encryptMessage(char* plaintext, char* key, char* ciphertext) {
    int i;
    for (i = 0; i < strlen(plaintext); i++) {
        int ptValue = (plaintext[i] == ' ') ? 26 : plaintext[i] - 'A';
        int keyValue = (key[i] == ' ') ? 26 : key[i] - 'A';
        int encryptedValue = (ptValue + keyValue) % 27;
        ciphertext[i] = (encryptedValue == 26) ? ' ' : 'A' + encryptedValue;
    }
    ciphertext[i] = '\0'; // Null-terminate the ciphertext
}

int main(int argc, char *argv[]) {
    int connectionSocket, charsRead;
    char buffer[70000], plaintext[70000], key[70000], ciphertext[70000];
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t sizeOfClientInfo = sizeof(clientAddress);

    if (argc < 2) { 
        fprintf(stderr, "USAGE: %s port\n", argv[0]); 
        exit(1);
    } 
  
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0) {
        error("ERROR opening socket");
    }

    // Allow port reuse to avoid TIME_WAIT issues
    int opt = 1;
    if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        error("ERROR setting socket options");
    }

    setupAddressStruct(&serverAddress, atoi(argv[1]));

    if (bind(listenSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        error("ERROR on binding");
    }

    listen(listenSocket, 5);

    while (1) {
        connectionSocket = accept(listenSocket, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); 
        if (connectionSocket < 0) {
            error("ERROR on accept");
        }

        memset(buffer, '\0', sizeof(buffer));
        charsRead = recv(connectionSocket, buffer, sizeof(buffer) - 1, 0); 
        if (charsRead < 0) {
            error("ERROR reading from socket");
        }

        printf("SERVER: Received message: \"%s\"\n", buffer);

        // Validate input
        if (charsRead >= sizeof(buffer) - 1) {
            fprintf(stderr, "ERROR: Message too large to process\n");
            close(connectionSocket);
            continue;
        }

        // Split the received message into plaintext and key
        char* token = strtok(buffer, "|");
        if (token == NULL) {
            fprintf(stderr, "ERROR: Malformed message (missing delimiter)\n");
            close(connectionSocket);
            continue;
        }
        strcpy(plaintext, token);

        token = strtok(NULL, "|");
        if (token == NULL) {
            fprintf(stderr, "ERROR: Malformed message (missing key)\n");
            close(connectionSocket);
            continue;
        }
        strcpy(key, token);

        // Trim newlines
        plaintext[strcspn(plaintext, "\n")] = '\0';
        key[strcspn(key, "\n")] = '\0';

        // Validate key length
        if (strlen(key) < strlen(plaintext)) {
            fprintf(stderr, "ERROR: Key length is shorter than plaintext length\n");
            close(connectionSocket);
            continue;
        }

        // Encrypt the plaintext
        memset(ciphertext, '\0', sizeof(ciphertext));
        encryptMessage(plaintext, key, ciphertext);

        // Send the ciphertext back to the client
        charsRead = send(connectionSocket, ciphertext, strlen(ciphertext), 0); 
        if (charsRead < 0) {
            error("ERROR writing to socket");
        }

        close(connectionSocket);
    }

    close(listenSocket); 
    return 0;
}
