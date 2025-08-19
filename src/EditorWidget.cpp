#include "EditorWidget.h"

#include <QtGui/QPaintEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <QtGui/QPainter>

#include <QtCore/QPropertyAnimation>
#include <QtCore/QElapsedTimer>
#include <QtCore/QTimer>

#include <QtCore/QDebug>

#include "Editor.h"
#include "FrameCounter.h"

#include <gl/GL.h>

///////////////////////////////////////////////////////////////////////////

#define NO_FPS_LIMIT


///////////////////////////////////////////////////////////////////////////

using namespace ::SCME;

//////////////////////////////////////////////////////////////////////////

EditorWidget::EditorWidget(Editor* editor, QWidget *parent) :
    QOpenGLWidget(parent),
    mEditor(editor),
    mTopLeft(0, 0),
    mZoomFactor(1),
    mTargetZoomFactor(1),
    mDragging(false),
    mDragStart(this, 0, 0)
{
    this->setMouseTracking(true);

    mFrameCounter = new FrameCounter(this);

    connect(mFrameCounter, &FrameCounter::framesCounted, this, [this](double fps)
        {
            //Force redraw
            update();
        });


#ifdef NO_FPS_LIMIT
    QTimer* timerMaxFps = new QTimer(this);
    timerMaxFps->setTimerType(Qt::TimerType::CoarseTimer);
    timerMaxFps->setInterval(0);
    timerMaxFps->setSingleShot(false);

    connect(timerMaxFps, &QTimer::timeout, this, [this]()
        {
            update();
        });

    timerMaxFps->start();
#endif
}

//////////////////////////////////////////////////////////////////////////

EditorWidget::~EditorWidget()
{
}

//////////////////////////////////////////////////////////////////////////

QSize EditorWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

//////////////////////////////////////////////////////////////////////////

