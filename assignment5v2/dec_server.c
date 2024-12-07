/*********************************************************************** 
** Program Filename: dec_server.c
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

void error(const char *errorMsg);
void setupAddressStruct(struct sockaddr_in *sockAddr, int portNum);
void handle_connection(int clientSocket);
int await_next_connection(int listeningSocket);
void dialog(int clientSocket);
int main(int argCount, char *argValues[]);

/********************************************************************* 
** Function: error
** Description: 
** Parameters: const char *msg - error message string
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
void error(const char *errorMsg) {
    perror(errorMsg);
    exit(1);
}

/********************************************************************* 
** Function: setupAddressStruct
** Description: 
** Parameters: struct sockaddr_in *address - pointer to address struct,
**             int portNumber - port to bind
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
void setupAddressStruct(struct sockaddr_in *sockAddr, int portNum) {
    memset((char *)sockAddr, '\0', sizeof(*sockAddr));
    sockAddr->sin_family = AF_INET;
    sockAddr->sin_port = htons(portNum);
    sockAddr->sin_addr.s_addr = INADDR_ANY;
}

/********************************************************************* 
** Function: await_next_connection
** Description: 
** Parameters: int listen_socket - socket file descriptor for listening
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
int await_next_connection(int listeningSocket) {
    int clientSocket;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    clientSocket = accept(listeningSocket, (struct sockaddr *)&clientAddr, &clientAddrSize);
    if (clientSocket < 0) {
        error("ERROR on accept");
    }
    pid_t childPid = fork();
    if (childPid == -1) {
        perror("fork failed");
        exit(1);
    } else if (childPid == 0) {
        printf("[dec_server]: Child process started.\n");
        handle_connection(clientSocket);
        printf("[dec_server]: Child process closed.\n");
        exit(0);
    }
    return childPid;
}

/********************************************************************* 
** Function: handle_connection
** Description: 
** Parameters: int connection_socket - socket file descriptor for client connection
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
void handle_connection(int clientSocket) {
    dialog(clientSocket);
    close(clientSocket);
}

/********************************************************************* 
** Function: dialog
** Description: 
** Parameters: int connection_socket - socket file descriptor for client connection
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
void dialog(int clientSocket) {
    char* clientGreeting = await_receive_message(clientSocket);
    if (strcmp(clientGreeting, "dec_client hello") != 0) {
        fprintf(stderr, "[dec_server]: Client didn't say hello. Closing connection.\n");
        await_send_message(clientSocket, "dec_server hello");
        usleep(100000);
        close(clientSocket);
        exit(1);
    }
    await_send_message(clientSocket, "dec_server hello");

    char* encryptedText = await_receive_message(clientSocket);
    char* decryptionKey = await_receive_message(clientSocket);
    char* decryptedText = decrypt_message(encryptedText, decryptionKey);

    printf("[dec_server]: Decrypted message: %d/%d/%d\n", strlen(decryptedText), strlen(decryptionKey), strlen(encryptedText));

    usleep(FLUSH_DELAY + strlen(decryptedText) * 2);
    await_send_message(clientSocket, decryptedText);
}

/********************************************************************* 
** Function: main
** Description: 
** Parameters: int argc - number of arguments,
**             char *argv[] - array of arguments
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
int main(int argCount, char *argValues[]) {
    if (argCount < 2) {
        fprintf(stderr, "USAGE: %s port\n", argValues[0]);
        exit(1);
    }

    int listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listeningSocket < 0) {
        error("ERROR opening socket");
    }

    struct sockaddr_in serverSockAddr;
    setupAddressStruct(&serverSockAddr, atoi(argValues[1]));

    if (bind(listeningSocket, (struct sockaddr *)&serverSockAddr, sizeof(serverSockAddr)) < 0) {
        error("ERROR on binding");
    }

    listen(listeningSocket, 5);

    setup_dialog("dec_server", 0);

    while (1) {
        await_next_connection(listeningSocket);
    }

    close(listeningSocket);
    return 0;
}
