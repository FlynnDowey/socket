#include "client.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MSG_MAX_SIZE 1024

static struct {
    int portNumber;
    char* ipAddr;
    int socket_fd;
} s_options;

static struct sockaddr_in sin;

void Client_init(int portNumber, char* ipAddr)
{
    s_options.ipAddr = ipAddr;
    s_options.portNumber = portNumber;
}
void Client_connectToServer(void)
{
    memset(&sin, 0, sizeof(sin));

    // Create socket:
    s_options.socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    //setsockopt(s_options.socket_fd, IPPROTO_TCP, TCP_NODELAY, 1, 4);

    if (s_options.socket_fd < 0) {
        perror("Unable to create socket\n");
        exit(EXIT_FAILURE);
    }

    // Set port and IP the same as server-side:
    sin.sin_family = AF_INET;
    sin.sin_port = htons(s_options.portNumber);
    sin.sin_addr.s_addr = inet_addr(s_options.ipAddr);

    // Send connection request to server:
    if (connect(s_options.socket_fd, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
        perror("Cannot connect to the server.\n");
        exit(EXIT_FAILURE);
    }
}
void Client_disconnectFromServer(void)
{
    close(s_options.socket_fd);
}

void Client_sendMessage(char* messageTx, int size)
{
    if (send(s_options.socket_fd, messageTx, size, 0) < 0) {
        perror("Unable to send message\n");
        exit(EXIT_FAILURE);
    }
}
char* Client_getMessage(long size)
{
    // first message from
    char* message = malloc(sizeof(char) * size);
    if (recv(s_options.socket_fd, message, size, 0) < 0) {
        perror("Error while receiving server's msg\n");
        exit(EXIT_FAILURE);
    }
    //fflush(s_options.socket_fd);
    message[size - 1] = '\0';
    return message;
}

long Client_getMessageSize(void)
{
    long size = 0;
    if (recv(s_options.socket_fd, &size, sizeof(long), 0) < 0) {
        perror("Error while receiving server's msg\n");
        exit(EXIT_FAILURE);
    }
    return size;
}
