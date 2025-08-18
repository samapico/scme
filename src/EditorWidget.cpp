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
    mDragging(false)
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

    mDragStart = event->pos();
    mCenterOrig = viewCenter();
    mDragging  = true;
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::mouseMoveEvent(QMouseEvent *event)
{
    mCursor = screenToLevelPixel(event->pos());

    if (mDragging && mEditor->level())
    {
        //qDebug() << "Drag@ " << event->position();
        setViewCenterSmooth(mEditor->boundPixelToLevel(mCenterOrig - ((event->pos() - mDragStart) / mZoomFactor)));
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

    zoomAt(screenToLevelPixel(event->position().toPoint()), zoomMultiplier);
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
    QSize levelPxSize = mEditor->levelPixelSize();

    //visible level bounds
    QRect visibleLevelBounds = viewBounds();

    //screen bounds in which the grid is to be drawn
    QRect screenBounds(QPoint(0, 0), QSize(QWidget::size()));
    if (visibleLevelBounds.left() < 0)
        screenBounds.setLeft(levelPixelToScreenX(0));
    if (visibleLevelBounds.top() < 0)
        screenBounds.setTop(levelPixelToScreenY(0));

    if (visibleLevelBounds.right() > levelPxSize.width())
        screenBounds.setRight(levelPixelToScreenX(levelPxSize.width()));
    if (visibleLevelBounds.bottom() > levelPxSize.height())
        screenBounds.setBottom(levelPixelToScreenY(levelPxSize.height()));

    //draw horizontal grid lines
    int firstTiley = mEditor->pixelToTileY(visibleLevelBounds.top());
    if (firstTiley < 0)
        firstTiley = 0;

    int lastTiley  = mEditor->pixelToTileY(visibleLevelBounds.bottom());
    if (lastTiley > levelSize.width())
        lastTiley = levelSize.width();

    float pixelsPerTileX = Editor::TILE_WIDTH * mZoomFactor;
    float pixelsPerTileY = Editor::TILE_HEIGHT * mZoomFactor;

    int minGridSizeX = std::floorf(4.0f / mZoomFactor / Editor::TILE_WIDTH);
    int minGridSizeY = std::floorf(4.0f / mZoomFactor / Editor::TILE_HEIGHT);

    for (int tiley = firstTiley; tiley <= lastTiley; tiley++)
    {
        int screeny = levelPixelToScreenY(mEditor->tileToPixelY(tiley));

        int currentGridSize;

        const QPen* pPen = mEditor->config().getGridPen(tiley, pixelsPerTileY);
        if (pPen)
        {
            painter.setPen(*pPen);
            painter.drawLine(screenBounds.left(), screeny, screenBounds.right(), screeny);
        }
    }

    //draw vertical grid lines
    int firstTilex = mEditor->pixelToTileX(visibleLevelBounds.left());
    if (firstTilex < 0)
        firstTilex = 0;

    int lastTilex  = mEditor->pixelToTileX(visibleLevelBounds.right());
    if (lastTilex > levelSize.height())
        lastTilex = levelSize.height();

    for (int tilex = firstTilex; tilex <= lastTilex; tilex++)
    {
        int screenx = levelPixelToScreenX(mEditor->tileToPixelX(tilex));

        int currentGridSize;

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

void EditorWidget::zoomAt(const QPoint& center, float newZoomFactor)
{
    //make sure the level is still in the view
    QPoint targetCenter = mEditor->boundPixelToLevel(center);
    QPoint screenTarget = levelPixelToScreen(targetCenter);

    //upper zoom limit
    if (newZoomFactor > mEditor->config().maxZoomFactor())
        newZoomFactor = mEditor->config().maxZoomFactor();

    //lower limit of zoom
    if (newZoomFactor < mEditor->config().minZoomFactor())
        newZoomFactor = mEditor->config().minZoomFactor();

    mTargetZoomFactor = newZoomFactor;

    QRect newView = QRect(targetCenter - (screenTarget / newZoomFactor), QWidget::size()/newZoomFactor);

    //Make sure the view's center is in the level
    QPoint boundedViewCenter = mEditor->boundPixelToLevel(newView.center());

    newView.translate(boundedViewCenter - newView.center());

    setViewBoundsSmooth(newView, false, false);
}

//////////////////////////////////////////////////////////////////////////

QRect EditorWidget::viewBounds() const
{
    return calcBoundsFromTopLeftAndZoom(mTopLeft, mZoomFactor);
}

//////////////////////////////////////////////////////////////////////////

QPoint EditorWidget::viewCenter() const
{
    return viewBounds().center();
}

///////////////////////////////////////////////////////////////////////////

float EditorWidget::zoomFactor() const
{
    return mZoomFactor;
}

///////////////////////////////////////////////////////////////////////////

void EditorWidget::setViewBounds(const QRect& bounds)
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

void EditorWidget::setViewBoundsKeepZoom(const QRect& bounds)
{
    mTopLeft = bounds.topLeft();
    //mZoomFactor = (float)QWidget::height() / (float)bounds.height();

    if (updatesEnabled())
    {
        update();
        emit viewMoved(viewBounds());
    }
}

///////////////////////////////////////////////////////////////////////////

void EditorWidget::setViewBoundsMaybeZoom(const QRect& bounds, bool keepZoom)
{
    if (keepZoom)
        setViewBoundsKeepZoom(bounds);
    else
        setViewBounds(bounds);
}

///////////////////////////////////////////////////////////////////////////

void EditorWidget::setViewBoundsSmooth(const QRect& bounds, bool forceFinishPreviousAnimation, bool keepZoom)
{
    if (mSmoothView && mSmoothView->state() == QAbstractAnimation::Running)
    {
        if (forceFinishPreviousAnimation)
            setViewBoundsMaybeZoom(mSmoothView->endValue().toRect(), mSmoothView->property("keepZoom").toBool()); //Jump to the end of the previous transition

        mSmoothView->stop();
    }

    int duration = mEditor->config().smoothCameraTime();

    if (!duration)
    {
        // 0 ms duration; do not bother using an animation
        setViewBoundsMaybeZoom(bounds, keepZoom);
        return;
    }

    //if (!mSmoothView)
    {
        mSmoothView = std::make_unique<QPropertyAnimation>(this, keepZoom ? "viewBoundsKeepZoom" : "viewBounds");
    }

    mSmoothView->setEasingCurve(QEasingCurve(QEasingCurve::OutCubic));
    mSmoothView->setDuration(mEditor->config().smoothCameraTime());
    mSmoothView->setStartValue(viewBounds());
    mSmoothView->setEndValue(bounds);

    //If true, the zoom will not be recomputed from the bounds
    mSmoothView->setProperty("keepZoom", keepZoom);

    mSmoothView->start();
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::setViewTopLeft(const QPoint& topLeft)
{
    mTopLeft = topLeft;

    if (updatesEnabled())
    {
        update();
        emit viewMoved(viewBounds());
    }
}

///////////////////////////////////////////////////////////////////////////

void EditorWidget::setViewTopLeftSmooth(const QPoint& topLeft)
{
    setViewBoundsSmooth(QRect(topLeft, QWidget::size()/mZoomFactor), true, true);
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::setViewCenter(const QPoint& centerPixel)
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

void EditorWidget::setViewCenterSmooth(const QPoint& centerPixel)
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

    setViewBoundsSmooth(calcBoundsFromCenterAndZoom(centerPixel, mZoomFactor), bFinishPreviousAnimation, true);
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
    setViewBoundsSmooth(calcBoundsFromCenterAndZoom(viewCenter(), factor), true, false);
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::alignView(const QPoint& screenPixel, const QPoint& levelPixel)
{
     mTopLeft = levelPixel - (screenPixel / mZoomFactor);

     //make sure the level is still in the view
     QPoint center = viewCenter();
     QPoint boundedCenter = mEditor->boundPixelToLevel(center);

     if (center != boundedCenter)
         setViewCenter(boundedCenter);
     else if (updatesEnabled())
     {
         update();
         emit viewMoved(viewBounds());
     }
}

///////////////////////////////////////////////////////////////////////////

QRect EditorWidget::calcBoundsFromCenterAndZoom(const QPoint& c, float _zoomFactor) const
{
    QPoint halfSize = QPoint(QWidget::width(), QWidget::height())/(_zoomFactor*2);

    return QRect(c - halfSize, c + halfSize);
}