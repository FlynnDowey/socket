#include "server.h"
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// static bool isDone = false;

#define FIFO_NAME1 "fifo1"
#define FIFO_NAME2 "fifo2"
#define MSG_MAX_LEN 1024

int main(int argc, char* argv[])
{
    if (argc < 3) {
        printf("./testSocket port_number ip_address\n");
        exit(EXIT_FAILURE);
    }

    Server_init(atoi(argv[1]), argv[2]);
    Server_listen();

    int fd_1 = open(FIFO_NAME1, O_WRONLY);
    if (fd_1 < 0) {
        perror("server: Cannot open fifo\n");
        exit(EXIT_FAILURE);
    }

    // while (!isDone) {
    char* messageRx = Server_getMessage();
    if (write(fd_1, messageRx, sizeof(char) * (strlen(messageRx) + 1)) < 0) {
        perror("server: Cannot write to fifo\n");
        exit(EXIT_FAILURE);
    }

    free(messageRx);

    int fd_2 = open(FIFO_NAME2, O_RDONLY);
    if (fd_2 < 0) {
        perror("server: Cannot open fifo\n");
        exit(EXIT_FAILURE);
    }
    char messageTx[MSG_MAX_LEN];
    if (read(fd_2, messageTx, sizeof(char) * MSG_MAX_LEN) < 0) {
        perror("server: Cannot read fifo\n");
        exit(EXIT_FAILURE);
    }
    close(fd_2);
    Server_sendMessage(messageTx, strlen(messageTx) + 1);
    // }
    close(fd_1);
    Server_cleanup();
}