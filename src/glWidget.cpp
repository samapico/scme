#include "glWidget.h"

#include <QtGui/QPaintEvent>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

//////////////////////////////////////////////////////////////////////////

GLWidget::GLWidget(Editor* editor, QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent),
    mEditor(editor),
    mTop(0),
    mLeft(0),
    mZoomFactor(1)
{
}

//////////////////////////////////////////////////////////////////////////

GLWidget::~GLWidget()
{

}

//////////////////////////////////////////////////////////////////////////

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

//////////////////////////////////////////////////////////////////////////

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}

//////////////////////////////////////////////////////////////////////////

void GLWidget::initializeGL()
{
    QGLWidget::initializeGL();
    /*
    qglClearColor(QColor::fromCmykF(0.39, 0.39, 0.0, 0.0));

    //logo = new QtLogo(this, 64);
    //logo->setColor(qtGreen.dark());

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_FLAT);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_MULTISAMPLE);
    static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    */
}

//////////////////////////////////////////////////////////////////////////
/*
void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -10.0);
    //glRotatef(xRot / 16.0, 1.0, 0.0, 0.0);
    //glRotatef(yRot / 16.0, 0.0, 1.0, 0.0);
    //glRotatef(zRot / 16.0, 0.0, 0.0, 1.0);
    //logo->draw();
}
*/
//////////////////////////////////////////////////////////////////////////

void GLWidget::resizeGL( int width, int height )
{
    QGLWidget::resizeGL(width, height);
    /*
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES_1
    glOrthof(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
#else
    glOrtho(0, width, height, 0, 4.0, 15.0);
#endif
    glMatrixMode(GL_MODELVIEW);

    repaint();
    */
}

//////////////////////////////////////////////////////////////////////////

void GLWidget::mousePressEvent( QMouseEvent *event )
{

}

//////////////////////////////////////////////////////////////////////////

void GLWidget::mouseMoveEvent( QMouseEvent *event )
{

}

//////////////////////////////////////////////////////////////////////////

void GLWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);

    painter.fillRect(event->rect(), QColor(Qt::black));

    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    
    QPen   penGrid(Qt::blue);
    painter.setPen(penGrid);

    for (int x = 0; x < width(); x+=16)
    {
        painter.drawLine(x, 0, x, height());
    }

    for (int y = 0; y < height(); y+=16)
    {
        painter.drawLine(0, y, width(), y);
    }
    
    painter.end();
}