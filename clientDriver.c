#include "client.h"
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define FIFO_NAME1 "/tmp/fifo1"
#define FIFO_NAME2 "/tmp/fifo2"
// static bool isDone = false;
#define MSG_MAX_LEN 1024

int main(int argc, char* argv[])
{
    Client_init(atoi(argv[1]), argv[2]);
    Client_connectToServer();

    int fd_2 = open(FIFO_NAME2, O_RDONLY);
    if (fd_2 < 0) {
        perror("client: Cannot open fifo\n");
        exit(EXIT_FAILURE);
    }
    char messageTx[MSG_MAX_LEN];
    if (read(fd_2, messageTx, sizeof(char) * MSG_MAX_LEN) < 0) {
        perror("client: Cannot read fifo\n");
        exit(EXIT_FAILURE);
    }
    close(fd_2);

    Client_sendMessage(messageTx, strlen(messageTx) + 1);

    int fd_1 = open(FIFO_NAME1, O_WRONLY);
    if (fd_1 < 0) {
        perror("client: Cannot open fifo\n");
        exit(EXIT_FAILURE);
    }

    char* messageRx = Client_getMessage();
    printf("(client) Message says: %s\n", messageRx);
    if (write(fd_1, messageRx, sizeof(char) * (strlen(messageRx) + 1)) < 0) {
        perror("client: Cannot write to fifo\n");
        exit(EXIT_FAILURE);
    }
    free(messageRx);
    Client_disconnectFromServer();
}