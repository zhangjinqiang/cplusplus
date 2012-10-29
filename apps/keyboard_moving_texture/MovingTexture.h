#ifndef JQ_MOVING_TEXTURE_H
#define JQ_MOVING_TEXTURE_H

#include <OpenGL/gl.h>
#include <string>
#include <vector>

class MovingTexture
{
public:
    MovingTexture(const std::string& name);
    ~MovingTexture();

    void initialize();

    void draw();

private:
    GLuint textureid;
    float z;
    std::string filename;
    //std::vector<unsigned char> data;
    unsigned int width;
    unsigned int height;
};

#endif
