#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>  
#include <sys/socket.h> 
#include <netdb.h>     

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void setupAddressStruct(struct sockaddr_in* address, int portNumber, char* hostname) {
    memset((char*) address, '\0', sizeof(*address)); 
    address->sin_family = AF_INET;
    address->sin_port = htons(portNumber);

    struct hostent* hostInfo = gethostbyname(hostname); 
    if (hostInfo == NULL) { 
        fprintf(stderr, "CLIENT: ERROR, no such host\n"); 
        exit(1); 
    }
    memcpy((char*) &address->sin_addr.s_addr, hostInfo->h_addr_list[0], hostInfo->h_length);
}

int main(int argc, char *argv[]) {
    int socketFD, portNumber, charsWritten, charsRead;
    struct sockaddr_in serverAddress;
    char buffer[70000];

    if (argc != 4) {
        fprintf(stderr,"USAGE: %s plaintext key port\n", argv[0]);
        exit(1);
    }

    FILE *plaintextFile = fopen(argv[1], "r");
    if (plaintextFile == NULL) {
        fprintf(stderr, "CLIENT: ERROR opening plaintext file\n");
        exit(1);
    }

    FILE *keyFile = fopen(argv[2], "r");
    if (keyFile == NULL) {
        fprintf(stderr, "CLIENT: ERROR opening key file\n");
        exit(1);
    }

    char plaintext[70000], key[70000];
    memset(plaintext, '\0', sizeof(plaintext));
    memset(key, '\0', sizeof(key));
    fgets(plaintext, sizeof(plaintext) - 1, plaintextFile);
    fgets(key, sizeof(key) - 1, keyFile);
    fclose(plaintextFile);
    fclose(keyFile);

    plaintext[strcspn(plaintext, "\n")] = '\0';
    key[strcspn(key, "\n")] = '\0';

    if (strlen(key) < strlen(plaintext)) {
        fprintf(stderr, "CLIENT: ERROR key is too short\n");
        exit(1);
    }

    char message[140000];
    memset(message, '\0', sizeof(message));
    snprintf(message, sizeof(message) - 1, "%s|%s", plaintext, key);


    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD < 0) {
        error("CLIENT: ERROR opening socket");
    }

    setupAddressStruct(&serverAddress, atoi(argv[3]), "localhost");

    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        error("CLIENT: ERROR connecting");
    }

    charsWritten = send(socketFD, message, strlen(message), 0);
    if (charsWritten < 0) {
        error("CLIENT: ERROR writing to socket");
    }
    if (charsWritten < strlen(message)) {
        fprintf(stderr, "CLIENT: WARNING: Not all data written to socket!\n");
    }

    memset(buffer, '\0', sizeof(buffer));
    charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0);
    if (charsRead < 0) {
        error("CLIENT: ERROR reading from socket");
    }
    printf("CLIENT: Received ciphertext: \"%s\"\n", buffer);

    close(socketFD); 
    return 0;
}
