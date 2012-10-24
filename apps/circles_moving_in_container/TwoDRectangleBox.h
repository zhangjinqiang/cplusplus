#ifndef TWODRECTANGLEBOX_H
#define TWODRECTANGLEBOX_H

#include <vector>

struct circle
{
    float x;
    float y;
    float radius;
    float vx;
    float vy;
    float color_r;
    float color_g;
    float color_b;
};

class TwoDRectangleBox
{
public:
    TwoDRectangleBox(double width, double length);
    ~TwoDRectangleBox();

    void initialize();

    void update(double deltaT);

    void draw();

private:

    struct block
    {
        float left;
        float right;
        float top;
        float bottom;
        std::vector<circle*> objects;
    };

    double m_width;
    double m_length;
    std::vector<circle> m_circles;
    float m_block_width;
    float m_block_length;
    std::size_t m_block_rows;
    std::size_t m_block_cols;
    std::vector<std::vector<block> > m_blocks;
};

#endif // TWODRECTANGLEBOX_H
