#include "photo.h"
#include "server.h"
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static bool isDone = false;

#define FIFO_NAME1 "/tmp/fifo1"
#define FIFO_NAME2 "/tmp/fifo2"
#define MSG_MAX_LEN 1024

int main(int argc, char* argv[])
{
    if (argc < 3) {
        printf("./testSocket port_number ip_address\n");
        exit(EXIT_FAILURE);
    }
    Server_init(atoi(argv[1]), argv[2]);
    Server_listen();

    while (!isDone) {

        int fd_1 = open(FIFO_NAME1, O_WRONLY);
        if (fd_1 < 0) {
            perror("server: Cannot open fifo\n");
            exit(EXIT_FAILURE);
        }

        char* messageRx = Server_getMessage();
        if (write(fd_1, messageRx, sizeof(char) * (strlen(messageRx) + 1)) < 0) {
            perror("server: Cannot write to fifo\n");
            exit(EXIT_FAILURE);
        }
        close(fd_1);
        free(messageRx);

        // Read the image buffer size from fifo
        int fd_2 = open(FIFO_NAME2, O_RDONLY);
        if (fd_2 < 0) {
            perror("server: Cannot open fifo\n");
            exit(EXIT_FAILURE);
        }

        long imgSize = 0;
        if (read(fd_2, imgSize, sizoef(imgSize)) < 0) {
            perror("server: Cannot read fifo\n");
            exit(EXIT_FAILURE);
        }
        close(fd_2);

        // Read the image buffer from fifo
        int fd_2 = open(FIFO_NAME2, O_RDONLY);
        if (fd_2 < 0) {
            perror("server: Cannot open fifo\n");
            exit(EXIT_FAILURE);
        }

        char* imgBuffer = NULL;
        if (read(fd_2, imgBuffer, sizoef(imgBuffer) * imgSize) < 0) {
            perror("server: Cannot read fifo\n");
            exit(EXIT_FAILURE);
        }
        close(fd_2);

        // send the image size to BBG via socket connection
        Server_sendMessageSize(imgSize);
        // send image buffer to BBG via socket connection
        Server_sendMessage(imgBuffer, imgSize);

        free(imgBuffer); 

    }
    Server_cleanup();
}