/*********************************************************************** 
** Program Filename: dialog.c
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

#define FLUSH_DELAY 500000

int dialog_debug = 1;
char* node_name = NULL;
int last_message_sent = 0;

void setup_dialog(char* name, int debug_flag);
void constant_flush();
void await_send(int socket_fd, char* msg);
void await_send_message(int socket_fd, char* msg);
char* await_receive(int socket_fd, char *recv_buffer, int buffer_len);
char* await_receive_message(int socket_fd);

/********************************************************************* 
** Function: setup_dialog
** Description: 
** Parameters: char* name - node name, 
**             int _dialog_debug - debug flag
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
void setup_dialog(char* name, int debug_flag) {
    node_name = name;
    dialog_debug = debug_flag;
}

/********************************************************************* 
** Function: constant_flush
** Description: 
** Parameters: None
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
void constant_flush() {
    usleep(FLUSH_DELAY);
}

/********************************************************************* 
** Function: await_send
** Description: 
** Parameters: int socket_fd - socket file descriptor, 
**             char* msg - message to send
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
void await_send(int connection_socket, char *message) {
    int messageLength = strlen(message);
    int charsWritten = send(connection_socket, message, messageLength, 0);
    if (charsWritten < 0) {
        perror("Error sending message");
        exit(1);  
    }
    if (charsWritten < messageLength) {
        printf("Warning: Incomplete message sent\n");
    }
}


/********************************************************************* 
** Function: await_send_message
** Description: 
** Parameters: int socket_fd - socket file descriptor, 
**             char* msg - message to send
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
void await_send_message(int socket_fd, char* msg) {
    char header[256];
    memset(header, '\0', sizeof(header));
    sprintf(header, "%s|%d", node_name, (int)strlen(msg));
    await_send(socket_fd, header);
    constant_flush();
    await_send(socket_fd, msg);
    constant_flush();
    usleep(FLUSH_DELAY + strlen(msg) * 2);
}

/********************************************************************* 
** Function: await_receive
** Description: 
** Parameters: int socket_fd - socket file descriptor, 
**             char* recv_buffer - buffer for message,
**             int buffer_len - size of the buffer
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
char* await_receive(int connection_socket, char *buffer, int buffer_size) {
    if (buffer == NULL) {
        buffer = malloc(buffer_size);
    }

    int charsRead = recv(connection_socket, buffer, buffer_size - 1, 0);
    if (charsRead < 0) {
        perror("Error receiving message");
        exit(1);  
    }
    if (charsRead == 0) {
        return NULL;  
    }

    buffer[charsRead] = '\0';  
    return buffer;
}

/********************************************************************* 
** Function: await_receive_message
** Description: 
** Parameters: int socket_fd - socket file descriptor
**
** Pre-Conditions: 
** Post-Conditions: 
*********************************************************************/
char* await_receive_message(int socket_fd) {
    int max_header_size = 256;
    char* header_data = await_receive(socket_fd, NULL, max_header_size);
    int delimiter_idx = strcspn(header_data, "|");
    int payload_size = atoi(header_data + delimiter_idx + 1);
    constant_flush();
    usleep(FLUSH_DELAY + payload_size * 2);
    char* payload_data = await_receive(socket_fd, NULL, payload_size + 1);
    if (dialog_debug) { printf("[msg]: %s\n", payload_data); }
    constant_flush();
    return payload_data;
}
