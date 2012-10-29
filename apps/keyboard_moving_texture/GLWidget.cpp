#include "GLWidget.h"
#include <QtGui/QMouseEvent>
#include <QTimer>

GLWidget::GLWidget(QWidget *parent )
 : QGLWidget(parent),
   m_model("abc.bmp"),
   m_frequency(20),
   m_timer(new QTimer(this)),
   m_initialized(false),
   x(0.F),
   y(0.F)
{
	setMouseTracking(true);
    m_timer->start(m_frequency);
    connect(m_timer,SIGNAL(timeout()),this,SLOT(repaint()));
}

void GLWidget::initializeGL() {
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
}

void GLWidget::repaint()
{
    paintGL();
}

void GLWidget::paintGL() {
    if (!m_initialized) {
        m_initialized = true;
        m_model.initialize();
    }

    glPushMatrix();
        glTranslatef(x, y, 0.F);
        m_model.draw();
    glPopMatrix();
}

void GLWidget::mousePressEvent(QMouseEvent */*event*/) {
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
    //printf("%d, %d\n", event->x(), event->y());
}

void GLWidget::keyPressEvent(QKeyEvent* event) {
	switch(event->key()) {
	case Qt::Key_Escape:
		close();
		break;
    case Qt::Key_Left:
        x -= 2.0;
        break;
    case Qt::Key_Right:
        x += 2.0;
        break;
    case Qt::Key_Up:
        y -= 2.0;
        break;
    case Qt::Key_Down:
        y += 2.0;
        break;
	default:
		event->ignore();
		break;
	}
}
