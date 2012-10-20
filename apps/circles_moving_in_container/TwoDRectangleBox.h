#ifndef TWODRECTANGLEBOX_H
#define TWODRECTANGLEBOX_H

class TwoDRectangleBox
{
public:
    TwoDRectangleBox(double width, double length);
    ~TwoDRectangleBox();

    void initialize();

    void update(double deltaT);

    void draw();

private:
    double m_width;
    double m_length;
    float m_x;
    float m_y;
    float m_radius;
    float m_x_speed;
    float m_y_speed;
};

#endif // TWODRECTANGLEBOX_H
