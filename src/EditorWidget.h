#ifndef EDITORWIDGET_H
#define EDITORWIDGET_H

#include "Global.h"

#include <QtOpenGLWidgets/QOpenGLWidget>

#include <QtCore/QPoint>
#include <QtCore/QPointF>
#include <QtCore/QPointer>

#include "Coords.h"

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

    //Properties use basic types to make animation interpolation easier
    Q_PROPERTY(QPointF viewCenter READ viewCenterProp WRITE setViewCenterProp)
    Q_PROPERTY(QRectF viewBounds READ viewBoundsProp WRITE setViewBoundsProp)
    Q_PROPERTY(QRectF viewBoundsKeepZoom READ viewBoundsProp WRITE setViewBoundsKeepZoomProp)

public:
    EditorWidget(Editor* editor, QWidget *parent = nullptr);
    virtual ~EditorWidget();

    /// Reimplemented from QWidget
    QSize minimumSizeHint() const;

    /// Reimplemented from QWidget
    QSize sizeHint() const;

    void zoomAt(const LevelCoords& center, float newZoomFactor);

    LevelBounds viewBounds() const;
    inline QRectF viewBoundsProp() const { return viewBounds(); }

    LevelCoords viewCenter() const;
    inline QPointF viewCenterProp() const { return viewCenter(); }

    const LevelCoords& viewTopLeft() const;

    float zoomFactor() const;

    void alignView(const ScreenCoords& screenPixel, const LevelCoords& levelPixel);

public slots:

    void setViewTopLeft(const LevelCoords& topLeft);

    void setViewTopLeftSmooth(const LevelCoords& topLeft);

    /// This adjusts both the pan and the zoom.
    /// The "right" element of the rect is ignored, as the height is used to determine the zoom.
    void setViewBounds(const LevelBounds& bounds);
    inline void setViewBoundsProp(const QRectF& bounds) { setViewBounds(LevelBounds(bounds)); }

    void setViewBoundsKeepZoom(const LevelBounds& bounds);
    inline void setViewBoundsKeepZoomProp(const QRectF& bounds) { setViewBoundsKeepZoom(LevelBounds(bounds)); }

    void setViewBoundsMaybeZoom(const LevelBounds& bounds, bool keepZoom);

    void setViewBoundsSmooth(const LevelBounds& bounds, bool forceFinishPreviousAnimation, bool keepZoom);


    void setViewCenter(const LevelCoords& centerPixel);
    inline void setViewCenterProp(const QPointF& centerPixel) { setViewCenter(LevelCoords(centerPixel)); }

    void setViewCenterSmooth(const LevelCoords& centerPixel);


    void setZoomFactorSmooth(float factor);

    void setDefaultZoom();

signals:

    void viewMoved(const LevelBounds& viewBounds);

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

private:

    QPointer<Editor> mEditor;

    QPointer<FrameCounter> mFrameCounter;

    LevelCoords mTopLeft{ 0,0 };

    float mZoomFactor = 1.0f;
    float mTargetZoomFactor = 1.0f;

    // = 0: no zoom
    // > 0: zoom in
    // < 0: zoom out
    int mZoomIndex = 0;

    /// temp pan
    ScreenCoords mDragStart; //in screen coordinates
    LevelCoords mCenterOrig; //in level pixel coordinates
    bool   mDragging = false;

    std::unique_ptr<QPropertyAnimation> mSmoothView;
    std::unique_ptr<QElapsedTimer>      mLastSmoothViewStart;

    LevelCoords mCursor;
};


///////////////////////////////////////////////////////////////////////////

} // End namespace SCME

///////////////////////////////////////////////////////////////////////////


#endif // EDITORWIDGET_H
