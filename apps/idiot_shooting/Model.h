#ifndef JQ_IDIOT_SHOOTING_H
#define JQ_IDIOT_SHOOTING_H

#include <list>

namespace jq
{
    class Model
    {
    public:
        Model(float w, float h);
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

        struct enemy
        {
            float x;
            float y;
            float z;
            float width;
            float length;
            float xv;
            float yv;
            float radius;
            color perimeter_color;

            enemy();
            void update(float deltaT);
            void draw();
        };

        float m_x;
        float m_y;
        float m_w;
        float m_h;
        float m_width;
        float m_length;
        color m_color;
        std::list<bullet> m_bullets;
        std::list<enemy> m_enemies;
        float m_deltaTAccumulated;
        float m_shootingThreshold;
    };
}

#endif
