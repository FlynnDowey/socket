#include "doorbell.h"
#include "general.h"
#include "photo.h"
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static bool isDone = false;

#define MSG_MAX_LEN 1024
#define PORT_NUMBER "20001"
#define IP_ADDRESS "192.168.2.2"

#define ACCESS_GRANTED "Access granted"
#define ACCESS_DENIED "Access denied"

#define CLIENT_COMMAND "./clientDriver"

#define FIFO_NAME_1 "/tmp/fifo1"
#define FIFO_NAME_2 "/tmp/fifo_ImgName"

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
        // Run the client driver
        // port #: 20001
        // ip address: 192.168.2.2
        char* args[] = { CLIENT_COMMAND, PORT_NUMBER, IP_ADDRESS, NULL };
        if (execvp(args[0], args) == -1) {
            perror("cannot exec program.\n");
        }

    } else {
        sleep(1);
        //  init button for pressing
        Doorbell_init();
        while (!isDone) {
            if (Doorbell_isPressed()) {
                // open fifo between main and client
                int fd_1 = open(FIFO_NAME_1, O_WRONLY);
                char* messageTx = "Someone is at the door!";
                if (write(fd_1, messageTx, strlen(messageTx) + 1) < 0) {
                    perror("Cannot write to fifo\n");
                    exit(EXIT_FAILURE);
                }
                close(fd_1);

                // wait for processing
                sleep(2);

                // read image name from driver
                int fd_2 = open(FIFO_NAME_2, O_RDONLY);
                char filename[MSG_MAX_LEN];
                memset(filename, 0, MSG_MAX_LEN);

                if (read(fd_2, filename, MSG_MAX_LEN) < 0) {
                    perror("(main)cannot read from fifo 2\n");
                    exit(EXIT_FAILURE);
                }
                close(fd_2);

                // process image

                // end proccessing

                // turn off led
                Doorbell_turnLedOFF();
            }
        }
    }
    return 0;
}
