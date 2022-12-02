#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MSG_MAX_SIZE 1024

static struct {
    int portNumber;
    char* ipAddr;
    int server_fd, client_fd;
} s_options;

static struct sockaddr_in s_server;
static struct sockaddr_in s_client;

void Server_init(int portNumber, char* ipAddr)
{
    s_options.portNumber = portNumber;
    s_options.ipAddr = ipAddr;
    s_options.server_fd = -1;
    s_options.client_fd = -1;
}
void Server_cleanup(void)
{
    close(s_options.client_fd);
    close(s_options.server_fd);
}

char* Server_getMessage(void)
{
    char* messageRx = malloc(MSG_MAX_SIZE * sizeof(messageRx));
    // memset(messageRx, 0, MSG_MAX_SIZE);
    int bytesRx = recv(s_options.client_fd, messageRx, MSG_MAX_SIZE - 1, 0);
    if (bytesRx < 0) {
        perror("Error: cannot read message...\n");
        exit(EXIT_FAILURE);
    }
    return messageRx;
}
void Server_sendMessage(char* messageTx, int size)
{
    if (send(s_options.client_fd, messageTx, size, 0) < 0) {
        perror("Error: cannot send message...\n");
        exit(EXIT_FAILURE);
    }
}
void Server_sendMessageSize(long size)
{
    if (send(s_options.client_fd, &size, sizeof(long), 0) < 0) {
        perror("Error: cannot send message...\n");
        exit(EXIT_FAILURE);
    }
}

void Server_listen(void)
{
    memset(&s_server, 0, sizeof(s_server));

    s_options.server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (s_options.server_fd < 0) {
        perror("Error: cannot create socket...\n");
        exit(EXIT_FAILURE);
    }
    s_server.sin_family = AF_INET;
    s_server.sin_port = htons(s_options.portNumber);
    s_server.sin_addr.s_addr = inet_addr(s_options.ipAddr);

    if (bind(s_options.server_fd, (struct sockaddr*)&s_server, sizeof(s_server)) < 0) {
        perror("Error: Could not bind to the port...s\n");
        exit(EXIT_FAILURE);
    }

    if (listen(s_options.server_fd, 1) < 0) {
        perror("Error: cannot retreive message...\n");
        exit(EXIT_FAILURE);
    }

    int clientSize = sizeof(s_client);
    s_options.client_fd = accept(s_options.server_fd, (struct sockaddr*)&s_client, (unsigned int* restrict)&clientSize);
    if (s_options.client_fd < 0) {
        perror("Error: Could not accept...s\n");
        exit(EXIT_FAILURE);
    }
}
