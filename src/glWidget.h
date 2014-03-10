#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtOpenGL/QGLWidget>

//////////////////////////////////////////////////////////////////////////

class Editor;

//////////////////////////////////////////////////////////////////////////

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(Editor* editor, QWidget *parent = 0);
    ~GLWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    void Test();

protected:
    void initializeGL();
    //void paintGL();
    void resizeGL(int width, int height);
    
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:

    Editor* mEditor;

    int mTop;
    int mLeft;

    float mZoomFactor;
};

#endif // GLWIDGET_H
