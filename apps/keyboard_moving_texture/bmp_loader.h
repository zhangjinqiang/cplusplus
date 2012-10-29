#ifndef BMP_LOADER_H
#define BMP_LOADER_H

#include <OpenGL/gl.h>
#include <string>
#include <vector>

class bmp_loader
{
public:
    static void load(const char* filename, unsigned char*& data, unsigned int& width, unsigned int& height);
};

#endif // BMP_LOADER_H
