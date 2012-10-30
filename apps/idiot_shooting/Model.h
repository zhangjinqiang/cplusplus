#ifndef JQ_IDIOT_SHOOTING_H
#define JQ_IDIOT_SHOOTING_H

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

    private:
        struct color
        {
            float r;
            float g;
            float b;

            color(float rV, float gV, float bV) : r(rV), g(gV), b(bV){}
        };

        float m_x;
        float m_y;
        float m_width;
        float m_length;
        color m_color;
    };
}

#endif
