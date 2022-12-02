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
        sleep(1);
        while (!isDone) {
            int fd_1 = open("/tmp/fifo1", O_RDONLY);
            char messageRx[MSG_MAX_LEN];
            memset(messageRx, 0, sizeof(char) * MSG_MAX_LEN);
            if (read(fd_1, messageRx, sizeof(char) * MSG_MAX_LEN) < 0) {
                perror("cannot read from fifo\n");
                exit(EXIT_FAILURE);
            }
            printf("(driver) message says: %s\n", messageRx);

            close(fd_1);

            pid_t embeddedChild = fork();
            if (embeddedChild == (pid_t)0) {
                char* args[] = { "./bash.sh", NULL };
                if (execvp(args[0], args) == -1) {
                    perror("cannot exec program.\n");
                }
            }
            sleep(5);

            // get image name from python
            char* photoName = Photo_getPictureName();
            // printf("(main) %s\n", photoName);
            if (photoName == NULL) {
                perror("error");
                exit(1);
            }

            // send message to BBG
            int fd_2 = open("/tmp/fifo2", O_WRONLY);
            if (fd_2 < 0) {
                perror("Error");
                exit(1);
            }
            if (write(fd_2, photoName, MSG_MAX_LEN) < 0) {
                perror("Cannot write to fifo\n");
                exit(EXIT_FAILURE);
            }
            close(fd_2);

            free(photoName);
        }
    }
    return 0;
}
