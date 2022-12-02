#ifndef PHOTO_H_
#define PHOTO_H_
#include <stdio.h>
#include <stdlib.h>

FILE* Photo_getPictureFile(char* filename);
int Photo_getPictureSize(void);
char* Photo_getPictureName(void);

#endif