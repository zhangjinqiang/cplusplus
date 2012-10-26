#include "GLWidget.h"
#include <QtGui/QMouseEvent>
#include <QTimer>

GLWidget::GLWidget(QWidget *parent )
 : QGLWidget(parent),
   m_model(QGLWidget::width(), QGLWidget::height(), 200),
   m_frequency(20),
   m_timer(new QTimer(this))
{
	setMouseTracking(true);
    m_timer->start(m_frequency);
    connect(m_timer,SIGNAL(timeout()),this,SLOT(repaint()));
}

void GLWidget::initializeGL() {
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	glEnable(GL_POLYGON_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0, 0, 0, 0);
}

void GLWidget::resizeGL(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, h, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    m_model.resize(w, h);
}

void GLWidget::repaint()
{
    paintGL();
}

void GLWidget::paintGL() {

    m_model.update(1.0/m_frequency);
    m_model.draw();
}

void GLWidget::mousePressEvent(QMouseEvent */*event*/) {
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
	printf("%d, %d\n", event->x(), event->y());
}

void GLWidget::keyPressEvent(QKeyEvent* event) {
	switch(event->key()) {
	case Qt::Key_Escape:
		close();
		break;
	default:
		event->ignore();
		break;
	}
}
