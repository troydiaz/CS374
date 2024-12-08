/*********************************************************************** 
** Program Filename: enc_client.c
** Author: Troy Diaz
** Date: 12/07/24
** Description: Reads plaintext message and key, connects to encryption server,
**              sends message and key for encryption, and receives encrypted message.
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include "dialog.c"

void error(const char *error_msg);
void setupAddressStruct(struct sockaddr_in *server_address, int port, char *host_name);
char* getFileContents(char* file_name);
int main(int argument_count, char *argument_values[]);

/********************************************************************* 
** Function: error
** Description: Outputs an error message and exits.
** Parameters: const char *msg - error message string
** Pre-Conditions: String is passed.
** Post-Conditions: Program terminates.
*********************************************************************/
void error(const char *error_msg) {
    fprintf(stderr, "%s", error_msg);
    exit(1);
}

/********************************************************************* 
** Function: setupAddressStruct
** Description: Initialize server address for connection.
** Parameters: struct sockaddr_in *server_address - server address structure,
**             int port - port to connect to,
**             char *host_name - hostname of the server
** Pre-Conditions: Port number, host name, and server address is given.
** Post-Conditions: Server address is filled.
*********************************************************************/
void setupAddressStruct(struct sockaddr_in *server_address, int port, char *host_name) {
    memset((char *)server_address, '\0', sizeof(*server_address));
    server_address->sin_family = AF_INET;
    server_address->sin_port = htons(port);
    struct hostent *host_info = gethostbyname(host_name);
    if (host_info == NULL) {
        fprintf(stderr, "ENC_CLIENT: ERROR: no such host\n");
        exit(1);
    }
    memcpy((char *)&server_address->sin_addr.s_addr, host_info->h_addr_list[0], host_info->h_length);
}

/********************************************************************* 
** Function: getFileContents
** Description: Reads file contents and returns it as a string.
** Parameters: char *file_name - name of the file to read
** Pre-Conditions: File exists.
** Post-Conditions: Returns a pointer to a string of the file.
*********************************************************************/
char* getFileContents(char* file_name) {
    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        fprintf(stderr, "ENC_CLIENT: ERROR opening file: %s\n", file_name);
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* content = malloc(file_size + 1);
    if (content == NULL) {
        fprintf(stderr, "ENC_CLIENT: ERROR allocating memory\n");
        exit(1);
    }

    fread(content, file_size, 1, file);
    fclose(file);

    content[file_size] = '\0';

    return content;
}

/********************************************************************* 
** Function: main
** Description: Reads plaintext and key, connects to server, sends plaintext
**              and key for encryption. Prints encrypted message from server.
** Parameters: int argument_count - number of arguments,
**             char *argument_values[] - argument list
** Pre-Conditions: Command line arguments are provided for plaintext file, key file, and port
**                 number.
** Post-Conditions: Client socket is closed.
*********************************************************************/
int main(int argument_count, char *argument_values[]) {
    
    // Read message and key from files
    char* message_file_name = argument_values[1];
    char* message_content = getFileContents(message_file_name);
    message_content[strcspn(message_content, "\r\n")] = '\0';

    char* key_file_name = argument_values[2];
    char* key_content = getFileContents(key_file_name);

    // Check length
    if (strlen(message_content) > strlen(key_content)) {
        fprintf(stderr, "ENC_CLIENT: ERROR: The key is too short.\n");
        exit(1);
    }

    // Setup dialog
    char node_identifier[] = "enc_client";
    int debug_mode = 0;
    setup_dialog(node_identifier, debug_mode);

    // Setup connection to server
    int client_socket, port_number;
    struct sockaddr_in server_config;
    char temporary_buffer[256];

    if (argument_count < 4) {
        fprintf(stderr, "ENC_CLIENT: USAGE: %s hostname port\n", argument_values[0]);
        exit(1);
    }

    int server_port = atoi(argument_values[3]);
    char server_hostname[] = "localhost";

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        error("ENC_CLIENT: ERROR opening socket");
    }

    setupAddressStruct(&server_config, server_port, server_hostname);

    if (connect(client_socket, (struct sockaddr *)&server_config, sizeof(server_config)) < 0) {
        error("ENC_CLIENT: ERROR connecting");
    }

    // Ensure connection
    await_send_message(client_socket, "enc_client hello");
    char* server_response = await_receive_message(client_socket);
    if (strcmp(server_response, "enc_server hello") != 0) {
        fprintf(stderr, "ENC_CLIENT: ERROR: Server validation failed. Response: %s\n", server_response);
        usleep(100000);
        exit(1);
    }

    // Send message and key to server
    await_send_message(client_socket, message_content);
    await_send_message(client_socket, key_content);

    // Delay for message processing
    usleep(FLUSH_DELAY + strlen(message_content) * 2);
    char* encrypted_message = await_receive_message(client_socket);

    // Print encrypted message to stdout
    printf("%s\n", encrypted_message);

    // Close socket
    close(client_socket);

    return 0;
}
