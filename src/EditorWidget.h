#ifndef EDITORWIDGET_H
#define EDITORWIDGET_H

#include <QtOpenGL/QGLWidget>

#include <QtCore/QPoint>

//////////////////////////////////////////////////////////////////////////

class Editor;

//////////////////////////////////////////////////////////////////////////

class EditorWidget : public QGLWidget
{
    Q_OBJECT

public:
    EditorWidget(Editor* editor, QWidget *parent = 0);
    ~EditorWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    void  setZoom(float zoom, bool redraw = true);

    void  zoomInAt(const QPoint& center, float zoomMultiplier, bool redraw = true);

    void  zoomOutAt(const QPoint& center, float zoomDivider, bool redraw = true);

    QRect  getViewBounds() const;
    QPoint getViewCenter() const;

    void  setTopLeft(const QPoint& topLeft, bool redraw = true);
    void  setCenter(const QPoint& centerPixel, bool redraw = true);
    void  alignView(const QPoint& screenPixel, const QPoint& levelPixel, bool redraw = true);

signals:

    void  viewMoved(const QRect& viewBounds);

protected:
    void initializeGL();
    //void paintGL();
    void resizeGL(int width, int height);
    
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    void paintEvent(QPaintEvent *event);

    void drawDebug(QPainter& painter);

    void drawGrid(QPainter& painter);

    void drawObjects(QPainter& painter);

private:

    Editor* mEditor;

    QPoint mTopLeft;

    float mZoomFactor;

    /// temp pan
    QPoint mDragStart;
    QPoint mCenterOrig;
    bool   mDragging;

    int levelPixelToScreenX(int x) const    { return (x - mTopLeft.x())*mZoomFactor; }
    int levelPixelToScreenY(int y) const    { return (y - mTopLeft.y())*mZoomFactor; }

    QPoint levelPixelToScreen(const QPoint& xy) const     { return (xy - mTopLeft)*mZoomFactor; }

    int screenToLevelPixelX(int screenx) const    { return (screenx / mZoomFactor) + mTopLeft.x(); }
    int screenToLevelPixelY(int screeny) const    { return (screeny / mZoomFactor) + mTopLeft.y(); }

    QPoint screenToLevelPixel(const QPoint& screenxy) const     { return (screenxy / mZoomFactor) + mTopLeft; }
};

#endif // EDITORWIDGET_H
