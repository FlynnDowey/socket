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
#define FIFO_NAME2 "/tmp/fifo_ImgName"
#define MSG_MAX_LEN 1024

int main(int argc, char* argv[])
{
    if (argc < 3) {
        printf("./testSocket port_number ip_address\n");
        exit(EXIT_FAILURE);
    }

    // init the server for connection
    Server_init(atoi(argv[1]), argv[2]);
    Server_listen();

    while (!isDone) {
        // open fifo between driver and main program
        int fd_1 = open(FIFO_NAME1, O_WRONLY);
        if (fd_1 < 0) {
            perror("server: Cannot open fifo\n");
            exit(EXIT_FAILURE);
        }
        // once a message from the client is recieved, pipe it to main.
        char* messageRx = Server_getMessage();
        if (write(fd_1, messageRx, sizeof(char) * (strlen(messageRx) + 1)) < 0) {
            perror("server: Cannot write to fifo\n");
            exit(EXIT_FAILURE);
        }
        close(fd_1);
        free(messageRx);

        // open fifo and wait for main to send image filename
        int fd_2 = open(FIFO_NAME2, O_RDONLY);
        char filename[MSG_MAX_LEN];
        memset(filename, 0, sizeof(char) * MSG_MAX_LEN);
        if (read(fd_2, filename, sizeof(char) * MSG_MAX_LEN) < 0) {
            perror("cannot read from fifo\n");
            exit(EXIT_FAILURE);
        }

        // open image and copy to a buffer
        char* imgBuffer = Photo_getImageBuffer(filename);
        long imgSize = Photo_getImageBufferLength();

        // send image length first then byte array of the image
        Server_sendMessageSize(imgSize);
        Server_sendMessage(imgBuffer, imgSize);

        free(imgBuffer);
    }

    Server_cleanup();
}