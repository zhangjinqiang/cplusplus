#ifndef _GLWIDGET_H
#define _GLWIDGET_H

#include "Model.h"
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
    jq::Model m_model;
    float m_frequency;
    QTimer* m_timer;
    float x;
    float y;
    bool m_keyPressed;
    int m_keyInput;
};

#endif

