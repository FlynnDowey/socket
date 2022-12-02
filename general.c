#include "general.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char* General_stringAppender(char* yourString, char* toBeAdded)
{
    int size = strlen(yourString) + strlen(toBeAdded) + 1;
    char* newString = malloc(size * sizeof(char));

    snprintf(newString, size, yourString); // copy string
    strncat(newString, toBeAdded, strlen(toBeAdded));

    return newString;
}

void General_writeToFile(char* filename, char* value)
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

void General_runCommand(char* command)
{
    // Execute the shell command (output into pipe)
    FILE* pipe = popen(command, "r");
    // Ignore output of the command; but consume it
    // so we don't get an error when closing the pipe.
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe)) {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL)
            break;
        // printf("--> %s", buffer); // Uncomment for debugging
    }
    // Get the exit code from the pipe; non-zero is an error:
    int exitCode = WEXITSTATUS(pclose(pipe));
    if (exitCode != 0) {
        perror("Unable to execute command:");
        printf(" command: %s\n", command);
        printf(" exit code: %d\n", exitCode);
    }
}