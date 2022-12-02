#include "general.h"
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

static bool isDone = false;

#define INIT_FIFO1 "mkfifo /tmp/fifo1"
#define INIT_FIFO2 "mkfifo /tmp/fifo2"
#define PORT_NUMBER "20001"
#define MY_IP_ADDRESS "192.168.2.2"
#define MSG_MAX_LEN 1024

#define CAPTURE_MODULE "capture"

int main(int argc, char* argv[])
{
    General_runCommand("./makeFifo_1.sh");
    General_runCommand("./makeFifo_2.sh");

    pid_t child = fork();
    if (child == -1) {
        perror("Cannot fork a process.\n");
        exit(EXIT_FAILURE);
    }

    if (child == 0) {
        char* args[] = { "./serverDriver", PORT_NUMBER, MY_IP_ADDRESS, NULL };
        if (execvp(args[0], args) == -1) {
            perror("cannot exec program.\n");
        }
    } else {
        while (!isDone) {
            int fd_1 = open("/tmp/fifo1", O_RDONLY);
            char messageRx[MSG_MAX_LEN];
            memset(messageRx, 0, sizeof(char) * MSG_MAX_LEN);
            if (read(fd_1, messageRx, sizeof(char) * MSG_MAX_LEN) < 0) {
                perror("cannot read from fifo\n");
                exit(EXIT_FAILURE);
            }
            close(fd_1);

            pid_t child = fork();

            if (child == (pid_t)0) {
                char* args[] = { "./bash.sh", NULL };
                if (execvp(args[0], args) == -1) {
                    perror("cannot exec program.\n");
                }
            }
            sleep(5);
            int fd = open("/tmp/testpipe", O_RDONLY);
            char filename[MSG_MAX_LEN];
            memset(filename, 0, sizeof(char) * MSG_MAX_LEN);
            if (read(fd, filename, sizeof(char) * MSG_MAX_LEN) < 0) {
                perror("cannot read from fifo\n");
                exit(EXIT_FAILURE);
            }

            char* imgBuffer = Photo_getImageBuffer(filename);
            long imgSize = Photo_getImageBufferLength();

            // Send length of image buffer to driver
            int fd_2 = open("/tmp/fifo2", O_WRONLY);
            if (fd_2 < 0) {
                perror("Error");
                exit(1);
            }

            if (write(fd_2, imgSize, sizeof(imgSize)) < 0) {
                perror("Cannot write to fifo\n");
                exit(EXIT_FAILURE);
            }
            close(fd_2);

            // send image buffer to driver
            int fd_2 = open("/tmp/fifo2", O_WRONLY);
            if (fd_2 < 0) {
                perror("Error");
                exit(1);
            }

            if (write(fd_2, imgBuffer, sizeof(imgSize) * imgSize) < 0) {
                perror("Cannot write to fifo\n");
                exit(EXIT_FAILURE);
            }
            close(fd_2);

            // free(imgBuffer);
            // or set to NULL?


            // free(photoName);
        }
    }
    return 0;
}
