#ifndef EDITORWIDGET_H
#define EDITORWIDGET_H

#include "Global.h"

#include <QtOpenGL/QGLWidget>

#include <QtCore/QPoint>

///////////////////////////////////////////////////////////////////////////

class QPropertyAnimation;
class QElapsedTimer;

///////////////////////////////////////////////////////////////////////////

namespace SCME {

//////////////////////////////////////////////////////////////////////////

class Editor;

//////////////////////////////////////////////////////////////////////////

class EditorWidget : public QGLWidget
{
    Q_OBJECT

    Q_PROPERTY(float zoomFactor READ zoomFactor WRITE setZoomFactor RESET setDefaultZoom)
    Q_PROPERTY(QPoint viewCenter READ viewCenter WRITE setViewCenter)
    Q_PROPERTY(QRect viewBounds READ viewBounds WRITE setViewBounds)
public:
    EditorWidget(Editor* editor, QWidget *parent = 0);
    virtual ~EditorWidget();

    /// Reimplemented from QWidget
    QSize minimumSizeHint() const;

    /// Reimplemented from QWidget
    QSize sizeHint() const;

    void zoomAt(const QPoint& center, float zoomMultiplier);

    QRect viewBounds() const;

    QPoint viewCenter() const;

    float zoomFactor() const;

    void alignView(const QPoint& screenPixel, const QPoint& levelPixel);

public slots:

    void setViewTopLeft(const QPoint& topLeft);
    
    void setViewTopLeftSmooth(const QPoint& topLeft);

    /// This adjusts both the pan and the zoom.
    /// The "right" element of the rect is ignored, as the height is used to determine the zoom.
    void setViewBounds(const QRect& bounds);

    void setViewBoundsSmooth(const QRect& bounds, bool forceFinishPreviousAnimation = true);


    void setViewCenter(const QPoint& centerPixel);
    
    void setViewCenterSmooth(const QPoint& centerPixel);
    

    void setZoomFactor(float factor);
    
    void setZoomFactorSmooth(float factor);
    
    void setDefaultZoom();

signals:

    void viewMoved(const QRect& viewBounds);

protected:
    void initializeGL();
    //void paintGL();
    void resizeGL(int width, int height);
    
    /// Reimplemented from QWidget
    void mousePressEvent(QMouseEvent *event);

    /// Reimplemented from QWidget
    void mouseMoveEvent(QMouseEvent *event);

    /// Reimplemented from QWidget
    void mouseReleaseEvent(QMouseEvent *event);

    /// Reimplemented from QWidget
    void wheelEvent(QWheelEvent *event);

    /// Reimplemented from QWidget
    void paintEvent(QPaintEvent *event);

    void drawDebug(QPainter& painter);

    void drawGrid(QPainter& painter);

    void drawObjects(QPainter& painter);

    QRect calcBoundsFromCenterAndZoom(const QPoint& c, float _zoomFactor) const;

    inline QRect calcBoundsFromTopLeftAndZoom(const QPoint& topLeft, float _zoomFactor) const  { return QRect(topLeft, QWidget::size()/_zoomFactor); }

    inline int levelPixelToScreenX(int x) const    { return (x - mTopLeft.x())*mZoomFactor; }
    inline int levelPixelToScreenY(int y) const    { return (y - mTopLeft.y())*mZoomFactor; }

    inline QPoint levelPixelToScreen(const QPoint& xy) const     { return (xy - mTopLeft)*mZoomFactor; }

    inline int screenToLevelPixelX(int screenx) const    { return (screenx / mZoomFactor) + mTopLeft.x(); }
    inline int screenToLevelPixelY(int screeny) const    { return (screeny / mZoomFactor) + mTopLeft.y(); }

    inline QPoint screenToLevelPixel(const QPoint& screenxy) const     { return (screenxy / mZoomFactor) + mTopLeft; }

private:

    Editor* mEditor;

    QPoint mTopLeft;

    float mZoomFactor;
    float mTargetZoomFactor;

    /// temp pan
    QPoint mDragStart;
    QPoint mCenterOrig;
    bool   mDragging;

    QPropertyAnimation* mSmoothView;
    QElapsedTimer*      mLastSmoothViewStart;

    QPoint mCursor;
};


///////////////////////////////////////////////////////////////////////////

} // End namespace SCME

///////////////////////////////////////////////////////////////////////////


#endif // EDITORWIDGET_H
