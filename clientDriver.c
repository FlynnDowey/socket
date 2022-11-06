#include "client.h"
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// static bool isDone = false;
#define MSG_MAX_LEN 1024

int main(int argc, char* argv[])
{
    Client_init(atoi(argv[1]), argv[2]);
    Client_connectToServer();
    // while (!isDone) {
    char* messageTx = "Hello from client";
    Client_sendMessage(messageTx, strlen(messageTx) + 1);
    char* messageRx = Client_getMessage();
    printf("Message recieved from parent: %s\n", messageRx);
    free(messageRx);
    //}
    Client_disconnectFromServer();
}