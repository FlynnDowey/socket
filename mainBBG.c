#include "doorbell.h"
#include "general.h"
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

#define INIT_FIFO1 "mkfifo /tmp/fifo1"
#define INIT_FIFO2 "mkfifo /tmp/fifo2"
#define MSG_MAX_LEN 1024
#define PORT_NUMBER "20001"
#define IP_ADDRESS "192.168.2.2"

#define ACCESS_GRANTED "Access granted"
#define ACCESS_DENIED "Access denied"

int main(int argc, char* argv[])
{
    General_runCommand(INIT_FIFO1);
    General_runCommand(INIT_FIFO2);

    pid_t child = fork();
    if (child == -1) {
        perror("Cannot fork a process.\n");
        exit(EXIT_FAILURE);
    }

    if (child == 0) {
        // Run the client driver
        // port #: 20001
        // ip address: 192.168.2.2
        char* args[] = { "./clientDriver", PORT_NUMBER, IP_ADDRESS, NULL };
        if (execvp(args[0], args) == -1) {
            perror("cannot exec program.\n");
        }

    } else {
        // Parent porgram will
        // 1. One thread listening for doorbell to be pressed
        // 2. One thread waiting for 1st thread to send message
        // 3. "main" to read message from server
        // Note all ^^ will need to be encapsulated
        sleep(1);
        Doorbell_init();

        while (!isDone) {
            if (Doorbell_isPressed()) {
                int fd_2 = open("/tmp/fifo2", O_WRONLY);
                char* messageTx = "Someone is at the door!";
                if (write(fd_2, messageTx, strlen(messageTx) + 1) < 0) {
                    perror("Cannot write to fifo\n");
                    exit(EXIT_FAILURE);
                }

                // wait for processing

                int fd_1 = open("/tmp/fifo1", O_RDONLY);
                char messageRx[MSG_MAX_LEN];
                memset(messageRx, 0, sizeof(char) * MSG_MAX_LEN);

                // first message to recieve is the photo length
                if (read(fd_1, messageRx, sizeof(char) * MSG_MAX_LEN) < 0) {
                    perror("cannot read from fifo\n");
                    exit(EXIT_FAILURE);
                }

                if (!strncmp(messageRx, ACCESS_GRANTED, MSG_MAX_LEN)) {
                    // open the door
                } else {
                    printf("Error: %s\n", ACCESS_DENIED);
                }
                close(fd_1);

                // turn off led
                Doorbell_turnLedOFF();
            }
        }
    }
    return 0;
}
