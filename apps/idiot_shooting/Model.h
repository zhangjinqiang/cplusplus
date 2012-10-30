#ifndef JQ_IDIOT_SHOOTING_H
#define JQ_IDIOT_SHOOTING_H

#include <list>

namespace jq
{
    class Model
    {
    public:
        Model();
        ~Model();

        void initialize();

        void key_input(int key);

        void draw();

        void update(float deltaT);

    private:
        struct color
        {
            float r;
            float g;
            float b;

            color(float rV, float gV, float bV) : r(rV), g(gV), b(bV){}
        };
        struct bullet
        {
            float m_x;
            float m_y;
            float m_speed;
            color m_color;
            float m_width;
            float m_length;

            void update(float deltaT);
            void draw();
            bullet(float xV, float yV, float sV) : m_x(xV), m_y(yV), m_speed(sV), m_color(0.6f, 0.6f, 0.6f), m_width(10), m_length(15)
            {
            }
        };

        float m_x;
        float m_y;
        float m_width;
        float m_length;
        color m_color;
        std::list<bullet> m_bullets;
    };
}

#endif
