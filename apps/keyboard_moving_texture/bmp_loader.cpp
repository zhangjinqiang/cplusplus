#include "bmp_loader.h"
#include <stdexcept>
#include <cassert>

void bmp_loader::load( const char* filename, unsigned char*& data, unsigned int& width, unsigned int& height)
{
    // Open the file
    FILE * file = fopen(filename,"rb");
    if (!file) {
        throw std::invalid_argument("Image could not be opened\n");
    }

    unsigned char header[54]; // Each BMP file begins by a 54-bytes header
    if ( fread(header, 1, 54, file) != 54 ){ // If not 54 bytes read : problem
        fclose(file);
        throw std::invalid_argument("Not a correct BMP file\n");
    }

    if ( header[0]!='B' || header[1]!='M' ){
        fclose(file);
        throw std::invalid_argument("Not a correct BMP file\n");
    }

    // Read ints from the byte array
    unsigned int dataPos    = *(int*)&(header[0x0A]);
    unsigned int imageSize  = *(int*)&(header[0x22]);
    width      = *(int*)&(header[0x12]);
    height     = *(int*)&(header[0x16]);

    // Some BMP files are misformatted, guess missing information
    if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
    if (dataPos==0)      dataPos=54; // The BMP header is done that way

    // Create a buffer
    data = new unsigned char[imageSize];

    // Read the actual data from the file into the buffer
    std::size_t read = fread(data, 1, imageSize, file);
    assert(read > 0);

    //Everything is in memory now, the file can be closed
    fclose(file);
}
