#include "photo.h"
#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// Adapted from https://stackoverflow.com/questions/13097375/sending-images-over-sockets

#define FIFO_LOCATION "/tmp/testpipe"
#define MAX_MSG_LEN 1024
static int s_size = 0;
static char s_name[MAX_MSG_LEN];

const char* fileFormat = ".jpg";

static char* getMostRecentPicture(void);
static char* removeWhitespaces(char* string);

FILE* Photo_getPictureFile(char* filename)
{
    printf("(photo) %s\n", filename);
    FILE* picture = fopen(filename, "rb");
    if (picture == NULL) {
        perror("cannot open picture\n");
        exit(EXIT_FAILURE);
    }
    fseek(picture, 0, SEEK_END);
    s_size = ftell(picture);
    assert(s_size);
    fseek(picture, 0, SEEK_SET);

    if (s_size < 0) {
        printf("Error reading picture.\n");
        exit(EXIT_FAILURE);
    }

    return picture;
}

int Photo_getPictureSize(void)
{
    return s_size;
}

char* Photo_getPictureName(void)
{
    char* name = getMostRecentPicture();
    if (name == NULL) {
        printf("Error cannot find name of file.\n");
        exit(1);
    }
    memset(s_name, 0, MAX_MSG_LEN);
    strncpy(s_name, name, MAX_MSG_LEN);
    // printf("s_name in photo is %s\n", s_name);
    return name;
}

static char* getMostRecentPicture(void)
{
    int fd_2 = open(FIFO_LOCATION, O_RDONLY);
    char* pictureName = malloc(sizeof(char) * MAX_MSG_LEN);
    if (pictureName == NULL) {
        printf("Cannot allocate memory\n");
        exit(1);
    }
    memset(pictureName, 0, MAX_MSG_LEN);
    pictureName[MAX_MSG_LEN - 1] = '\0';
    if (read(fd_2, pictureName, sizeof(char) * MAX_MSG_LEN) < 0) {
        perror("server: Cannot read fifo\n");
        exit(EXIT_FAILURE);
    }
    char* newName = removeWhitespaces(pictureName);

    // printf("(photo) strlen of picture name = %d\n", strlen(newName) + 1);

    close(fd_2);
    return newName;
}

// copied code from https://codeforwin.org/2016/04/c-program-to-trim-trailing-white-space-characters-in-string.html
// used to trim extra spaces from fifo
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