#include "TwoDRectangleBox.h"
#include <QtOpenGL/QGLWidget>//find a better header to include opengl headers
#include <cmath>
#include <stdlib.h>

TwoDRectangleBox::TwoDRectangleBox(double width, double length)
:  m_width(width),
   m_length(length),
   m_circles()
{
    initialize();
}

TwoDRectangleBox::~TwoDRectangleBox()
{
}

void TwoDRectangleBox::update(double deltaT)
{
    for (unsigned int i = 0; i < m_circles.size(); ++i) {
        circle& c = m_circles.at(i);

        c.m_x += deltaT * c.m_x_speed;
        c.m_y += deltaT * c.m_y_speed;

        if (c.m_x - c.m_radius < 0) {
            c.m_x = c.m_radius + 0.1;
            c.m_x_speed = -c.m_x_speed;
        }
        else if (m_width < c.m_x + c.m_radius) {
            c.m_x = m_width - c.m_radius;
            c.m_x_speed = -c.m_x_speed;
        }

        if (c.m_y - c.m_radius < 0) {
            c.m_y = c.m_radius + 0.1;
            c.m_y_speed = -c.m_y_speed;
        }
        else if (m_length < c.m_y + c.m_radius) {
            c.m_y = m_length - c.m_radius - 0.1;
            c.m_y_speed = -c.m_y_speed;
        }
    }
}

void draw_cirle(float x, float y, float radius, float step = 1)
{
    glVertex2f(x, y);
    for(float angle = 0; angle < 360; angle += step) {
        glVertex2f(x + std::sin(angle) * radius, y + std::cos(angle) * radius);
    }
}

void TwoDRectangleBox::draw()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.2, 0.2, 0.2);

    for (unsigned int i = 0; i < m_circles.size(); ++i) {
        const circle& c = m_circles.at(i);
        glBegin(GL_TRIANGLE_FAN);
        glColor3f(c.m_color_r, c.m_color_g, c.m_color_b);
        draw_cirle(c.m_x, c.m_y, c.m_radius);
        glEnd();
    }
}

void TwoDRectangleBox::initialize()
{
    for (unsigned int i = 0; i < 10; ++i) {
        circle c;
        c.m_radius = (std::min(m_width, m_length)/30),
        c.m_x = rand() % int(m_width - c.m_radius);
        if (c.m_x < c.m_radius) c.m_x = c.m_radius;
        c.m_y = rand() % int(m_length - c.m_radius);
        if (c.m_y < c.m_radius) c.m_y = c.m_radius;

        c.m_x_speed = 36 + rand() % 50;
        c.m_y_speed = 58 + rand() % 62;

        const int denominator = 100;

        c.m_color_r = float(rand() % denominator ) / float(denominator);
        c.m_color_g = float(rand() % denominator ) / float(denominator);
        c.m_color_b = float(rand() % denominator ) / float(denominator);

        m_circles.push_back(c);
    }
}
