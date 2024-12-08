/*********************************************************************** 
** Program Filename: enc_server.c
** Author: Troy Diaz
** Date: 12/07/24
** Description: Listens for incoming client connections, receives plaintext and keys,
**              encrypts plaintext using ONE TIME PAD and returns ciphertext to client.
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
** Description: Outputs an error message and exits.
** Parameters: const char *msg - error message string
** Pre-Conditions: String is passed.
** Post-Conditions: Program terminates.
*********************************************************************/
void error(const char *error_msg) {
    perror(error_msg);
    exit(1);
}

/********************************************************************* 
** Function: setupAddressStruct
** Description: Initializes server address.
** Parameters: struct sockaddr_in *server_addr - server address structure,
**             int port_number - port to bind the server
** Pre-Conditions: Port number is given.
** Post-Conditions: Server address structure is filled.
*********************************************************************/
void setupAddressStruct(struct sockaddr_in *server_addr, int port_number) {
    memset((char *)server_addr, '\0', sizeof(*server_addr));
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(port_number);
    server_addr->sin_addr.s_addr = INADDR_ANY;
}

/********************************************************************* 
** Function: await_next_connection
** Description: Waits for next client connection. Child processes handles multiple
**              connections.
** Parameters: int server_socket - listening socket for server
** Pre-Conditions: Client connection is made.
** Post-Conditions: Child process is created for client connection.
*********************************************************************/
int await_next_connection(int server_socket) {
    int client_socket;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Accept client connection
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_socket < 0) {
        error("ENC_SERVER: ERROR on accept");
    }

    // Fork child processes for connection
    pid_t child_pid = fork();
    if (child_pid == -1) {
        perror("ENC_SERVER: forking failed");
        exit(1);
    } else if (child_pid == 0) {
        printf("ENC_SERVER: Child process started.\n");
        handle_connection(client_socket);
        printf("ENC_SERVER: Child process closed.\n");
        exit(0);
    }

    return child_pid;
}

/********************************************************************* 
** Function: handle_connection
** Description: Handles communication and encryption between client.
** Parameters: int client_socket - socket for client connection
** Pre-Conditions: Client socket is connected.
** Post-Conditions: Client socket is closed
*********************************************************************/
void handle_connection(int client_socket) {
    dialog(client_socket);
    close(client_socket);
}

/********************************************************************* 
** Function: dialog
** Description: Receives plaintext and key from client, encrypts plaintext, sends
**              ciphertext back to client.
** Parameters: int client_socket - socket for client connection
** Pre-Conditions: Client socket is connected.
** Post-Conditions: Client socket is closed.
*********************************************************************/
void dialog(int client_socket) {
    // Ensure client is connected
    char* client_greeting = await_receive_message(client_socket);
    if (strcmp(client_greeting, "enc_client hello") != 0) {
        fprintf(stderr, "ENC_SERVER: Client did not say hello. Closing connection.\n");
        await_send_message(client_socket, "enc_server hello");
        usleep(100000);
        close(client_socket);
        exit(1);
    }
    printf("ENC_SERVER: Client said hello.\n");
    await_send_message(client_socket, "enc_server hello");

    // Receive plaintext and key from client
    char* received_plaintext = await_receive_message(client_socket);
    char* received_key = await_receive_message(client_socket);
    char* encrypted_text = encrypt_message(received_plaintext, received_key);

    printf("ENC_SERVER: plaintext len: %d, key len: %d, ciphertext len: %d\n", strlen(received_plaintext), strlen(received_key), strlen(encrypted_text));

    // Delay for message processing
    usleep(FLUSH_DELAY + strlen(encrypted_text) * 2);
    await_send_message(client_socket, encrypted_text);
}

/********************************************************************* 
** Function: main
** Description: Sets up server for client connection.
** Parameters: int argc - number of arguments,
**             char *argv[] - argument list
** Pre-Conditions: Port number is given.
** Post-Conditions: Handles client connections.
*********************************************************************/
int main(int argc, char *argv[]) {
    int server_socket;
    struct sockaddr_in server_config;

    if (argc < 2) {
        fprintf(stderr, "ENC_SERVER: USAGE: %s port\n", argv[0]);
        exit(1);
    }

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        error("ENC_SERVER: ERROR opening socket");
    }

    // Create server address
    setupAddressStruct(&server_config, atoi(argv[1]));

    if (bind(server_socket, (struct sockaddr *)&server_config, sizeof(server_config)) < 0) {
        error("ENC_SERVER: ERROR on binding");
    }

    // Listen for incoming connections
    listen(server_socket, 5);

    // Setup dialog system
    char server_name[] = "enc_server";
    int debug_mode = 0;
    setup_dialog(server_name, debug_mode);

    // Wait for client connections
    while (1) {
        await_next_connection(server_socket);
    }

    close(server_socket);
    
    return 0;
}
