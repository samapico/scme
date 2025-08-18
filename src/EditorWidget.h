#ifndef EDITORWIDGET_H
#define EDITORWIDGET_H

#include "Global.h"

#include <QtOpenGLWidgets/QOpenGLWidget>

#include <QtCore/QPoint>
#include <QtCore/QPointF>
#include <QtCore/QPointer>

#include <memory>


///////////////////////////////////////////////////////////////////////////

class QPropertyAnimation;
class QElapsedTimer;

///////////////////////////////////////////////////////////////////////////

namespace SCME {

//////////////////////////////////////////////////////////////////////////

class Editor;
class FrameCounter;


//////////////////////////////////////////////////////////////////////////

class EditorWidget : public QOpenGLWidget
{
    Q_OBJECT

    Q_PROPERTY(QPoint viewCenter READ viewCenter WRITE setViewCenter)
    Q_PROPERTY(QRect viewBounds READ viewBounds WRITE setViewBounds)
    Q_PROPERTY(QRect viewBoundsKeepZoom READ viewBounds WRITE setViewBoundsKeepZoom)

public:
    EditorWidget(Editor* editor, QWidget *parent = nullptr);
    virtual ~EditorWidget();

    /// Reimplemented from QWidget
    QSize minimumSizeHint() const;

    /// Reimplemented from QWidget
    QSize sizeHint() const;

    void zoomAt(const QPoint& center, float newZoomFactor);

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

    void setViewBoundsKeepZoom(const QRect& bounds);

    void setViewBoundsMaybeZoom(const QRect& bounds, bool keepZoom);

    void setViewBoundsSmooth(const QRect& bounds, bool forceFinishPreviousAnimation, bool keepZoom);


    void setViewCenter(const QPoint& centerPixel);

    void setViewCenterSmooth(const QPoint& centerPixel);


    void setZoomFactorSmooth(float factor);

    void setDefaultZoom();

signals:

    void viewMoved(const QRect& viewBounds);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

    /// Reimplemented from QWidget
    void mousePressEvent(QMouseEvent *event) override;

    /// Reimplemented from QWidget
    void mouseMoveEvent(QMouseEvent *event) override;

    /// Reimplemented from QWidget
    void mouseReleaseEvent(QMouseEvent *event) override;

    /// Reimplemented from QWidget
    void wheelEvent(QWheelEvent *event) override;

    /// Reimplemented from QWidget
    void paintEvent(QPaintEvent *event) override;

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

    QPointer<Editor> mEditor;

    QPointer<FrameCounter> mFrameCounter;

    QPoint mTopLeft{ 0,0 };

    float mZoomFactor = 1.0f;
    float mTargetZoomFactor = 1.0f;

    // = 0: no zoom
    // > 0: zoom in
    // < 0: zoom out
    int mZoomIndex = 0;

    /// temp pan
    QPoint mDragStart; //in screen coordinates
    QPoint mCenterOrig; //in level pixel coordinates
    bool   mDragging = false;

    std::unique_ptr<QPropertyAnimation> mSmoothView;
    std::unique_ptr<QElapsedTimer>      mLastSmoothViewStart;

    QPoint mCursor;
};


///////////////////////////////////////////////////////////////////////////

} // End namespace SCME

///////////////////////////////////////////////////////////////////////////


#endif // EDITORWIDGET_H
