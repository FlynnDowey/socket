#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define INIT_FIFO1 "mkfifo fifo1"
#define INIT_FIFO2 "mkfifo fifo2"
#define MSG_MAX_LEN 1024

static void runCommand(char* command);

int main(int argc, char* argv[])
{
    // runCommand(INIT_FIFO1);
    // runCommand(INIT_FIFO2);

    pid_t child = fork();
    if (child == -1) {
        perror("Cannot fork a process.\n");
        exit(EXIT_FAILURE);
    }

    if (child == 0) {
        char* args[] = { "./server", "20001", "127.0.0.1", NULL };
        if (execvp(args[0], args) == -1) {
            perror("cannot exec program.\n");
        }

    } else {
        sleep(1);
        int fd_1 = open("fifo1", O_RDONLY);
        char messageRx[MSG_MAX_LEN];
        memset(messageRx, 0, sizeof(char) * MSG_MAX_LEN);
        if (read(fd_1, messageRx, sizeof(char) * MSG_MAX_LEN) < 0) {
            perror("cannot read from fifo\n");
            exit(EXIT_FAILURE);
        }
        printf("Message I got: %s\n", messageRx);

        close(fd_1);
        if (!strncmp(messageRx, "request", MSG_MAX_LEN)) {
            // launch camera
            int fd_2 = open("fifo2", O_WRONLY);
            char* messageTx = "okay";
            if (write(fd_2, messageTx, strlen(messageTx) + 1) < 0) {
                perror("Cannot write to fifo\n");
                exit(EXIT_FAILURE);
            }
            close(fd_2);
        } else {
            printf("Unknown request\n");
            int fd_2 = open("fifo2", O_WRONLY);
            char* messageTx = "nope";
            if (write(fd_2, messageTx, strlen(messageTx) + 1) < 0) {
                perror("Cannot write to fifo\n");
                exit(EXIT_FAILURE);
            }
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}

static void runCommand(char* command)
{
    // Execute the shell command (output into pipe)
    FILE* pipe = fopen(command, "r");
    // Ignore output of the command; but consume it
    // so we don't get an error when closing the pipe.
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe)) {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL)
            break;
        printf("--> %s", buffer); // Uncomment for debugging
    }
    // Get the exit code from the pipe; non-zero is an error:
    int exitCode = WEXITSTATUS(fclose(pipe));
    if (exitCode != 0) {
        perror("Unable to execute command:");
        printf(" command: %s\n", command);
        printf(" exit code: %d\n", exitCode);
    }
}