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
char* _node_name = NULL;
int last_sent = 0;

void setup_dialog(char* name, int _dialog_debug) {
    _node_name = name;
    dialog_debug = _dialog_debug;
}

void constant_flush() {
    usleep(FLUSH_DELAY);
}

void await_send(int connection_socket, char* message) {
    int message_length = strlen(message);
    int chars_written = send(connection_socket, message, message_length, 0);
    if (chars_written < 0) {
        printf("ERROR sending message\n");
    } else if (dialog_debug) {
        printf("Sent: \"%s\"\n", message);
    }
    if (chars_written < message_length) {
        printf("WARNING: Incomplete message sent\n");
    }
}

void await_send_message(int connection_socket, char* message) {
    char header[256];
    memset(header, '\0', sizeof(header));
    sprintf(header, "%s|%d", _node_name, (int)strlen(message));
    await_send(connection_socket, header);
    constant_flush();
    await_send(connection_socket, message);
    constant_flush();
    usleep(FLUSH_DELAY + strlen(message) * 2);
}

char* await_receive(int connection_socket, char *buffer, int buffer_size) {
    if (buffer == NULL) {
        buffer = malloc(buffer_size);
    }
    memset(buffer, '\0', buffer_size);
    int chars_read = recv(connection_socket, buffer, buffer_size - 1, 0);
    if (chars_read < 0) {
        fprintf(stderr,"ERROR receiving message\n");
        exit(1);
    }
    last_sent = 0;
    if (dialog_debug) { printf("<- \"%s\"\n", buffer); }
    return buffer;
}

char* await_receive_message(int connection_socket) {
    int header_max_size = 256;
    char* header_buffer = await_receive(connection_socket, NULL, header_max_size);
    int bar_idx = strcspn(header_buffer, "|");
    int header_size = atoi(header_buffer + bar_idx + 1);
    constant_flush();
    usleep(FLUSH_DELAY + header_size * 2);
    char* message_buffer = await_receive(connection_socket, NULL, header_size + 1);
    if (dialog_debug) { printf("[msg]: %s\n", message_buffer); }
    constant_flush();
    return message_buffer;
}
