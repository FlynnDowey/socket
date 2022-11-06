#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MSG_MAX_SIZE 1024
//"127.0.0.1"
int main(int argc, char* argv[])
{
    if (argc < 3) {
        printf("Input: ./<name_of_program> port host\n");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in s_server;
    memset(&s_server, 0, sizeof(s_server));

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Error: cannot create socket...\n");
        exit(EXIT_FAILURE);
    }
    s_server.sin_family = AF_INET;
    s_server.sin_port = htons(atoi(argv[1]));
    s_server.sin_addr.s_addr = inet_addr(argv[2]);

    if (bind(server_fd, (struct sockaddr*)&s_server, sizeof(s_server)) < 0) {
        perror("Error: Could not bind to the port...s\n");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 1) < 0) {
        perror("Error: cannot retreive message...\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr s_client;
    int clientSize = sizeof(s_client);
    int client_fd = accept(server_fd, (struct sockaddr*)&s_client, (unsigned int* restrict)&clientSize);
    if (client_fd < 0) {
        perror("Error: Could not accept...s\n");
        exit(EXIT_FAILURE);
    }

    char messageRx[MSG_MAX_SIZE];
    memset(messageRx, 0, sizeof(messageRx));
    int bytesRx = recv(client_fd, messageRx, sizeof(messageRx), 0);
    if (bytesRx < 0) {
        perror("Error: cannot read message...\n");
        exit(EXIT_FAILURE);
    }
    // int fd = open("/tmp/fifo", O_WRONLY);
    // write(fd, messageRx, strlen(messageRx) + 1);
    // close(fd);

    char messageTx[MSG_MAX_SIZE];
    memset(messageTx, 0, sizeof(messageTx));

    snprintf(messageTx, MSG_MAX_SIZE, "Hello from the server.\n");

    if (send(client_fd, messageTx, strlen(messageTx), 0) < 0) {
        perror("Error: cannot send message...\n");
        exit(EXIT_FAILURE);
    }

    close(client_fd);
    close(server_fd);
    return 0;
}
