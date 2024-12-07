#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include "dialog.c"

void error(const char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

void setupAddressStruct(struct sockaddr_in *address, int portNumber, char *hostname) {
    memset((char *)address, '\0', sizeof(*address));
    address->sin_family = AF_INET;
    address->sin_port = htons(portNumber);
    struct hostent *hostInfo = gethostbyname(hostname);
    if (hostInfo == NULL) {
        error("No such host");
    }
    memcpy((char *)&address->sin_addr.s_addr, hostInfo->h_addr_list[0], hostInfo->h_length);
}

char* getFileContents(char* filename);

int main(int argc, char *argv[]) {
    if (argc < 4) {
        error("Usage: <program> <message_file> <key_file> <hostname> <port>");
    }

    char* messageFileName = argv[1];
    char* message = getFileContents(messageFileName);
    message[strcspn(message, "\r\n")] = '\0';

    char* keyFileName = argv[2];
    char* key = getFileContents(keyFileName);

    if (strlen(message) > strlen(key)) {
        error("Key is too short for the message");
    }

    char node_name[] = "dec_client";
    setup_dialog(node_name, 0);

    int socketFD;
    struct sockaddr_in serverAddress;
    int port = atoi(argv[3]);
    char* hostname = argv[4]; 

    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD < 0) {
        error("Error opening socket");
    }

    setupAddressStruct(&serverAddress, port, hostname);

    if (connect(socketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        error("Error connecting to server");
    }

    await_send_message(socketFD, "dec_client hello");
    char* response = await_receive_message(socketFD);
    if (strcmp(response, "dec_server hello") != 0) {
        fprintf(stderr, "Error: Decryption server validation failed\n");
        exit(1);
    }

    await_send_message(socketFD, message);
    await_send_message(socketFD, key);

    usleep(FLUSH_DELAY + strlen(message) * 2);
    char* plaintext = await_receive_message(socketFD);

    printf("%s\n", plaintext);

    close(socketFD);

    return 0;
}


char* getFileContents(char* filename) {
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "Error opening file '%s'\n", filename);
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* string = malloc(fsize + 1);
    if (string == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }

    fread(string, fsize, 1, f);
    fclose(f);

    string[fsize] = '\0';

    return string;
}
