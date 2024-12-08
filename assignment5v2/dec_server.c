/*********************************************************************** 
** Program Filename: dec_server.c
** Author: Troy Diaz
** Date: 12/07/24
** Description: Receieves encrypted messages from DEC_CLIENT and decrypts using
**              ONE TIME PAD and sends back plaintext.
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

void error(const char *errorMsg);
void setupAddressStruct(struct sockaddr_in *sockAddr, int portNum);
void handle_connection(int clientSocket);
int await_next_connection(int listeningSocket);
void dialog(int clientSocket);
int main(int argCount, char *argValues[]);

/********************************************************************* 
** Function: error
** Description: Outputs an error message and exits.
** Parameters: const char *msg - error message string
** Pre-Conditions: String is passed.
** Post-Conditions: Program terminates.
*********************************************************************/
void error(const char *errorMsg) {
    perror(errorMsg);
    exit(1);
}

/********************************************************************* 
** Function: setupAddressStruct
** Description: Set up port, IP and socket family for sockaddr_in struct.
** Parameters: struct sockaddr_in *address - pointer to address struct,
**             int portNumber - port to bind
** Pre-Conditions: Receieves a valid port number.
** Post-Conditions: Address is filled.
*********************************************************************/
void setupAddressStruct(struct sockaddr_in *sockAddr, int portNum) {
    memset((char *)sockAddr, '\0', sizeof(*sockAddr));
    
    // IPv4
    sockAddr->sin_family = AF_INET;

    // Convert to network bytes
    sockAddr->sin_port = htons(portNum);

    // Allow for any incoming IP addresses
    sockAddr->sin_addr.s_addr = INADDR_ANY;
}

/********************************************************************* 
** Function: await_next_connection
** Description: Waits for incoming client connection. Child process is created
**              for each connection.
** Parameters: int listen_socket - socket file descriptor for listening
** Pre-Conditions: Socket is in listening state.
** Post-Conditions: Connection is established and child process is created.
*********************************************************************/
int await_next_connection(int listeningSocket) {
    
    // Accept incoming connection from client
    int clientSocket;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    clientSocket = accept(listeningSocket, (struct sockaddr *)&clientAddr, &clientAddrSize);
    if (clientSocket < 0) {
        error("DEC_SERVER: ERROR on accept");
    }

    // Create child process for connection
    pid_t childPid = fork();
    if (childPid == -1) {
        perror("DEC_SERVER: fork failed");
        exit(1);
    } else if (childPid == 0) {
        // Handle client connection
        printf("DEC_SERVER: Child process started.\n");
        handle_connection(clientSocket);
        printf("DEC_SERVER: Child process closed.\n");
        exit(0);
    }

    return childPid;
}

/********************************************************************* 
** Function: handle_connection
** Description: Handles client connection.
** Parameters: int connection_socket - socket file descriptor for client connection
** Pre-Conditions: Client socket is connected.
** Post-Conditions: Client socket is closed.
*********************************************************************/
void handle_connection(int clientSocket) {
    dialog(clientSocket);
    close(clientSocket);
}

/********************************************************************* 
** Function: dialog
** Description: Server verifies client, receives encrypted text and decryptionkey, decrypts and 
**              returns decrypted plaintext.
** Parameters: int connection_socket - socket file descriptor for client connection
** Pre-Conditions: Client socket is connected.
** Post-Conditions: Client socket is closed.
*********************************************************************/
void dialog(int clientSocket) {
    char* clientGreeting = await_receive_message(clientSocket);
    if (strcmp(clientGreeting, "dec_client hello") != 0) {
        fprintf(stderr, "DEC_SERVER: Client didn't say hello. Closing connection.\n");
        await_send_message(clientSocket, "dec_server hello");
        usleep(100000);
        close(clientSocket);
        exit(1);
    }
    await_send_message(clientSocket, "dec_server hello");

    char* encryptedText = await_receive_message(clientSocket);
    char* decryptionKey = await_receive_message(clientSocket);
    char* decryptedText = decrypt_message(encryptedText, decryptionKey);

    printf("DEC_SERVER: Decrypted message: %d/%d/%d\n", strlen(decryptedText), strlen(decryptionKey), strlen(encryptedText));

    usleep(FLUSH_DELAY + strlen(decryptedText) * 2);
    await_send_message(clientSocket, decryptedText);
}

/********************************************************************* 
** Function: main
** Description: Starts the decryyption server and listens for incoming client
**              connections. 
** Parameters: int argc - number of arguments,
**             char *argv[] - array of arguments
** Pre-Conditions: Port number is given.
** Post-Conditions: Connection is handled by child processes.
*********************************************************************/
int main(int argCount, char *argValues[]) {
    if (argCount < 2) {
        fprintf(stderr, "USAGE: %s port\n", argValues[0]);
        exit(1);
    }

    int listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listeningSocket < 0) {
        error("DEC_SERVER: ERROR opening socket");
    }

    struct sockaddr_in serverSockAddr;
    setupAddressStruct(&serverSockAddr, atoi(argValues[1]));

    if (bind(listeningSocket, (struct sockaddr *)&serverSockAddr, sizeof(serverSockAddr)) < 0) {
        error("DEC_SERVER: ERROR on binding");
    }

    listen(listeningSocket, 5);

    setup_dialog("dec_server", 0);

    while (1) {
        await_next_connection(listeningSocket);
    }

    close(listeningSocket);
    return 0;
}
