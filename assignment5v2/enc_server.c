#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include "dialog.c"
#include "otp.c"

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void setupAddressStruct(struct sockaddr_in *address, int portNumber) {
    memset((char *)address, '\0', sizeof(*address));
    address->sin_family = AF_INET;
    address->sin_port = htons(portNumber);
    address->sin_addr.s_addr = INADDR_ANY;
}

void handle_connection(int connection_socket);

int await_next_connection(int listen_socket) {
    int connection_socket;
    struct sockaddr_in client_address;
    socklen_t size_of_client_info = sizeof(client_address);

    connection_socket = accept(listen_socket, (struct sockaddr *)&client_address, &size_of_client_info);
    if (connection_socket < 0) {
        error("ERROR on accept");
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("forking failed");
        exit(1);
    } else if (pid == 0) {
        printf("Child process started.\n");
        handle_connection(connection_socket);
        printf("Child process closed.\n");
        exit(0);
    }

    return pid;
}

#define MAX_BUFFER_SIZE 256

void dialog(int connection_socket);

void handle_connection(int connection_socket) {
    dialog(connection_socket);
    close(connection_socket);
}

void dialog(int connection_socket) {
    char* client_hello = await_receive_message(connection_socket);
    if (strcmp(client_hello, "enc_client hello") != 0) {
        fprintf(stderr, "Client did not say hello. Closing connection.\n");
        await_send_message(connection_socket, "enc_server hello");
        usleep(100000);
        close(connection_socket);
        exit(1);
    }
    printf("Client said hello.\n");
    await_send_message(connection_socket, "enc_server hello");

    char* plaintext = await_receive_message(connection_socket);
    char* key = await_receive_message(connection_socket);
    char* ciphertext = encrypt_message(plaintext, key);

    printf("plaintext len: %d, key len: %d, ciphertext len: %d\n", strlen(plaintext), strlen(key), strlen(ciphertext));

    usleep(FLUSH_DELAY + strlen(ciphertext) * 2);
    await_send_message(connection_socket, ciphertext);
}

int main(int argc, char *argv[]) {
    int connectionSocket, charsRead;
    char buffer[256];
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

    setupAddressStruct(&serverAddress, atoi(argv[1]));

    if (bind(listenSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        error("ERROR on binding");
    }

    listen(listenSocket, 5);

    char node_name[] = "enc_server";
    int debug = 0;
    setup_dialog(node_name, debug);

    while (1) {
        int pid = await_next_connection(listenSocket);
    }

    close(listenSocket);
    return 0;
}
