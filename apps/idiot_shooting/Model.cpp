#include "Model.h"
#include <QtGui>
#include <OpenGl/gl.h>

namespace jq {

Model::Model()
 :  m_x(200),
    m_y(500),
    m_width(30),
    m_length(30),
    m_color(0.8f, 0.8f, 0.8f)
{
}

Model::~Model()
{
}

void Model::initialize()
{
}

void Model::key_input(int key)
{
    switch (key) {
    case Qt::Key_Left:
        m_x -= 3.0;
        break;
    case Qt::Key_Right:
        m_x += 3.0;
        break;
    case Qt::Key_Space:
        m_bullets.push_back(bullet(m_x, m_y, 30));
        break;
    default:
        break;
    }
}

void Model::draw()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(m_color.r, m_color.g, m_color.b);
    glRectf(m_x, m_y, m_x + m_width, m_y + m_length);

    for (std::list<bullet>::iterator i = m_bullets.begin(); i != m_bullets.end(); ++i) {
        i->draw();
    }
}

void Model::update(float deltaT)
{
    for (std::list<bullet>::iterator i = m_bullets.begin(); i != m_bullets.end(); ++i) {
        i->update(deltaT);
        if (i->m_y < 0) {
            std::list<bullet>::iterator t = i;
            m_bullets.erase(t);
            i = t;
        }
    }
}

void Model::bullet::update(float deltaT)
{
    m_y -= m_speed * deltaT;
}

void Model::bullet::draw()
{
    glColor3f(m_color.r, m_color.g, m_color.b);
    glRectf(m_x, m_y, m_x + m_width, m_y + m_length);
}

}
