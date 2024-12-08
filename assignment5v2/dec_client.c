/*********************************************************************** 
** Program Filename: dec_client.c
** Author: Troy Diaz
** Date: 12/07/24
** Description: This program sends a ciphertext and key to the DEC_SERVER,
**              and receives a plaintext as a response.
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
** Description: Outputs an error message and exits.
** Parameters: const char *msg - error message string
** Pre-Conditions: String is passed.
** Post-Conditions: Program terminates.
*********************************************************************/
void error(const char *msg) {
    fprintf(stderr, "%s", msg);
    exit(0);
}

/********************************************************************* 
** Function: setupAddressStruct
** Description: Initializes the IP address, port, and connection for sockaddr_in
** Parameters: struct sockaddr_in *address, int portNumber, char *hostname
** Pre-Conditions: Hostname and port number is given.
** Post-Conditions: sockaddr_in struct is created for connection to server.
*********************************************************************/
void setupAddressStruct(struct sockaddr_in *address, int portNumber, char *hostname) {
    memset((char *)address, '\0', sizeof(*address));

    // set address to IPv4
    address->sin_family = AF_INET; 

    // Convert port number to network byte order
    address->sin_port = htons(portNumber); 

    // Get host information
    struct hostent *hostInfo = gethostbyname(hostname);
    if (hostInfo == NULL) {
        fprintf(stderr, "DEC_CLIENT: Error: Host not found\n");
        exit(0);
    }

    // Copy host address to sockaddr_in address
    memcpy((char *)&address->sin_addr.s_addr, hostInfo->h_addr_list[0], hostInfo->h_length);
}

/********************************************************************* 
** Function: getFileContents
** Description: Reads a file and returns a string.
** Parameters: char* filename
** Pre-Conditions: File exists.
** Post-Conditions: Returns dynamic allocated string.
*********************************************************************/
char* getFileContents(char* filename) {
    // Open file for read only
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "DEC_CLIENT: Error: Unable to open message file '%s'", filename);
        exit(1);
    }

    // Move poointer to end of file
    fseek(f, 0, SEEK_END);

    // Get file size
    long fsize = ftell(f);

    // Move pointer to start of file
    fseek(f, 0, SEEK_SET);

    // Allocate memory for file
    char* string = malloc(fsize + 1);
    if (string == NULL) {
        fprintf(stderr, "DEC_CLIENT: Error: Memory allocation failed");
        exit(1);
    }

    // Read file into string
    fread(string, fsize, 1, f);
    fclose(f);
    
    // Null terminate string
    string[fsize] = '\0';

    return string;
}

/********************************************************************* 
** Function: main
** Description: Reads ciphertext and key files. Connects to DEC_SERVER and receives
**              messages for decryption. Plaintext is then printed.
** Parameters: int argc, char *argv[]
** Pre-Conditions: Command line is given.
** Post-Conditions: Outputs decrypted plaintext.
*********************************************************************/
int main(int argc, char *argv[]) {
    // Receive file name, file contents, and remove newline character
    char* messageFileName = argv[1];
    char* message = getFileContents(messageFileName);
    message[strcspn(message, "\r\n")] = '\0';

    // Receieve key file name and file contents
    char* keyFileName = argv[2];
    char* key = getFileContents(keyFileName);

    // Check for key length
    if (strlen(message) > strlen(key)) {
        fprintf(stderr, "DEC_CLIENT: Error: Key too short\n");
        exit(1);
    }
    
    // Set debug mode
    char node_name[] = "dec_client";
    int debug = 0;
    setup_dialog(node_name, debug);

    // Setup socket address
    int socketFD, portNumber, charsWritten, charsRead;
    struct sockaddr_in serverAddress;
    char buffer[256];

    // Validate arguments
    if (argc < 4) {
        fprintf(stderr, "DEC_CLIENT: USAGE: %s hostname port\n", argv[0]);
        exit(0);
    }  

    // Convert port number to int and set hostname
    int port = atoi(argv[3]);
    char hostname[] = "localhost";

    // Create TCP socket for connection
    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD < 0) {
        error("DEC_CLIENT: Error: Socket open failed");
    }

    // Connect to server
    setupAddressStruct(&serverAddress, port, hostname);
    if (connect(socketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        error("DEC_CLIENT: Error: Connection failed");
    }

    // Verify server details
    await_send_message(socketFD, "dec_client hello");
    char* response = await_receive_message(socketFD);
    if (strcmp(response, "dec_server hello") != 0) {
        fprintf(stderr, "DEC_CLIENT: Error: Invalid decryption server. Response: %s\n", response);
        usleep(100000);
        exit(1);
    }

    // Send ciphertext to server
    await_send_message(socketFD, message);
    await_send_message(socketFD, key);

    // Delay for message process
    usleep(FLUSH_DELAY + strlen(message) * 2);
    char* plaintext = await_receive_message(socketFD);

    // Print plaintext and close TCP connection
    printf("%s\n", plaintext);
    close(socketFD);

    return 0;
}
