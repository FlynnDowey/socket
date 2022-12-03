#include "photo.h"
#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

// Adapted from https://stackoverflow.com/questions/13097375/sending-images-over-sockets

#define FIFO_LOCATION "/tmp/testpipe"
#define MAX_MSG_LEN 1024

const char* fileFormat = ".jpg";
static long bufferSize = 0;

static char* removeWhitespaces(char* string);

char* Photo_getImageBuffer(char* _filename)
{
    // remove white spaces from end of string for proper
    // file handling.
    char* filename = removeWhitespaces(_filename);

    // open the file as read binary
    FILE* pFile = fopen(filename, "rb");
    if (pFile == NULL) {
        perror("error(photo get buff)");
        exit(EXIT_FAILURE);
    }
    // seek to start of file
    fseek(pFile, 0, SEEK_END);
    long len = ftell(pFile);
    assert(len);
    bufferSize = len;
    fseek(pFile, 0, SEEK_SET);

    // Copying image file to buffer
    char* imgBuff = (char*)malloc(len);
    ssize_t bytes_read = fread(imgBuff, 1, len, pFile);
    if (bytes_read < 0) {
        perror("Error: did not read any bytes\n");
        exit(EXIT_FAILURE);
    }
    fclose(pFile);

    return imgBuff;
}

long Photo_getImageBufferLength(void)
{
    return bufferSize;
}

char* Photo_saveImageBuffer(char* imgBuffer, int len)
{
    time_t seconds;
    time(&seconds);

    //char copyName[MAX_MSG_LEN];
    char* copyName = malloc(MAX_MSG_LEN);
    memset(copyName, 0, MAX_MSG_LEN);
    snprintf(copyName, MAX_MSG_LEN, "%ld%s", seconds, fileFormat);

    printf("Saving copy image as %s\n", copyName);

    FILE* pCopy = fopen(copyName, "wb");
    if (pCopy == NULL) {
        perror("error (photo saving image)");
        exit(EXIT_FAILURE);
    }

    ssize_t bytes_write = fwrite(imgBuffer, 1, len, pCopy);
    if (bytes_write < 0) {
        perror("Error: did not write any bytes\n");
        exit(EXIT_FAILURE);
    }
    fclose(pCopy);

    return copyName;
}

// copied code from https://codeforwin.org/2016/04/c-program-to-trim-trailing-white-space-characters-in-string.html

static char* removeWhitespaces(char* string)
{
    int idx = -1;
    int i = 0;
    while (string[i] != '\0') {
        if (string[i] != ' ' && string[i] != '\t' && string[i] != '\n') {
            idx = i;
        }
        i++;
    }
    string[idx + 1] = '\0';
    return string;
}