#include "Model.h"
#include <QtGui>
#include <OpenGl/gl.h>

namespace jq {

Model::Model(float w, float h)
 :  m_x(00),
    m_y(500),
    m_w(20),
    m_h(18),
    m_width(w),
    m_length(h),
    m_color(0.8f, 0.8f, 0.8f),
    m_deltaTAccumulated(0.0),
    m_shootingThreshold(8.)
{
}

Model::~Model()
{
}

void Model::initialize()
{
    enemy e0;
    e0.x = 20;
    e0.y = 30;
    e0.xv = 6;
    e0.yv = 1;
    e0.perimeter_color = color(0.2, 0.3, 0.4);

    enemy e1;
    e1.x = 180;
    e1.y = 40;
    e1.xv = 5;
    e1.yv = 2;
    e1.perimeter_color = color(0.2, 0.3, 0.4);

    m_enemies.push_back(e0);
    m_enemies.push_back(e1);
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
    glRectf(m_x, m_y, m_x + m_w, m_y + m_h);

    for (std::list<bullet>::iterator i = m_bullets.begin(); i != m_bullets.end(); ++i) {
        i->draw();
    }

    for (std::list<enemy>::iterator i = m_enemies.begin(); i != m_enemies.end(); ++i) {
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

    bool to_shoot(false);
    m_deltaTAccumulated += deltaT;
    if (m_deltaTAccumulated > m_shootingThreshold) {
        to_shoot = true;
        m_shootingThreshold += deltaT * 50;
    }

    for (std::list<enemy>::iterator i = m_enemies.begin(); i != m_enemies.end(); ++i) {
        //i->shoot(m_x, m_y);
        i->update(deltaT);
        if (i->x < 0 || m_width < i->x) {
            i->xv = -i->xv;
        }

        if (i->y < 0 || m_length < i->y ) {
            i->yv = -i->yv;
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

Model::enemy::enemy()
 :  x(),
    y(),
    z(0),
    width(12),
    length(15),
    xv(0),
    yv(0),
    radius(12),
    perimeter_color(0.f, 0.f, 0.f)
{
}

void Model::enemy::update(float deltaT)
{
    x += xv * deltaT;
    y += yv * deltaT;
}

void Model::enemy::draw()
{
    glColor3f(perimeter_color.r, perimeter_color.g, perimeter_color.b);
    glRectf(x, y, x + width, y + length);
}

}
