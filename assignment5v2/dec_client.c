/*********************************************************************** 
** Program Filename: dec_client.c
** Author: Troy Diaz
** Date: 12/07/24
** Description: 
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include "dialog.c"

void error(const char *msg);
void setupAddressStruct(struct sockaddr_in *address, int portNumber, char *hostname);
char* getFileContents(char* filename);
int main(int argc, char *argv[]);

/********************************************************************* 
** Function: error
** Description: 
** Parameters: const char *msg - error message string
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
void error(const char *msg) {
    fprintf(stderr, "%s", msg);
    exit(0);
}

/********************************************************************* 
** Function: 
** Description: 
** Parameters: 
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
void setupAddressStruct(struct sockaddr_in *address, int portNumber, char *hostname) {
    memset((char *)address, '\0', sizeof(*address));
    address->sin_family = AF_INET;
    address->sin_port = htons(portNumber);

    struct hostent *hostInfo = gethostbyname(hostname);
    if (hostInfo == NULL) {
        fprintf(stderr, "DEC_CLIENT: Error: Host not found\n");
        exit(0);
    }
    memcpy((char *)&address->sin_addr.s_addr, hostInfo->h_addr_list[0], hostInfo->h_length);
}

/********************************************************************* 
** Function: 
** Description: 
** Parameters: 
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
char* getFileContents(char* filename) {
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "DEC_CLIENT: Error: Unable to open message file '%s'", filename);
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* string = malloc(fsize + 1);
    if (string == NULL) {
        fprintf(stderr, "DEC_CLIENT: Error: Memory allocation failed");
        exit(1);
    }

    fread(string, fsize, 1, f);
    fclose(f);
    
    string[fsize] = '\0';

    return string;
}

/********************************************************************* 
** Function: 
** Description: 
** Parameters: 
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
int main(int argc, char *argv[]) {
    char* messageFileName = argv[1];
    char* message = getFileContents(messageFileName);
    message[strcspn(message,"\r\n")] = '\0';

    char* keyFileName = argv[2];
    char* key = getFileContents(keyFileName);

    if (strlen(message) > strlen(key)) {
        fprintf(stderr, "DEC_CLIENT: Error: Key too short\n");
        exit(1);
    }

    char node_name[] = "dec_client";
    int debug = 0;
    setup_dialog(node_name, debug);

    int socketFD, portNumber, charsWritten, charsRead;
    struct sockaddr_in serverAddress;
    char buffer[256];

    if (argc < 4) {
        fprintf(stderr, "DEC_CLIENT: USAGE: %s hostname port\n", argv[0]);
        exit(0);
    }

    int port = atoi(argv[3]);
    char hostname[] = "localhost";

    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD < 0) {
        error("DEC_CLIENT: Error: Socket open failed");
    }

    setupAddressStruct(&serverAddress, port, hostname);

    if (connect(socketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        error("DEC_CLIENT: Error: Connection failed");
    }

    await_send_message(socketFD, "dec_client hello");
    char* response = await_receive_message(socketFD);
    if (strcmp(response, "dec_server hello") != 0) {
        fprintf(stderr,"DEC_CLIENT: Error: Invalid decryption server. Response: %s\n",response);
        usleep(100000);
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