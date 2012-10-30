#include <QtGui/QApplication>
#include <QtOpenGL/QGLWidget>
#include "GLWidget.h"
#include <iostream>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

    try
    {
        GLWidget window;
        window.resize(800, 600);
        window.show();

        return app.exec();
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}
