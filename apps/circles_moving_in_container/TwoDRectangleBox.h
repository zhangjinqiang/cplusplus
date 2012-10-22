#ifndef TWODRECTANGLEBOX_H
#define TWODRECTANGLEBOX_H

#include <vector>

class TwoDRectangleBox
{
public:
    TwoDRectangleBox(double width, double length);
    ~TwoDRectangleBox();

    void initialize();

    void update(double deltaT);

    void draw();

private:
	struct circle
	{
		float m_x;
		float m_y;
		float m_radius;
		float m_x_speed;
		float m_y_speed;
        float m_color_r;
        float m_color_g;
        float m_color_b;
	};

    double m_width;
    double m_length;
    std::vector<circle> m_circles;
};

#endif // TWODRECTANGLEBOX_H
