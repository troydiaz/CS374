#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include "dialog.c"

void error(const char *msg) {
    fprintf(stderr, "%s", msg);
    exit(1);
}

void setupAddressStruct(struct sockaddr_in *address, int portNumber, char *hostname) {
    memset((char *)address, '\0', sizeof(*address));
    address->sin_family = AF_INET;
    address->sin_port = htons(portNumber);
    struct hostent *hostInfo = gethostbyname(hostname);
    if (hostInfo == NULL) {
        fprintf(stderr, "ERROR: no such host\n");
        exit(1);
    }
    memcpy((char *)&address->sin_addr.s_addr, hostInfo->h_addr_list[0], hostInfo->h_length);
}

char* getFileContents(char* filename);

int main(int argc, char *argv[]) {
    char* messageFileName = argv[1];
    char* message = getFileContents(messageFileName);
    message[strcspn(message,"\r\n")] = '\0';

    char* keyFileName = argv[2];
    char* key = getFileContents(keyFileName);

    if (strlen(message) > strlen(key)) {
        fprintf(stderr, "ERROR: The key is too short.\n");
        exit(1);
    }

    char node_name[] = "enc_client";
    int debug = 0;
    setup_dialog(node_name, debug);

    int socketFD, portNumber;
    struct sockaddr_in serverAddress;
    char buffer[256];

    if (argc < 4) {
        fprintf(stderr, "USAGE: %s hostname port\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[3]);
    char hostname[] = "localhost";

    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD < 0) {
        error("ERROR opening socket");
    }

    setupAddressStruct(&serverAddress, port, hostname);

    if (connect(socketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        error("ERROR connecting");
    }

    await_send_message(socketFD, "enc_client hello");
    char* response = await_receive_message(socketFD);
    if (strcmp(response, "enc_server hello") != 0) {
        fprintf(stderr, "ERROR: Server validation failed. Response: %s\n", response);
        usleep(100000);
        exit(1);
    }

    await_send_message(socketFD, message);
    await_send_message(socketFD, key);

    usleep(FLUSH_DELAY + strlen(message) * 2);
    char* ciphertext = await_receive_message(socketFD);

    printf("%s\n", ciphertext);

    close(socketFD);

    return 0;
}

char* getFileContents(char* filename) {
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "ERROR opening file: %s\n", filename);
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* string = malloc(fsize + 1);
    if (string == NULL) {
        fprintf(stderr, "ERROR allocating memory\n");
        exit(1);
    }

    fread(string, fsize, 1, f);
    fclose(f);

    string[fsize] = '\0';

    return string;
}
