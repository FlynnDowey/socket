#include "gpio.h"
#include "general.h"
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

#define MAX_SIZE 1024
#define NUMBER_OF_FILE_DESCRIPTORS 1
#define NUMBER_OF_EVENTS 1

static char* stringAppender(char* buff, char* path, char* added);
static void writeToGPIO(char* filename, char* value);
static int readLineFromFile(char* fileName, char* buff, unsigned int maxLength);

bool GPIO_setDirection(enum GPIO_DIRECTION direction, struct GPIO* gpioStruct)
{
    char buff[MAX_SIZE];
    char* path = stringAppender(buff, gpioStruct->path, "/direction");
    if (direction) {
        writeToGPIO(path, "out");
    } else if (!direction) {
        writeToGPIO(path, "in");
    } else {
        return false;
    }
    return true;
}
bool GPIO_setActiveLow(enum GPIO_ACTIVE_LOW active_low, struct GPIO* gpioStruct)
{
    char buff[MAX_SIZE];
    char* path = stringAppender(buff, gpioStruct->path, "/active_low");
    if (active_low) {
        writeToGPIO(path, "1");
    } else if (!active_low) {
        writeToGPIO(path, "0");
    } else {
        return false;
    }
    return true;
}

bool GPIO_setValue(enum GPIO_VALUE val, gpio_t* gpio)
{
    char buff[MAX_SIZE];
    char* path = stringAppender(buff, gpio->path, "/value");
    switch (val) {
    case GPIO_ONE:
        writeToGPIO(path, "1");
        break;
    case GPIO_ZERO:
        break;
        writeToGPIO(path, "0");
    default:
        break;
    }
    return true;
}

bool GPIO_setEdge(enum GPIO_EDGE edge, struct GPIO* gpioStruct)
{
    char buff[MAX_SIZE];
    char* path = stringAppender(buff, gpioStruct->path, "/edge");
    if (edge == GPIO_BOTH) {
        writeToGPIO(path, "both");
    } else if (edge == GPIO_RISING) {
        writeToGPIO(path, "rising");
    } else {
        return false;
    }
    return true;
}

int GPIO_waitForGpioEdge(gpio_t* gpio)
{
    // If you want to wait for input on multiple file, you could change this function
    // to take an array of names, and then loop throug each, adding it to the
    // epoll instance (accessed via epollfd) by calling epoll_ctl() numerous times.

    // create an epoll instance
    // .. kernel ignores size argument; must be > 0
    int epollfd = epoll_create(1);
    if (epollfd == -1) {
        fprintf(stderr, "ERROR: epoll_create() returned (%d) = %s\n", epollfd, strerror(errno));
        return -1;
    }

    // open GPIO value file:
    char buff[MAX_SIZE];
    char* path = stringAppender(buff, gpio->path, "/value");
    int fdLeft = open(path, O_RDONLY | O_NONBLOCK);
    if (fdLeft == -1) {
        fprintf(stderr, "ERROR: unable to open() GPIO value file (%d) = %s\n", fdLeft, strerror(errno));
        return -1;
    }

    // configure epoll to wait for events: read operation | edge triggered | urgent data
    struct epoll_event epollStruct;
    memset(&epollStruct, 0, sizeof(struct epoll_event));
    epollStruct.events = EPOLLIN | EPOLLET | EPOLLPRI;
    epollStruct.data.fd = fdLeft;

    // register file descriptor on the epoll instance, see: man epoll_ctl
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fdLeft, &epollStruct) == -1) {
        fprintf(stderr, "ERROR: epoll_ctl() failed on add control interface (%d) = %s\n", fdLeft, strerror(errno));
        return -1;
    }

    int waitRet = 0;
    // ignore first trigger
    for (int i = 0; i < 2; i++) {
        waitRet = epoll_wait(
            epollfd,
            &epollStruct,
            1, // maximum # events
            100); // timeout in ms, -1 = wait indefinite; 0 = returne immediately

        if (waitRet == -1) {
            fprintf(stderr, "ERROR: epoll_wait() failed (%d) = %s\n", waitRet, strerror(errno));
            close(fdLeft);
            close(epollfd);
            return -1;
        }
    }

    // cleanup epoll instance:
    close(fdLeft);
    close(epollfd);
    return waitRet;
}

static char* stringAppender(char* buff, char* path, char* added)
{
    snprintf(buff, MAX_SIZE, path);
    strncat(buff, added, MAX_SIZE - strnlen(buff, MAX_SIZE) - 1);
    return buff;
}

static void writeToGPIO(char* filename, char* value)
{
    // code from GPIO guide.
    FILE* pFile = fopen(filename, "w");
    if (pFile == NULL) {
        printf("ERROR: Unable to open export file.\n");
        exit(1);
    }

    fprintf(pFile, "%s", value);

    fclose(pFile);
}

int GPIO_getValue(struct GPIO* gpioStruct)
{
    char buff[MAX_SIZE];
    char* path = stringAppender(buff, gpioStruct->path, "/value");
    int bytes_read = readLineFromFile(path, buff, MAX_SIZE);
    if (bytes_read > 0) {
        return atoi(buff);
    } else {
        printf("Error: could not read any bytes from %s\n", path);
        exit(1);
    }
}

// function from gpio edge trigger example in guides
static int readLineFromFile(char* fileName, char* buff, unsigned int maxLength)
{
    FILE* file = fopen(fileName, "r");
    int bytes_read = getline(&buff, &maxLength, file);
    fclose(file);
    return bytes_read;
}