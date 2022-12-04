#include "client.h"
#include "photo.h"
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
#define FIFO_NAME2 "/tmp/fifo_ImgName"
static bool isDone = false;
#define MSG_MAX_LEN 1024

int main(int argc, char* argv[])
{
    // init the socket for connection
    Client_init(atoi(argv[1]), argv[2]);
    Client_connectToServer();

    while (!isDone) {
        // open fifo between main and driver
        int fd_1 = open(FIFO_NAME1, O_RDONLY);
        if (fd_1 < 0) {
            perror("client: Cannot open fifo\n");
            exit(EXIT_FAILURE);
        }
        char messageTx[MSG_MAX_LEN];
        if (read(fd_1, messageTx, sizeof(char) * MSG_MAX_LEN) < 0) {
            perror("client: Cannot read fifo\n");
            exit(EXIT_FAILURE);
        }
        close(fd_1);
        // send message across socket.
        Client_sendMessage(messageTx, strlen(messageTx) + 1);

        // Read the image size from socket
        long imgSize = Client_getMessageSize();
        char* imgBuffer = Client_getMessage(imgSize);

        sleep(1);
        // save the image for processing
        char* filename = Photo_saveImageBuffer(imgBuffer, imgSize);

        free(imgBuffer);

        // open fifo to main
        int fd_2 = open(FIFO_NAME2, O_WRONLY);
        if (fd_2 < 0) {
            perror("client: Cannot open fifo\n");
            exit(EXIT_FAILURE);
        }
        // signal completion of image being saved.
        if (write(fd_2, filename, strlen(filename) + 1) < 0) {
            perror("client: Cannot write to fifo\n");
            exit(EXIT_FAILURE);
        }
        close(fd_2);
        free(filename);

        // TODO: isDone feature
    }
    Client_disconnectFromServer();
}