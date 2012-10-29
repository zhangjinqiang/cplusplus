#include "MovingTexture.h"
#include "bmp_loader.h"
#include <cassert>
#include <iostream>

MovingTexture::MovingTexture(const std::string& name)
 :  textureid(0),
    z(0),
    filename(name),
    //data(),
    width(),
    height()
{
}

MovingTexture::~MovingTexture()
{
}

void MovingTexture::initialize()
{
    // Create one OpenGL texture
    glGenTextures(1, &textureid);
    if (textureid <= 0) {
        std::cout << "opengl error :=" << glGetError() << std::endl;
    }
    else {
        std::cout << "got texture" << std::endl;
        unsigned char* data = 0;
        bmp_loader::load(filename.c_str(), data, width, height);
        glBindTexture(GL_TEXTURE_2D, textureid);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, height, width, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
}

void MovingTexture::draw()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureid);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glPushMatrix();
        glBegin(GL_QUADS);
            glTexCoord2f(0.F, 0.F); glVertex3f(0, 0, z);
            glTexCoord2f(0.F, 1.F); glVertex3f(0, height, z);
            glTexCoord2f(1.F, 1.F); glVertex3f(width, height, z);
            glTexCoord2f(1.F, 0.F); glVertex3f(width, 0, z);
        glEnd();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}
