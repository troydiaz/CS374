/*********************************************************************** 
** Program Filename: enc_server.c
** Author: Troy Diaz
** Date: 
** Description: 
*********************************************************************/
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

#define MAX_BUFFER_SIZE 256

void error(const char *error_msg);
void setupAddressStruct(struct sockaddr_in *server_addr, int port_number);
void handle_connection(int client_socket);
int await_next_connection(int server_socket);
void dialog(int client_socket);

/********************************************************************* 
** Function: error
** Description: 
** Parameters: const char *error_msg - error message to display
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
void error(const char *error_msg) {
    perror(error_msg);
    exit(1);
}

/********************************************************************* 
** Function: setupAddressStruct
** Description: 
** Parameters: struct sockaddr_in *server_addr - server address structure,
**             int port_number - port to bind the server
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
void setupAddressStruct(struct sockaddr_in *server_addr, int port_number) {
    memset((char *)server_addr, '\0', sizeof(*server_addr));
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(port_number);
    server_addr->sin_addr.s_addr = INADDR_ANY;
}

/********************************************************************* 
** Function: await_next_connection
** Description: 
** Parameters: int server_socket - listening socket for server
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
int await_next_connection(int server_socket) {
    int client_socket;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_socket < 0) {
        error("ERROR on accept");
    }

    pid_t child_pid = fork();
    if (child_pid == -1) {
        perror("forking failed");
        exit(1);
    } else if (child_pid == 0) {
        printf("Child process started.\n");
        handle_connection(client_socket);
        printf("Child process closed.\n");
        exit(0);
    }

    return child_pid;
}

/********************************************************************* 
** Function: handle_connection
** Description: 
** Parameters: int client_socket - socket for client connection
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
void handle_connection(int client_socket) {
    dialog(client_socket);
    close(client_socket);
}

/********************************************************************* 
** Function: dialog
** Description: 
** Parameters: int client_socket - socket for client connection
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
void dialog(int client_socket) {
    char* client_greeting = await_receive_message(client_socket);
    if (strcmp(client_greeting, "enc_client hello") != 0) {
        fprintf(stderr, "Client did not say hello. Closing connection.\n");
        await_send_message(client_socket, "enc_server hello");
        usleep(100000);
        close(client_socket);
        exit(1);
    }
    printf("Client said hello.\n");
    await_send_message(client_socket, "enc_server hello");

    char* received_plaintext = await_receive_message(client_socket);
    char* received_key = await_receive_message(client_socket);
    char* encrypted_text = encrypt_message(received_plaintext, received_key);

    printf("plaintext len: %d, key len: %d, ciphertext len: %d\n", strlen(received_plaintext), strlen(received_key), strlen(encrypted_text));

    usleep(FLUSH_DELAY + strlen(encrypted_text) * 2);
    await_send_message(client_socket, encrypted_text);
}

/********************************************************************* 
** Function: main
** Description: 
** Parameters: int argc - number of arguments,
**             char *argv[] - argument list
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
int main(int argc, char *argv[]) {
    int server_socket;
    struct sockaddr_in server_config;

    if (argc < 2) {
        fprintf(stderr, "USAGE: %s port\n", argv[0]);
        exit(1);
    }

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        error("ERROR opening socket");
    }

    setupAddressStruct(&server_config, atoi(argv[1]));

    if (bind(server_socket, (struct sockaddr *)&server_config, sizeof(server_config)) < 0) {
        error("ERROR on binding");
    }

    listen(server_socket, 5);

    char server_name[] = "enc_server";
    int debug_mode = 0;
    setup_dialog(server_name, debug_mode);

    while (1) {
        await_next_connection(server_socket);
    }

    close(server_socket);
    return 0;
}