QSize EditorWidget::sizeHint() const
{
    return QSize(400, 400);
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::initializeGL()
{
    QOpenGLWidget::initializeGL();

    //Background color
    glClearColor(0, 0, 0, 1);

    mFrameCounter->start();
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::paintGL()
{
    QOpenGLWidget::paintGL();

    QPainter painter;
    painter.begin(this);

    if (mEditor->level())
    {
        drawGrid(painter);

//#ifdef _DEBUG
        drawDebug(painter);
//#endif
    }

    painter.end();


    mFrameCounter->onFrameRendered();
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::resizeGL( int width, int height )
{
    emit viewMoved(viewBounds());

    QOpenGLWidget::resizeGL(width, height);
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
    //// Calculate aspect ratio
    //qreal aspect = qreal(w) / qreal(h ? h : 1);
    //
    //// Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    //const qreal zNear = 3.0, zFar = 7.0, fov = 45.0;
    //
    //// Reset projection
    //projection.setToIdentity();
    //
    //// Set perspective projection
    //projection.perspective(fov, aspect, zNear, zFar);
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::mousePressEvent(QMouseEvent *event)
{
    if (!mEditor->level())
        return;

    mDragStart = ScreenCoords(this, event);
    mCenterOrig = viewCenter();
    mDragging  = true;
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::mouseMoveEvent(QMouseEvent *event)
{
    mCursor = ScreenCoords(this, event).toLevel();

    if (mDragging && mEditor->level())
    {
        //qDebug() << "Drag@ " << event->position();
        setViewCenterSmooth(mEditor->boundPixelToLevel(LevelCoords(mCenterOrig - ((event->pos() - mDragStart) / mZoomFactor))));
    }
    else
    {
        update();
    }
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::wheelEvent(QWheelEvent *event)
{
    if (!mEditor->level())
        return;

    if (mDragging)
        return;

    int scrollDelta = event->hasPixelDelta() ? event->pixelDelta().manhattanLength() : event->angleDelta().y();

    //qDebug() << "Wheel@" << event->position() << "; pixelDelta=" << event->pixelDelta() << "; angleDelta=" << event->angleDelta();

    if (scrollDelta > 0) //zoom in
    {
        mZoomIndex = std::min(mZoomIndex + 1, mEditor->config().zoomIndexMax());
    }
    else if (scrollDelta < 0) //zoom out
    {
        mZoomIndex = std::max(mZoomIndex - 1, mEditor->config().zoomIndexMin());
    }

    float zoomMultiplier = mEditor->config().zoomFactorAtIndex(mZoomIndex);
    qDebug() << "zoomIndex=" << mZoomIndex << "; zoom=" << zoomMultiplier;

    zoomAt(LevelCoords(ScreenCoords(this, event)), zoomMultiplier);
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!mEditor->level())
        return;

    if (mDragging)
    {
        if (mEditor->config().smoothDragSpeed() > 0)
        {
            qDebug() << "Release@ " << event->position();
        }
        else if (mSmoothView && mSmoothView->state() == QAbstractAnimation::Running)
        {
            //Abort the last animation
            mSmoothView->stop();
        }
    }
    mDragging = false;
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::paintEvent(QPaintEvent *event)
{
    QOpenGLWidget::paintEvent(event);
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::drawGrid(QPainter& painter)
{
    painter.setRenderHint(QPainter::Antialiasing, false);

    QSize levelSize = mEditor->levelSize();

    //visible level bounds
    LevelBounds visibleLevelBounds = viewBounds();

    LevelBounds entireLevelBounds = mEditor->levelBounds();
    ScreenCoords levelTopLeftInScreen = entireLevelBounds.topLeft().toScreen(this);
    ScreenCoords levelBottomRightInScreen = entireLevelBounds.bottomRight().toScreen(this);

    //screen bounds in which the grid is to be drawn
    QRect screenBounds(QPoint(0, 0), QSize(QWidget::size()));
    if (visibleLevelBounds.left() < 0)
        screenBounds.setLeft(levelTopLeftInScreen.x());
    if (visibleLevelBounds.top() < 0)
        screenBounds.setTop(levelTopLeftInScreen.y());

    if (visibleLevelBounds.right() > entireLevelBounds.width())
        screenBounds.setRight(levelBottomRightInScreen.x());
    if (visibleLevelBounds.bottom() > entireLevelBounds.height())
        screenBounds.setBottom(levelBottomRightInScreen.y());

    //Bound to level limits
    visibleLevelBounds = visibleLevelBounds.intersected(mEditor->levelBounds());

    LevelCoords visibleTopLeft = visibleLevelBounds.topLeft();
    LevelCoords visibleBottomRight = visibleLevelBounds.bottomRight();

    //first visible complete tile
    int firstTiley = std::ceil(visibleTopLeft.tileYf());
    Q_ASSERT(firstTiley >= 0);
    if (firstTiley < 0)
        firstTiley = 0;

    int lastTiley = visibleBottomRight.tileY();
    Q_ASSERT(lastTiley <= levelSize.width());
    if (lastTiley > levelSize.width())
        lastTiley = levelSize.width();

    int firstTilex = std::ceil(visibleTopLeft.tileXf());
    Q_ASSERT(firstTilex >= 0);
    if (firstTilex < 0)
        firstTilex = 0;

    int lastTilex = visibleBottomRight.tileX();
    Q_ASSERT(lastTilex <= levelSize.height());
    if (lastTilex > levelSize.height())
        lastTilex = levelSize.height();

    float pixelsPerTileX = TILE_W * mZoomFactor;
    float pixelsPerTileY = TILE_H * mZoomFactor;


    LevelCoords tile = LevelCoords::fromTile(firstTilex, firstTiley);

    //draw horizontal grid lines
    for (int tiley = firstTiley; tiley <= lastTiley; tiley++)
    {
        tile.setY(LevelCoords::tileToPixel(tiley, TILE_H));

        int screeny = tile.toScreen(this).y();

        const QPen* pPen = mEditor->config().getGridPen(tiley, pixelsPerTileY);
        if (pPen)
        {
            painter.setPen(*pPen);
            painter.drawLine(screenBounds.left(), screeny, screenBounds.right(), screeny);
        }
    }

    //draw vertical grid lines
    for (int tilex = firstTilex; tilex <= lastTilex; tilex++)
    {
        tile.setX(LevelCoords::tileToPixel(tilex, TILE_W));

        int screenx = tile.toScreen(this).x();

        const QPen* pPen = mEditor->config().getGridPen(tilex, pixelsPerTileX);
        if (pPen)
        {
            painter.setPen(*pPen);
            painter.drawLine(screenx, screenBounds.top(), screenx, screenBounds.bottom());
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::drawDebug(QPainter& painter)
{
    QString str;

    painter.setPen(QColor(Qt::green));
    str = QString("(%1,%2)x%3/%4 - %5FPS").arg(
        QString::number(mTopLeft.x()),
        QString::number(mTopLeft.y()),
        QString::number(mZoomFactor, 'f', 6),
        QString::number(mTargetZoomFactor, 'f', 6),
        QString::number(mFrameCounter->lastFPS(), 'f', 1));

    painter.drawText(0, 10, str);

    str = QString("(%1,%2)").arg(
        QString::number(mCursor.x()),
        QString::number(mCursor.y()));

    painter.drawText(0, 30, str);
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::zoomAt(const LevelCoords& center, float newZoomFactor)
{
    //make sure the level is still in the view
    LevelCoords targetCenter = mEditor->boundPixelToLevel(center);
    ScreenCoords screenTarget(this, targetCenter);

    //upper zoom limit
    if (newZoomFactor > mEditor->config().maxZoomFactor())
        newZoomFactor = mEditor->config().maxZoomFactor();

    //lower limit of zoom
    if (newZoomFactor < mEditor->config().minZoomFactor())
        newZoomFactor = mEditor->config().minZoomFactor();

    mTargetZoomFactor = newZoomFactor;

    LevelBounds newView = LevelBounds(LevelCoords(targetCenter - (screenTarget / newZoomFactor)), QWidget::size()/newZoomFactor);

    //Make sure the view's center is in the level
    LevelCoords boundedViewCenter = mEditor->boundPixelToLevel(newView.center());

    newView.translate(boundedViewCenter - newView.center());

    setViewBoundsSmooth(newView, false, false);
}

//////////////////////////////////////////////////////////////////////////

LevelBounds EditorWidget::viewBounds() const
{
    return LevelBounds::fromTopLeftAndZoom(mTopLeft, QWidget::size(), mZoomFactor);
}

//////////////////////////////////////////////////////////////////////////

LevelCoords EditorWidget::viewCenter() const
{
    return viewBounds().center();
}

//////////////////////////////////////////////////////////////////////////

const LevelCoords& EditorWidget::viewTopLeft() const
{
    return mTopLeft;
}

///////////////////////////////////////////////////////////////////////////

float EditorWidget::zoomFactor() const
{
    return mZoomFactor;
}

///////////////////////////////////////////////////////////////////////////

void EditorWidget::setViewBounds(const LevelBounds& bounds)
{
    mTopLeft = bounds.topLeft();

    int wh = QWidget::height();

    mZoomFactor = (float)wh/(float)bounds.height();

    float zoomFactorApproxA = (float)wh / (float)(bounds.height() - 1);
    float zoomFactorApproxB = (float)wh / (float)(bounds.height() + 1);

    if (mTargetZoomFactor >= zoomFactorApproxB && mTargetZoomFactor <= zoomFactorApproxA)
    {
        //Current zoom is as close as possible to the target zoom, force it to be equal
        mZoomFactor = mTargetZoomFactor;
    }

    if (mTargetZoomFactor == 1.0f && mZoomFactor != 1.0f)
    {
        int x = 8;
        x++;
    }

    if (updatesEnabled())
    {
        update();
        emit viewMoved(viewBounds());
    }
}

///////////////////////////////////////////////////////////////////////////

void EditorWidget::setViewBoundsKeepZoom(const LevelBounds& bounds)
{
    mTopLeft = bounds.topLeft();

    if (updatesEnabled())
    {
        update();
        emit viewMoved(viewBounds());
    }
}

///////////////////////////////////////////////////////////////////////////

void EditorWidget::setViewBoundsMaybeZoom(const LevelBounds& bounds, bool keepZoom)
{
    if (keepZoom)
        setViewBoundsKeepZoom(bounds);
    else
        setViewBounds(bounds);
}

///////////////////////////////////////////////////////////////////////////

void EditorWidget::setViewBoundsSmooth(const LevelBounds& bounds, bool forceFinishPreviousAnimation, bool keepZoom)
{
    if (mSmoothView && mSmoothView->state() == QAbstractAnimation::Running)
    {
        if (forceFinishPreviousAnimation)
            setViewBoundsMaybeZoom(LevelBounds(mSmoothView->endValue().toRectF()), mSmoothView->property("keepZoom").toBool()); //Jump to the end of the previous transition

        mSmoothView->stop();
    }

    int duration = mEditor->config().smoothCameraTime();

    if (!duration)
    {
        // 0 ms duration; do not bother using an animation
        setViewBoundsMaybeZoom(bounds, keepZoom);
        return;
    }

    mSmoothView = std::make_unique<QPropertyAnimation>(this, keepZoom ? "viewBoundsKeepZoom" : "viewBounds");

    mSmoothView->setEasingCurve(QEasingCurve(QEasingCurve::OutCubic));
    mSmoothView->setDuration(mEditor->config().smoothCameraTime());
    mSmoothView->setStartValue(viewBounds());
    mSmoothView->setEndValue(bounds);

    //If true, the zoom will not be recomputed from the bounds
    mSmoothView->setProperty("keepZoom", keepZoom);

    mSmoothView->start();
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::setViewTopLeft(const LevelCoords& topLeft)
{
    mTopLeft = topLeft;

    if (updatesEnabled())
    {
        update();
        emit viewMoved(viewBounds());
    }
}

///////////////////////////////////////////////////////////////////////////

void EditorWidget::setViewTopLeftSmooth(const LevelCoords& topLeft)
{
    setViewBoundsSmooth(LevelBounds(topLeft, QWidget::size()/mZoomFactor), true, true);
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::setViewCenter(const LevelCoords& centerPixel)
{
    mTopLeft.setX(centerPixel.x() - QWidget::width() /(mZoomFactor*2));
    mTopLeft.setY(centerPixel.y() - QWidget::height()/(mZoomFactor*2));

    if (updatesEnabled())
    {
        update();
        emit viewMoved(viewBounds());
    }
}

///////////////////////////////////////////////////////////////////////////

void EditorWidget::setViewCenterSmooth(const LevelCoords& centerPixel)
{
    /// The thumbnail widget calls this method on mouse move events, which happen
    /// really fast if you're dragging. If we use 'true' to jump to the end
    /// of the previous animation, we get weird jumps if you are simply clicking around.
    /// If we use 'false', the view does not move when you're dragging because the animation
    /// gets cancelled before it has time to move at all.
    /// Ugly solution is to use 'false', except if the animation has not been running for long enough
    bool bFinishPreviousAnimation = false;
    if (!mLastSmoothViewStart)
    {
        mLastSmoothViewStart = std::make_unique<QElapsedTimer>();
        mLastSmoothViewStart->start();
    }
    else
    {
        qint64 dt = mLastSmoothViewStart->restart();

        bFinishPreviousAnimation = (dt < 100); ///< 10ms minimum animation time?
    }

    setViewBoundsSmooth(LevelBounds::fromCenterAndZoom(centerPixel, size(), mZoomFactor), bFinishPreviousAnimation, true);
}

///////////////////////////////////////////////////////////////////////////

void EditorWidget::setDefaultZoom()
{
    mZoomIndex = 0;
    mZoomFactor = 1.0f;
    mTargetZoomFactor = 1.0f;

    if (updatesEnabled())
    {
        update();
        emit viewMoved(viewBounds());
    }
}

///////////////////////////////////////////////////////////////////////////

void EditorWidget::setZoomFactorSmooth(float factor)
{
    setViewBoundsSmooth(LevelBounds::fromCenterAndZoom(viewCenter(), size(), factor), true, false);
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::alignView(const ScreenCoords& screenPixel, const LevelCoords& levelPixel)
{
     mTopLeft = LevelCoords(QPointF(levelPixel) - (QPointF(screenPixel) / mZoomFactor));

     //make sure the level is still in the view
     LevelCoords center = viewCenter();
     LevelCoords boundedCenter = mEditor->boundPixelToLevel(center);

     if (center != boundedCenter)
         setViewCenter(boundedCenter);
     else if (updatesEnabled())
     {
         update();
         emit viewMoved(viewBounds());
     }
}
