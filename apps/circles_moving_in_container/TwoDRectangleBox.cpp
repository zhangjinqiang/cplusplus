#include "TwoDRectangleBox.h"
#include <QtOpenGL/QGLWidget>//find a better header to include opengl headers
#include <cmath>

TwoDRectangleBox::TwoDRectangleBox(double width, double length)
:  m_width(width),
   m_length(length),
   m_x(width/2),
   m_y(length/2),
   m_radius(std::min(width, length)/20),
   m_x_speed(18),
   m_y_speed(25)
{
}

TwoDRectangleBox::~TwoDRectangleBox()
{
}

void TwoDRectangleBox::initialize()
{

}

void TwoDRectangleBox::update(double deltaT)
{
    m_x += deltaT * m_x_speed;
    m_y += deltaT * m_y_speed;

    if (m_x - m_radius < 0) {
        m_x = m_radius + 0.1;
        m_x_speed = -m_x_speed;
    }
    else if (m_width < m_x + m_radius) {
        m_x = m_width - m_radius;
        m_x_speed = -m_x_speed;
    }

    if (m_y - m_radius < 0) {
        m_y = m_radius + 0.1;
        m_y_speed = -m_y_speed;
    }
    else if (m_length < m_y + m_radius) {
        m_y = m_length - m_radius - 0.1;
        m_y_speed = -m_y_speed;
    }
}

void draw_cirle(float x, float y, float radius, float step = 1)
{
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for(float angle = 0; angle < 360; angle += step) {
            glVertex2f(x + std::sin(angle) * radius, y + std::cos(angle) * radius);
        }
    glEnd();
}

void TwoDRectangleBox::draw()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1, 0, 0);

    draw_cirle(m_x, m_y, m_radius);
}
