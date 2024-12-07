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
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        printf("[dec_server]: Child process started.\n");
        handle_connection(connection_socket);
        printf("[dec_server]: Child process closed.\n");
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
    if (strcmp(client_hello, "dec_client hello") != 0) {
        fprintf(stderr, "[dec_server]: Client didn't say hello. Closing connection.\n");
        await_send_message(connection_socket, "dec_server hello");
        usleep(100000);
        close(connection_socket);
        exit(1);
    }
    await_send_message(connection_socket, "dec_server hello");

    char* ciphertext = await_receive_message(connection_socket);
    char* key = await_receive_message(connection_socket);
    char* plaintext = decrypt_message(ciphertext, key);

    printf("[dec_server]: Decrypted message: %d/%d/%d\n", strlen(plaintext), strlen(key), strlen(ciphertext));

    usleep(FLUSH_DELAY + strlen(plaintext) * 2);
    await_send_message(connection_socket, plaintext);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "USAGE: %s port\n", argv[0]);
        exit(1);
    }

    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0) {
        error("ERROR opening socket");
    }

    struct sockaddr_in serverAddress;
    setupAddressStruct(&serverAddress, atoi(argv[1]));

    if (bind(listenSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        error("ERROR on binding");
    }

    listen(listenSocket, 5);

    setup_dialog("dec_server", 0);

    while (1) {
        await_next_connection(listenSocket);
    }

    close(listenSocket);
    return 0;
}
