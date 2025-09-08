#ifndef INC_EditorWidget_H
#define INC_EditorWidget_H

#include "UiGlobal.h"

#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QtOpenGL/QOpenGLFunctions_3_3_Core>

#include <QtCore/QParallelAnimationGroup>
#include <QtCore/QPoint>
#include <QtCore/QPointF>
#include <QtCore/QPointer>

#include "ScreenCoords.h"
#include "ViewBounds.h"

#include <memory>


///////////////////////////////////////////////////////////////////////////

class QPropertyAnimation;
class QElapsedTimer;

///////////////////////////////////////////////////////////////////////////

namespace SCME {

//////////////////////////////////////////////////////////////////////////

class Editor;
class FrameCounter;
class TileRenderer;
class MinimapRenderer;
class LevelData;


//////////////////////////////////////////////////////////////////////////

class SCME_UI_DLL EditorWidget : public QOpenGLWidget
{
    Q_OBJECT

    //Properties use basic types to make animation interpolation easier
    Q_PROPERTY(SmoothViewBounds viewBoundsAndZoom READ viewBoundsAndZoom WRITE setViewBoundsAndZoom)

public:
    EditorWidget(Editor* editor, QWidget *parent = nullptr);

    virtual ~EditorWidget();

    std::shared_ptr<LevelData> level() const;

    /// Reimplemented from QWidget
    QSize minimumSizeHint() const;

    /// Reimplemented from QWidget
    QSize sizeHint() const;

    /// Centers view at \a center
    void zoomAt(const LevelCoords& center, float newZoomFactor);

    void zoomTowards(const LevelCoords& targetLevel, const ScreenCoords& screenTarget, float newZoomFactor);

    LevelBounds viewBounds() const;

    SmoothViewBounds viewBoundsAndZoom() const;

    LevelCoords viewCenter() const;

    LevelCoords viewTopLeft() const;

    float zoomFactor() const;

    ScreenCoords screenCenter() const;

    static ScreenCoords boundScreenPixelToLevel(const LevelData* pLevel, const ScreenCoords& screenPixel);

public slots:

    void setViewCenter(const LevelCoords& centerPixel);

    void setZoomFactor(float zoomFactor);

    void setDefaultZoom();

    void setViewCenterSmooth(const LevelCoords& targetCenter);

    void setViewTargetSmooth(const LevelCoords& targetLevel, const ScreenCoords& targetScreen);

    void setViewTargetAndZoomSmooth(const LevelCoords& targetLevel, const ScreenCoords& targetScreen, float targetZoom);

    void setViewBoundsAndZoom(const SmoothViewBounds& boundsAndZoom);

    void onLevelChanged();

    void onTilesetChanged();

    /// @todo Remove this when we have an actual selectable tileset
    void setCurrentTileId(TileId id) { mCurrentTileId = id; }

signals:

    void viewMoved(const LevelBounds& viewBounds);

    void levelTilesChanged(const LevelData* level);

    void levelTilesChangedArea(const LevelData* level, const LevelBounds& bounds);

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

    void drawDebug(QPainter& painter, const LevelData* pLevel);

    void drawGrid(QPainter& painter, const LevelData* pLevel);

    void drawObjects(QPainter& painter, const LevelData* pLevel);

private:

    void onTilesChanged(const LevelData* level);

    void onTilesChangedArea(const LevelData* level, const LevelBounds& bounds);

    QPointer<Editor> mEditor;

    QPointer<FrameCounter> mFrameCounter;

    //LevelCoords mTopLeft{ 0,0 };
    LevelCoords mCenter{ 0,0 };

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

    QPointer<QPropertyAnimation> mSmoothView;
    bool mSmoothViewStopPan = false;

    std::unique_ptr<TileRenderer> mTileRenderer;
    std::unique_ptr<MinimapRenderer> mMinimapRenderer;

    LevelCoords mCursor;

    TileId mCurrentTileId = 1;
};


///////////////////////////////////////////////////////////////////////////

} // End namespace SCME

///////////////////////////////////////////////////////////////////////////


#endif // INC_EditorWidget_H
