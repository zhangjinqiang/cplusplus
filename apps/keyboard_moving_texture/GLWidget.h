#ifndef _GLWIDGET_H
#define _GLWIDGET_H

#include "MovingTexture.h"
#include <QtOpenGL/QGLWidget>

class QTimer;

class GLWidget : public QGLWidget {

	Q_OBJECT

public:
	GLWidget(QWidget *parent = NULL);

protected:
	void initializeGL();
	void resizeGL(int w, int h);
    void repaint();
	void paintGL();
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);

private:
    MovingTexture m_model;
    float m_frequency;
    QTimer* m_timer;
    bool m_initialized;
    float x;
    float y;
};

#endif

