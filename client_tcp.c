#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MSG_MAX_SIZE 1024

int main(int argc, char* argv[])
{
    if (argc < 3) {
        printf("Input: ./<name_of_program> port host\n");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));

    // Create socket:
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket < 0) {
        printf("Unable to create socket\n");
        return -1;
    }

    // Set port and IP the same as server-side:
    sin.sin_family = AF_INET;
    sin.sin_port = htons(atoi(argv[1]));
    sin.sin_addr.s_addr = inet_addr(argv[2]);

    // Send connection request to server:
    if (connect(socket_fd, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
        printf("Unable to connect\n");
        return -1;
    }

    char messageTx[MSG_MAX_SIZE];
    memset(messageTx, 0, sizeof(messageTx));

    // Get input from the user:
    snprintf(messageTx, MSG_MAX_SIZE, "hello from the client!");

    // Send the message to server:
    if (send(socket_fd, messageTx, strlen(messageTx), 0) < 0) {
        printf("Unable to send message\n");
        return -1;
    }

    char messageRx[MSG_MAX_SIZE];
    memset(messageRx, 0, sizeof(messageRx));

    // Receive the server's response:
    if (recv(socket_fd, messageRx, sizeof(messageRx), 0) < 0) {
        printf("Error while receiving server's msg\n");
        return -1;
    }

    printf("Server's response: %s\n", messageRx);

    // Close the socket:
    close(socket_fd);

    return 0;
}
