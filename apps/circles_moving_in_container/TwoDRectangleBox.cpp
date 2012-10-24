#include "TwoDRectangleBox.h"
#include <QtOpenGL/QGLWidget>//find a better header to include opengl headers
#include <cmath>
#include <stdlib.h>

TwoDRectangleBox::TwoDRectangleBox(double width, double length)
 :  m_width(width),
    m_length(length),
    m_circles(),
    m_block_width(),
    m_block_length(),
    m_block_rows(),
    m_block_cols(),
    m_blocks()
{
    initialize();
}

TwoDRectangleBox::~TwoDRectangleBox()
{
}

double distance(const circle& a, const circle& b)
{
    const double dx = b.x - a.x;
    const double dy = b.y - a.y;
    return std::sqrt(dx*dx + dy*dy);
}

bool collided(const circle& a, const circle& b)
{
    return distance(a, b) <= (a.radius + b.radius + 0.3);
}

float sign(float x)
{
    return (x > 0) - (x < 0);
}

void on_collision(circle& a, circle& b)
{
    double dx  = b.x - a.x;
    double dy  = b.y - a.y;
    double dvx = b.vx - a.vx;
    double dvy = b.vy - a.vy;
    double dvdr = dx*dvx + dy*dvy;             // dv dot dr
    double dist = distance(a, b);   // distance between particle centers at collison

    // normal force F, and in x and y directions
    double F = dvdr / dist;
    double fx = F * dx / dist;
    double fy = F * dy / dist;

    // update velocities according to normal force
    a.vx += fx;
    a.vy += fy;
    b.vx -= fx;
    b.vy -= fy;

/*    const float reasonable_distance = a.radius + b.radius;
    if (dist < reasonable_distance) {
        const float diff = reasonable_distance - dist;
        if (a.vx == 0) {
            a.y += sign(a.vy) * diff;
        }
        else if (a.vy == 0) {
            a.x += sign(a.vx) * diff;
        }
        else {
            const float yaw = std::atan2(a.vy, a.vx);
            a.x += diff * std::cos(yaw);
            a.y += diff * std::sin(yaw);
        }
    }
*/
}

void TwoDRectangleBox::update(double deltaT)
{
    //clear objects information inside every block
    for (std::size_t r = 0; r < m_blocks.size(); ++r) {
        std::vector<block>& col = m_blocks.at(r);
        for (std::size_t c = 0; c < col.size(); ++c) {
            if (!col.at(c).objects.empty()) {
                col.at(c).objects.clear();
            }
        }
    }

    //update every object's position and their occupation
    for (unsigned int i = 0; i < m_circles.size(); ++i) {
        circle& c = m_circles.at(i);

        c.x += deltaT * c.vx;
        c.y += deltaT * c.vy;

        if (c.x - c.radius < 0) {
           c.x = c.radius + 0.1;
           c.vx = -c.vx;
        }
        else if (m_width <c.x +c.radius) {
           c.x = m_width -c.radius;
           c.vx = -c.vx;
        }

        if (c.y -c.radius < 0) {
           c.y = c.radius + 0.1;
           c.vy = -c.vy;
        }
        else if (m_length <c.y +c.radius) {
           c.y = m_length - c.radius - 0.1;
           c.vy = -c.vy;
        }

        const std::size_t row_index = c.y / m_block_length;
        const std::size_t col_index = c.x / m_block_width;
        block& b = m_blocks.at(row_index).at(col_index);
        b.objects.push_back(&c);
        if (row_index == 0) {
            if ((c.y + c.radius) > b.bottom) {
                m_blocks.at(row_index + 1).at(col_index).objects.push_back(&c);
            }
        }
        else if (row_index + 1 == m_block_rows) {
            if ((c.y - c.radius) < b.top) {
                m_blocks.at(row_index - 1).at(col_index).objects.push_back(&c);
            }
        }
        else {
            if ((c.y + c.radius) > b.bottom) {
                m_blocks.at(row_index + 1).at(col_index).objects.push_back(&c);
            }
            if ((c.y - c.radius) < b.top) {
                m_blocks.at(row_index - 1).at(col_index).objects.push_back(&c);
            }
        }

        if (col_index == 0) {
            if ((c.x + c.radius) > b.right) {
                m_blocks.at(row_index).at(col_index + 1).objects.push_back(&c);
            }
        }
        else if (col_index + 1 == m_block_cols) {
            if ((c.x - c.radius) < b.left) {
                m_blocks.at(row_index).at(col_index - 1).objects.push_back(&c);
            }
        }
        else {
            if ((c.x + c.radius) > b.right) {
                m_blocks.at(row_index).at(col_index + 1).objects.push_back(&c);
            }
            if ((c.x - c.radius) < b.left) {
                m_blocks.at(row_index).at(col_index - 1).objects.push_back(&c);
            }
        }
    }

    for (std::size_t r = 0; r < m_blocks.size(); ++r) {
        std::vector<block>& col = m_blocks.at(r);
        for (std::size_t c = 0; c < col.size(); ++c) {
            if (col.at(c).objects.size() > 1) {
                std::vector<circle*>& objects = col.at(c).objects;
                for (std::size_t a = 0; a < objects.size(); ++a) {
                    for (std::size_t b = a + 1; b < objects.size(); ++b) {
                        if (collided(*objects.at(a), *objects.at(b))) {
                            on_collision(*objects.at(a), *objects.at(b));
                        }
                    }
                }
            }
        }
    }
}

void draw_cirle(float x, float y, float radius, float step = 1.5)
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
        glColor3f(c.color_r,c.color_g,c.color_b);
        draw_cirle(c.x,c.y,c.radius);
        glEnd();
    }
}

void TwoDRectangleBox::initialize()
{
    const float radius = std::min(m_width, m_length) / 60;
    for (unsigned int i = 0; i < 80; ++i) {
        circle c;
        c.radius = radius;
        c.x = rand() % int(m_width -c.radius);
        if (c.x < c.radius)c.x = c.radius;
        c.y = rand() % int(m_length -c.radius);
        if (c.y < c.radius)c.y = c.radius;

        c.vx = 26 + rand() % 20;
        c.vy = 28 + rand() % 22;

        const int denominator = 100;

        c.color_r = float(rand() % denominator ) / float(denominator);
        c.color_g = float(rand() % denominator ) / float(denominator);
        c.color_b = float(rand() % denominator ) / float(denominator);

        m_circles.push_back(c);
    }

    m_block_width = radius * 7;
    m_block_length = m_block_width;
    m_block_cols = std::floor(m_width / m_block_width);
    m_block_rows = std::floor(m_length / m_block_length);
    m_block_width = m_width / float(m_block_cols);
    m_block_length = m_length / float(m_block_rows);
    for(std::size_t r = 0; r < m_block_rows; ++r) {
        std::vector<block> row;
        for(std::size_t c = 0; c < m_block_cols; ++c) {
            block b;
            b.left = c * m_block_width;
            b.right = b.left + m_block_width;
            b.top = r * m_block_length;
            b.bottom = b.top + m_block_length;
            row.push_back(b);
        }
        m_blocks.push_back(row);
    }
}
