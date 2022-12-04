#include "general.h"
#include "photo.h"
#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

static bool isDone = false;

#define PORT_NUMBER "20001"
#define MY_IP_ADDRESS "192.168.2.2"
#define MSG_MAX_LEN 1024

#define MAKE_FIFO_1 "./makeFifo_1.sh"
#define MAKE_FIFO_2 "./makeFifo_2.sh"
#define FIFO_NAME_1 "/tmp/fifo1"
#define FIFO_NAME_2 "/tmp/fifo_ImgName"

#define SERVER_COMMAND "./serverDriver"

int main(int argc, char* argv[])
{
    General_runCommand(MAKE_FIFO_1);
    General_runCommand(MAKE_FIFO_2);
    pid_t child = fork();
    if (child == -1) {
        perror("Cannot fork a process.\n");
        exit(EXIT_FAILURE);
    }

    if (child == 0) {
        char* args[] = { SERVER_COMMAND, PORT_NUMBER, MY_IP_ADDRESS, NULL };
        if (execvp(args[0], args) == -1) {
            perror("cannot exec program.\n");
        }
    } else {
        while (!isDone) {
            // wait for server to recieve message from client
            // this is a blocking call; once the message is recieved
            // the camera will capture a photo.
            int fd_1 = open("/tmp/fifo1", O_RDONLY);
            char messageRx[MSG_MAX_LEN];
            memset(messageRx, 0, sizeof(char) * MSG_MAX_LEN);
            if (read(fd_1, messageRx, sizeof(char) * MSG_MAX_LEN) < 0) {
                perror("cannot read from fifo\n");
                exit(EXIT_FAILURE);
            }
            close(fd_1);

            // get unix timestamp for filename
            // Use jpeg formatting
            time_t seconds;
            time(&seconds);

            char filename[MSG_MAX_LEN];
            snprintf(filename, MSG_MAX_LEN, "%ld%s", seconds, ".jpg");

            pid_t child = fork();

            // call the bash script to take a picture with filename "filename"
            if (child == (pid_t)0) {
                char* args[] = { "./bash.sh", filename, NULL };
                if (execvp(args[0], args) == -1) {
                    perror("cannot exec program.\n");
                }
            } else {
                wait(NULL);

                // open fifo and write filename so the server can open the image
                int fd_2 = open("/tmp/fifo_ImgName", O_WRONLY);
                if (write(fd_2, &filename, strlen(filename) + 1) < 0) {
                    perror("Cannot write to fifo\n");
                    exit(EXIT_FAILURE);
                }
                close(fd_2);

                // TODO: implement isDone feature
            }
        }
    }
    return 0;
}
