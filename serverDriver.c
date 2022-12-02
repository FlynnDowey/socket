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

        int fd_2 = open(FIFO_NAME2, O_RDONLY);
        if (fd_2 < 0) {
            perror("server: Cannot open fifo\n");
            exit(EXIT_FAILURE);
        }

        char messageTx[MSG_MAX_LEN];
        memset(messageTx, 0, MSG_MAX_LEN);
        if (read(fd_2, messageTx, MSG_MAX_LEN) < 0) {
            perror("server: Cannot read fifo\n");
            exit(EXIT_FAILURE);
        }
        close(fd_2);

        FILE* pictureFile = Photo_getPictureFile(messageTx);

        // send the photo size first
        int photoSize = Photo_getPictureSize();

        if (photoSize <= 0) {
            printf("photo size is incorrect\n");
            exit(1);
        }

        Server_sendMessageSize(messageTx);

        // now repeatedly send the image as byte array
        char tcpBuffer[photoSize];

        int nb = fread(tcpBuffer, 1, sizeof(tcpBuffer), pictureFile);
        while (!feof(pictureFile)) {
            Server_sendMessage(tcpBuffer, nb);
            nb = fread(tcpBuffer, 1, sizeof(tcpBuffer), pictureFile);
            // no need to bzero
        }

        Server_sendMessage(tcpBuffer, photoSize);
        // printf("size of buffer = %d\n", sizeof(tcpBuffer));
        // while (!feof(pictureFile)) {
        //     fread(tcpBuffer, 1, sizeof(tcpBuffer), pictureFile);
        //     Server_sendMessage(tcpBuffer, sizeof(tcpBuffer));
        //     // write(sock, send_buffer, sizeof(send_buffer));
        //     bzero(tcpBuffer, sizeof(tcpBuffer));
        //     // memset(tcpBuffer, 0, sizeof(tcpBuffer));
        // }
    }
    Server_cleanup();
}