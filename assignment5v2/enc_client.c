/*********************************************************************** 
** Program Filename: enc_client.c
** Author: Troy Diaz
** Date: 12/07/24
** Description: 
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
** Description: 
** Parameters: const char *error_msg - error message to display
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
void error(const char *error_msg) {
    fprintf(stderr, "%s", error_msg);
    exit(1);
}

/********************************************************************* 
** Function: setupAddressStruct
** Description: 
** Parameters: struct sockaddr_in *server_address - server address structure,
**             int port - port to connect to,
**             char *host_name - hostname of the server
**
** Pre-Conditions: 
** Post-Conditions: 
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
** Description: 
** Parameters: char *file_name - name of the file to read
**
** Pre-Conditions: 
** Post-Conditions: 
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
** Description: 
** Parameters: int argument_count - number of arguments,
**             char *argument_values[] - argument list
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
int main(int argument_count, char *argument_values[]) {
    char* message_file_name = argument_values[1];
    char* message_content = getFileContents(message_file_name);
    message_content[strcspn(message_content, "\r\n")] = '\0';

    char* key_file_name = argument_values[2];
    char* key_content = getFileContents(key_file_name);

    if (strlen(message_content) > strlen(key_content)) {
        fprintf(stderr, "ENC_CLIENT: ERROR: The key is too short.\n");
        exit(1);
    }

    char node_identifier[] = "enc_client";
    int debug_mode = 0;
    setup_dialog(node_identifier, debug_mode);

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

    await_send_message(client_socket, "enc_client hello");
    char* server_response = await_receive_message(client_socket);
    if (strcmp(server_response, "enc_server hello") != 0) {
        fprintf(stderr, "ENC_CLIENT: ERROR: Server validation failed. Response: %s\n", server_response);
        usleep(100000);
        exit(1);
    }

    await_send_message(client_socket, message_content);
    await_send_message(client_socket, key_content);

    usleep(FLUSH_DELAY + strlen(message_content) * 2);
    char* encrypted_message = await_receive_message(client_socket);

    printf("%s\n", encrypted_message);

    close(client_socket);

    return 0;
}
