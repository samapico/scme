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

//#define NO_FPS_LIMIT


///////////////////////////////////////////////////////////////////////////

using namespace ::SCME;

//////////////////////////////////////////////////////////////////////////

EditorWidget::EditorWidget(Editor* editor, QWidget *parent) :
    QOpenGLWidget(parent),
    mEditor(editor),
    mCenter(0, 0),
    mZoomFactor(1),
    mTargetZoomFactor(1),
    mDragging(false),
    mDragStart(this, 0, 0),
    mSmoothView(new QPropertyAnimation(this, "viewBoundsAndZoom", this))
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

    /// @todo Start drag with specific mouse button(s)

    mDragStart = ScreenCoords(this, event);
    mCenterOrig = viewCenter();
    mDragging  = true;
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::mouseMoveEvent(QMouseEvent *event)
{
    mCursor = mEditor->boundPixelToLevel(ScreenCoords(this, event).toLevel());

    if (mDragging && mEditor->level())
    {
        //qDebug() << "Drag@ " << event->position();
        setViewCenter(mEditor->boundPixelToLevel(LevelCoords(mCenterOrig - ((event->position() - mDragStart) / mZoomFactor))));
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

    ScreenCoords screenTarget(this, event);

    //zoomAt(LevelCoords(ScreenCoords(this, event)), zoomMultiplier);
    zoomTowards(LevelCoords(screenTarget), mEditor->boundScreenPixelToLevel(screenTarget), zoomMultiplier);
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
            //qDebug() << "Release@ " << event->position();
        }

        mDragging = false;
    }
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

        QPen pen = mEditor->config().getGridPen(tiley, pixelsPerTileY);
        if (pen.color().alpha() > 0)
        {
            painter.setPen(pen);
            painter.drawLine(screenBounds.left(), screeny, screenBounds.right(), screeny);
        }
    }

    //draw vertical grid lines
    for (int tilex = firstTilex; tilex <= lastTilex; tilex++)
    {
        tile.setX(LevelCoords::tileToPixel(tilex, TILE_W));

        int screenx = tile.toScreen(this).x();

        QPen pen = mEditor->config().getGridPen(tilex, pixelsPerTileX);
        if (pen.color().alpha() > 0)
        {
            painter.setPen(pen);
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
        QString::number(mCenter.x()),
        QString::number(mCenter.y()),
        QString::number(mZoomFactor, 'f', 6),
        QString::number(mTargetZoomFactor, 'f', 6),
        QString::number(mFrameCounter->lastFPS(), 'f', 1));

    painter.drawText(0, 10, str);

    str = QString("(%1,%2) - (%3,%4)").arg(
        QString::number(mCursor.x(), 'f', 1),
        QString::number(mCursor.y(), 'f', 1),
        QString::number(mCursor.tileX()),
        QString::number(mCursor.tileY()));

    painter.drawText(0, 30, str);


    painter.drawText(0, 50, QString("%1; %2/%3; %4")
        .arg(QString::number(mSmoothView->state()))
        .arg(mSmoothView->currentTime())
        .arg(mSmoothView->duration())
        .arg(mSmoothViewStopPan)
    );
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::zoomAt(const LevelCoords& center, float newZoomFactor)
{
    //make sure the level is still in the view
    LevelCoords targetCenter = mEditor->boundPixelToLevel(center);

    //upper zoom limit
    if (newZoomFactor > mEditor->config().maxZoomFactor())
        newZoomFactor = mEditor->config().maxZoomFactor();

    //lower limit of zoom
    if (newZoomFactor < mEditor->config().minZoomFactor())
        newZoomFactor = mEditor->config().minZoomFactor();

    mTargetZoomFactor = newZoomFactor;

    setViewTargetAndZoomSmooth(targetCenter, mEditor->boundScreenPixelToLevel(screenCenter()), newZoomFactor);
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::zoomTowards(const LevelCoords& target, const ScreenCoords& screenTarget, float newZoomFactor)
{
    //make sure the level is still in the view
    LevelCoords targetCenter = mEditor->boundPixelToLevel(target);

    //upper zoom limit
    if (newZoomFactor > mEditor->config().maxZoomFactor())
        newZoomFactor = mEditor->config().maxZoomFactor();

    //lower limit of zoom
    if (newZoomFactor < mEditor->config().minZoomFactor())
        newZoomFactor = mEditor->config().minZoomFactor();

    mTargetZoomFactor = newZoomFactor;

    setViewTargetAndZoomSmooth(targetCenter, screenTarget, newZoomFactor);
}

//////////////////////////////////////////////////////////////////////////

LevelBounds EditorWidget::viewBounds() const
{
    return LevelBounds::fromCenterAndZoom(mCenter, QWidget::size(), mZoomFactor);
}

//////////////////////////////////////////////////////////////////////////

SmoothViewBounds EditorWidget::viewBoundsAndZoom() const
{
    SmoothViewBounds svb(screenCenter());
    svb.mLevelTarget = viewCenter();
    svb.mViewBounds = viewBounds();
    svb.mZoomFactor = zoomFactor();

    return svb;
}

//////////////////////////////////////////////////////////////////////////

LevelCoords EditorWidget::viewCenter() const
{
    return viewBounds().center();
}

//////////////////////////////////////////////////////////////////////////

LevelCoords EditorWidget::viewTopLeft() const
{
    return LevelCoords(mCenter - QPointF(QWidget::width(), QWidget::height()) / (mZoomFactor * 2));
}

///////////////////////////////////////////////////////////////////////////

float EditorWidget::zoomFactor() const
{
    return mZoomFactor;
}

///////////////////////////////////////////////////////////////////////////

ScreenCoords EditorWidget::screenCenter() const
{
    return ScreenCoords(this, QPointF(QWidget::width() / 2.0f, QWidget::height() / 2.0f));
}


//////////////////////////////////////////////////////////////////////////

void EditorWidget::setViewCenter(const LevelCoords& centerPixel)
{
    mCenter = centerPixel;

    if (mSmoothView->state() == QAbstractAnimation::State::Running)
    {
        mSmoothViewStopPan = true;
    }

    if (updatesEnabled())
    {
        update();
        emit viewMoved(viewBounds());
    }
}

///////////////////////////////////////////////////////////////////////////

void EditorWidget::setViewTargetSmooth(const LevelCoords& targetLevel, const ScreenCoords& targetScreen)
{
    Q_ASSERT(mSmoothView);

    float targetZoom = zoomFactor();

    //If animation is still running, keep the previous zoom target
    if (mSmoothView->state() == QAbstractAnimation::State::Running)
    {
        targetZoom = mSmoothView->endValue().value<SmoothViewBounds>().mZoomFactor;
    }

    setViewTargetAndZoomSmooth(targetLevel, mEditor->boundScreenPixelToLevel(targetScreen), targetZoom);
}

///////////////////////////////////////////////////////////////////////////

void EditorWidget::setViewCenterSmooth(const LevelCoords& targetCenter)
{
    Q_ASSERT(mSmoothView);

    float targetZoom = zoomFactor();

    //If animation is still running, keep the previous zoom target
    if (mSmoothView->state() == QAbstractAnimation::State::Running)
    {
        //Jump to the end of the animation (except zoom)
        SmoothViewBounds svb = mSmoothView->endValue().value<SmoothViewBounds>();

        float currentZoom = targetZoom;

        //Continue new animation with the same zoom target
        targetZoom = svb.mZoomFactor;

        //Skip to the final position of the current animation
        //svb.mZoomFactor = currentZoom;
        //setViewBoundsAndZoom(svb);
    }

    setViewTargetAndZoomSmooth(targetCenter, mEditor->boundScreenPixelToLevel(screenCenter()), targetZoom);
}

///////////////////////////////////////////////////////////////////////////

void EditorWidget::setViewTargetAndZoomSmooth(const LevelCoords& targetLevel, const ScreenCoords& targetScreen, float targetZoom)
{
    Q_ASSERT(mSmoothView);

    if (mSmoothView->state() == QAbstractAnimation::State::Running)
    {
        //Restart the ongoing animation
        mSmoothView->stop();
    }

    //Start a new animation
    SmoothViewBounds svbInitial(targetScreen);
    svbInitial.mLevelTarget = LevelCoords(targetScreen);
    svbInitial.mViewBounds = viewBounds();
    svbInitial.mZoomFactor = zoomFactor();

    SmoothViewBounds svb(targetScreen);
    svb.mLevelTarget = targetLevel;
    svb.mViewBounds = LevelBounds::fromTargetAndZoom(targetLevel, targetScreen.uv(), QWidget::size(), targetZoom);
    svb.mZoomFactor = targetZoom;

    mSmoothView->setEasingCurve(QEasingCurve(QEasingCurve::OutCubic));
    mSmoothView->setDuration(mEditor->config().smoothCameraTime());
    mSmoothView->setStartValue(QVariant::fromValue(svbInitial));
    mSmoothView->setEndValue(QVariant::fromValue(svb));

    mSmoothViewStopPan = false;

    mSmoothView->start();
}

///////////////////////////////////////////////////////////////////////////

void EditorWidget::setViewBoundsAndZoom(const SmoothViewBounds& boundsAndZoom)
{
    if (!mSmoothViewStopPan)
        mCenter = mEditor->boundPixelToLevel(boundsAndZoom.mViewBounds.center());

    mZoomFactor = boundsAndZoom.mZoomFactor;

    if (updatesEnabled())
    {
        update();
        emit viewMoved(viewBounds());
    }
}

///////////////////////////////////////////////////////////////////////////

void EditorWidget::setZoomFactor(float zoomFactor)
{
    mZoomFactor = zoomFactor;

    if (updatesEnabled())
    {
        update();
        emit viewMoved(viewBounds());
    }
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

