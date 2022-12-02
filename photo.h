#ifndef PHOTO_H_
#define PHOTO_H_
#include <stdio.h>
#include <stdlib.h>

/// @brief Function to open image file as jpeg and copy to buffer
/// @param _filename
/// @return char*; user must remeber to free memory
char* Photo_getImageBuffer(char* _filename);

/// @brief gets image buffer length after call to getImageBuffer
/// @param  
/// @return 
long Photo_getImageBufferLength(void);

/// @brief Function write to jpeg file the contents of imgBuffer
/// @param imgBuffer
/// @param len
void Photo_saveImageBuffer(char* imgBuffer, int len);

#endif