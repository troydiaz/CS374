/*********************************************************************** 
** Program Filename: dec_client.c
** Author: Troy Diaz
** Date: 
** Description: 
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include "dialog.c"

void error(const char *errorMsg);
void setupAddressStruct(struct sockaddr_in *sockAddr, int portNum, char *hostName);
char* getFileContents(char* filePath);
int main(int argumentCount, char *argumentVector[]);

/********************************************************************* 
** Function: error
** Description: 
** Parameters: const char *msg - string
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
void error(const char *errorMsg) {
    fprintf(stderr, "%s\n", errorMsg);
    exit(1);
}

/********************************************************************* 
** Function: setupAddressStruct
** Description: 
** Parameters: struct sockaddr_in *address - pointer to address structure,
**             int portNumber - port number to connect to,
**             char *hostname - hostname of the server
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
void setupAddressStruct(struct sockaddr_in *sockAddr, int portNum, char *hostName) {
    memset((char *)sockAddr, '\0', sizeof(*sockAddr));
    sockAddr->sin_family = AF_INET;
    sockAddr->sin_port = htons(portNum);
    struct hostent *hostInformation = gethostbyname(hostName);
    if (hostInformation == NULL) {
        error("No such host");
    }
    memcpy((char *)&sockAddr->sin_addr.s_addr, hostInformation->h_addr_list[0], hostInformation->h_length);
}

/********************************************************************* 
** Function: getFileContents
** Description: 
** Parameters: char* filename - file name to read from
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
char* getFileContents(char* filePath) {
    FILE* filePointer = fopen(filePath, "r");
    if (filePointer == NULL) {
        fprintf(stderr, "Error opening file '%s'\n", filePath);
        exit(1);
    }

    fseek(filePointer, 0, SEEK_END);
    long fileSize = ftell(filePointer);
    fseek(filePointer, 0, SEEK_SET);

    char* fileContent = malloc(fileSize + 1);
    if (fileContent == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }

    fread(fileContent, fileSize, 1, filePointer);
    fclose(filePointer);

    fileContent[fileSize] = '\0';

    return fileContent;
}

/********************************************************************* 
** Function: main
** Description: 
** Parameters: int argc - number of arguments,
**             char *argv[] - array of arguments
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
int main(int argumentCount, char *argumentVector[]) {
    if (argumentCount < 4) {
        error("Usage: <program> <message_file> <key_file> <hostname> <port>");
    }

    char* messageFile = argumentVector[1];
    char* messageContent = getFileContents(messageFile);
    messageContent[strcspn(messageContent, "\r\n")] = '\0';

    char* keyFile = argumentVector[2];
    char* keyContent = getFileContents(keyFile);

    if (strlen(messageContent) > strlen(keyContent)) {
        error("Key is too short for the message");
    }

    char nodeName[] = "dec_client";
    setup_dialog(nodeName, 0);

    int clientSocket;
    struct sockaddr_in serverSockAddr;
    int serverPort = atoi(argumentVector[3]);
    char* serverHost = argumentVector[4]; 

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        error("Error opening socket");
    }

    setupAddressStruct(&serverSockAddr, serverPort, serverHost);

    if (connect(clientSocket, (struct sockaddr *)&serverSockAddr, sizeof(serverSockAddr)) < 0) {
        error("Error connecting to server");
    }

    await_send_message(clientSocket, "dec_client hello");
    char* serverResponse = await_receive_message(clientSocket);
    if (strcmp(serverResponse, "dec_server hello") != 0) {
        fprintf(stderr, "Error: Decryption server validation failed\n");
        exit(1);
    }

    await_send_message(clientSocket, messageContent);
    await_send_message(clientSocket, keyContent);

    usleep(FLUSH_DELAY + strlen(messageContent) * 2);
    char* plainTextMessage = await_receive_message(clientSocket);

    printf("%s\n", plainTextMessage);

    close(clientSocket);

    return 0;
}
