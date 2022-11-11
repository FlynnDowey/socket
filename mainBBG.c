#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>

#define INIT_FIFO1 "mkfifo /tmp/fifo1"
#define INIT_FIFO2 "mkfifo /tmp/fifo2"
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
        // Run the client driver
        // port #: 20001
        // ip address: 192.168.1.69
        char* args[] = { "./clientDriver", "20001", "192.168.1.69", NULL };
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
        int fd_2 = open("/tmp/fifo2", O_WRONLY);
        char* messageTx = "Hello from the BeagleBone!";
        if (write(fd_2, messageTx, strlen(messageTx) + 1) < 0) {
            perror("Cannot write to fifo\n");
            exit(EXIT_FAILURE);
        }

        int fd_1 = open("/tmp/fifo1", O_RDONLY);
        char messageRx[MSG_MAX_LEN];
        memset(messageRx, 0, sizeof(char) * MSG_MAX_LEN);
        if (read(fd_1, messageRx, sizeof(char) * MSG_MAX_LEN) < 0) {
            perror("cannot read from fifo\n");
            exit(EXIT_FAILURE);
        }
        printf("(driver) message says: %s\n", messageRx);

        close(fd_1);
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
