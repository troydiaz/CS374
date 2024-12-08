/*********************************************************************** 
** Program Filename: dialog.c
** Author: Troy Diaz
** Date: 12/07/24
** Description: Handles socket exchange between client and server.
*********************************************************************/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

// Set delay for message processing
#define FLUSH_DELAY 500000

// Debug mode
int dialog_debug = 1;

// Node for dialog system
char* node_name = NULL;

// Last message state
int last_message_sent = 0;

void setup_dialog(char* name, int debug_flag);
void constant_flush();
void await_send(int socket_fd, char* msg);
void await_send_message(int socket_fd, char* msg);
char* await_receive(int socket_fd, char *recv_buffer, int buffer_len);
char* await_receive_message(int socket_fd);

/********************************************************************* 
** Function: setup_dialog
** Description: Sets dialog system with node name and debug flag.
** Parameters: char* name - node name, 
**             int debug_flag - debug flag
** Pre-Conditions: String is given.
** Post-Conditions: Global variables are set.
*********************************************************************/
void setup_dialog(char* name, int debug_flag) {
    node_name = name;
    dialog_debug = debug_flag;
}

/********************************************************************* 
** Function: constant_flush
** Description: Delay for socket buffer.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: Program sleeps.
*********************************************************************/
void constant_flush() {
    usleep(FLUSH_DELAY);
}

/********************************************************************* 
** Function: await_send
** Description: Sends message over socket connection.
** Parameters: int connection_socket - socket file descriptor, 
**             char* message - message to send
** Pre-Conditions: Socket is connected.
** Post-Conditions: Message is sent to socket connection.
*********************************************************************/
void await_send(int connection_socket, char *message) {
    int messageLength = strlen(message);

    // Send message
    int charsWritten = send(connection_socket, message, messageLength, 0);

    // If error
    if (charsWritten < 0) {
        perror("DIALOG: Error sending message");
        exit(1);  
    }
    if (charsWritten < messageLength) {
        printf("DIALOG: Warning: Incomplete message sent\n");
    }
}

/********************************************************************* 
** Function: await_send_message
** Description: Sends a message with node name and message contents.
** Parameters: int socket_fd - socket file descriptor, 
**             char* msg - message to send
** Pre-Conditions: Socket is connected.
** Post-Conditions: Message is sent.
*********************************************************************/
void await_send_message(int socket_fd, char* msg) {
    char header[256];
    memset(header, '\0', sizeof(header));

    // Create message header
    sprintf(header, "%s|%d", node_name, (int)strlen(msg));

    // Send header
    await_send(socket_fd, header);
    constant_flush();

    // Send message
    await_send(socket_fd, msg);
    constant_flush();

    // Wait until message is sent completely
    usleep(FLUSH_DELAY + strlen(msg) * 2);
}

/********************************************************************* 
** Function: await_receive
** Description: Receives data from socket and stores it in buffer.
** Parameters: int connection_socket - socket file descriptor, 
**             char* buffer - buffer for message,
**             int buffer_size - size of the buffer
** Pre-Conditions: Socket is connected.
** Post-Conditions: Buffer has received the message and returns a pointer to it.
*********************************************************************/
char* await_receive(int connection_socket, char *buffer, int buffer_size) {
    if (buffer == NULL) {
        buffer = malloc(buffer_size);
    }

    int charsRead = recv(connection_socket, buffer, buffer_size - 1, 0);
    if (charsRead < 0) {
        perror("DIALOG: Error receiving message");
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
** Description: Receives message with header.
** Parameters: int socket_fd - socket file descriptor
** Pre-Conditions: Socket is connected.
** Post-Conditions: Returns message.
*********************************************************************/
char* await_receive_message(int socket_fd) {
    int max_header_size = 256;
    char* header_data = await_receive(socket_fd, NULL, max_header_size);
    
    // Extract message size
    int delimiter_idx = strcspn(header_data, "|");
    int payload_size = atoi(header_data + delimiter_idx + 1);
    
    // Set delay for message processing
    constant_flush();
    usleep(FLUSH_DELAY + payload_size * 2);
    
    // Receive message mody
    char* payload_data = await_receive(socket_fd, NULL, payload_size + 1);
    if (dialog_debug) { 
        printf("[msg]: %s\n", payload_data); 
    }
    constant_flush();
    
    return payload_data;
}
