#include "client.h"
#include "server.h"
#include "stdio.h"
#include "stdlib.h"
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc < 3) {
        printf("./testSocket port_number ip_address");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == 0) {
        // We are the child.
        Server_init(atoi(argv[1]), argv[2]);
        Server_listen();
        int count = 0;
        while (count < 5) {

            count++;
            char* messageRx = Server_getMessage();
            printf("Message recieved from parent: %s\n", messageRx);
            free(messageRx);
            char* messageTx = "Hello from child";
            Server_sendMessage(messageTx, strlen(messageTx) + 1);
        }
        Server_cleanup();
    } else {
        sleep(1);
        // We are the parent.
        Client_init(atoi(argv[1]), argv[2]);
        Client_connectToServer();
        int count = 0;
        while (count < 5) {
            count++;
            char* messageTx = "Hello from parent";
            Client_sendMessage(messageTx, strlen(messageTx) + 1);
            char* messageRx = Client_getMessage();
            printf("Message recieved from child: %s\n", messageRx);
            free(messageRx);
        }
        Client_disconnectFromServer();
    }
}
