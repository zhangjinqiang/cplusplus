#include <QtGui/QApplication>
#include <QtOpenGL/QGLWidget>
#include "GLWidget.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	GLWidget window;
    window.resize(600, 400);
	window.show();

	return app.exec();
}
