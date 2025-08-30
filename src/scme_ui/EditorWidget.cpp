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
#include "TileRenderer.h"
#include "MinimapRenderer.h"

#include "LevelData.h"

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
    //cleanup GL stuff
    makeCurrent();
    mTileRenderer = nullptr;
    mMinimapRenderer = nullptr;
}

//////////////////////////////////////////////////////////////////////////

std::shared_ptr<LevelData> EditorWidget::level() const
{
    return mEditor ? mEditor->level() : nullptr;
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

ScreenCoords EditorWidget::boundScreenPixelToLevel(const LevelData* pLevel, const ScreenCoords& screenPixel)
{
    if (!pLevel)
        return screenPixel;

    LevelCoords level = screenPixel.toLevel();

    LevelCoords levelBounded =  pLevel->boundPixelToLevel(level);

    if (level != levelBounded)
        return ScreenCoords(screenPixel.widget(), levelBounded);

    return screenPixel;
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::initializeGL()
{
    QOpenGLWidget::initializeGL();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //Background color
    glClearColor(0.01f, 0.01f, 0.01f, 1.0f);

    auto pLevel = level();

    //Ensure tile shader is initialized
    mTileRenderer = std::make_unique<TileRenderer>();
    mTileRenderer->init(pLevel ? pLevel->tileset() : Tileset());

    mMinimapRenderer = std::make_unique<MinimapRenderer>();
    mMinimapRenderer->init();

    mFrameCounter->start();
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::paintGL()
{
    QOpenGLWidget::paintGL();

    auto pLevel = level();
    if (!pLevel)
        return;

    mMinimapRenderer->refreshMinimap(pLevel);

    {
        QPainter painter(this);
        painter.beginNativePainting();

        //Draw grid under rendered tiles
        drawGrid(painter, pLevel.get());

        painter.endNativePainting();
    }

    LevelBounds renderBounds = viewBounds();

    constexpr float pixelRenderFadeInZoomFactor = 1.f / 12.f;
    constexpr float pixelRenderFadeOutZoomFactor = 1.f / 16.f;

    float minimapRenderOpacity = 1.f - std::clamp(
        (zoomFactor() - pixelRenderFadeOutZoomFactor) / (pixelRenderFadeInZoomFactor - pixelRenderFadeOutZoomFactor),
        0.f,
        1.f);

    if (minimapRenderOpacity < 1.f)
    {
        mTileRenderer->render(pLevel.get(), renderBounds, zoomFactor());
    }


    if (minimapRenderOpacity > 0.f)
    {
        std::shared_ptr<QImage> img = mMinimapRenderer->image();

        if (img)
        {
            QPainter painter(this);
            painter.beginNativePainting();

            //Use exact pixel scale for 1:16 zoom (or more zoomed in), and use smooth interpolation when zoomed out more
            painter.setRenderHint(QPainter::SmoothPixmapTransform, zoomFactor() < 1.f / 17.f);

            painter.setOpacity(minimapRenderOpacity);

            ScreenCoords screenTopLeft = boundScreenPixelToLevel(pLevel.get(), ScreenCoords(this, QPointF(0, 0)));
            ScreenCoords screenBottomRight = boundScreenPixelToLevel(pLevel.get(), ScreenCoords(this, QPointF(width(), height())));

            painter.drawImage(
                QRectF(screenTopLeft, screenBottomRight),
                *img,
                QRectF(screenTopLeft.toLevel().tilef(), screenBottomRight.toLevel().tilef())
            );

            painter.endNativePainting();
        }
    }

    {
        QPainter painter(this);
        painter.beginNativePainting();

        //Draw on top of render
        drawDebug(painter, pLevel.get());

        painter.endNativePainting();
    }

    mFrameCounter->onFrameRendered();
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::resizeGL( int width, int height )
{
    QOpenGLWidget::resizeGL(width, height);

    glViewport(0, 0, width, height);

    emit viewMoved(viewBounds());

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

static void setLevelTile(EditorWidget* w, const std::shared_ptr<LevelData>& level, const QPoint& tile, TileId id)
{
    auto& r = level->tiles()(tile);
    if (r.mId != id)
    {
        r.mId = id;
        w->onTilesChanged();
    }
}


//////////////////////////////////////////////////////////////////////////

void EditorWidget::mousePressEvent(QMouseEvent *event)
{
    auto pLevel = level();
    if (!pLevel)
        return;

    ScreenCoords clickScreenCoords(this, event);

    /// @todo Start drag with specific mouse button(s)

    if (event->buttons().testFlag(Qt::MouseButton::LeftButton))
    {
        //Draw
        QPoint tile = pLevel->boundPixelToLevel(clickScreenCoords.toLevel()).tile();
        setLevelTile(this, pLevel, tile, mCurrentTileId);
    }

    if (event->buttons().testFlag(Qt::MouseButton::RightButton))
    {
        //Erase
        QPoint tile = pLevel->boundPixelToLevel(clickScreenCoords.toLevel()).tile();
        setLevelTile(this, pLevel, tile, Tile::Void);
    }

    if (event->buttons().testFlag(Qt::MouseButton::MiddleButton))
    {
        mDragStart = clickScreenCoords;
        mCenterOrig = viewCenter();
        mDragging = true;
    }
}

/// Move this!
template<typename F>
static void doOnLine(int x0, int y0, int x1, int y1, F func)
{
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy; // Initial error term

    while (true)
    {
        func(x0, y0);

        if (x0 == x1 && y0 == y1) {
            break; // Reached endpoint
        }

        int e2 = 2 * err; // Double the error for comparison

        if (e2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::mouseMoveEvent(QMouseEvent *event)
{
    auto pLevel = level();
    if (!pLevel)
        return;

    auto previousCursor = mCursor;
    mCursor = pLevel->boundPixelToLevel(ScreenCoords(this, event).toLevel());

    if (event->buttons().testFlag(Qt::MouseButton::LeftButton) ||
        event->buttons().testFlag(Qt::MouseButton::RightButton))
    {
        TileId id = event->buttons().testFlag(Qt::MouseButton::LeftButton) ? mCurrentTileId : Tile::Void;

        //Draw or erase
        QPoint tileFrom = pLevel->boundPixelToLevel(previousCursor).tile();
        QPoint tileTo = pLevel->boundPixelToLevel(mCursor).tile();

        doOnLine(tileFrom.x(), tileFrom.y(), tileTo.x(), tileTo.y(), [this, pLevel, id](int x, int y)
            {
                setLevelTile(this, pLevel, QPoint(x, y), id);
            }
        );
    }




    if (mDragging)
    {
        //qDebug() << "Drag@ " << event->position();
        setViewCenter(pLevel->boundPixelToLevel(LevelCoords(mCenterOrig - ((event->position() - mDragStart) / mZoomFactor))));
    }
    else
    {
        update();
    }
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::wheelEvent(QWheelEvent *event)
{
    auto pLevel = level();
    if (!pLevel)
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
    qDebug() << "zoomIndex=" << mZoomIndex << "; zoom=" << zoomMultiplier << zoomFactorAsString(zoomMultiplier);

    ScreenCoords screenTarget(this, event);

    //zoomAt(LevelCoords(ScreenCoords(this, event)), zoomMultiplier);
    zoomTowards(screenTarget.toLevel(), boundScreenPixelToLevel(pLevel.get(), screenTarget), zoomMultiplier);
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!level())
        return;

    if (mDragging && !event->buttons().testFlag(Qt::MouseButton::MiddleButton))
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

void EditorWidget::drawGrid(QPainter& painter, const LevelData* pLevel)
{
    Q_ASSERT(pLevel);

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, false);

    QSize levelSize = pLevel->size();

    //visible level bounds
    LevelBounds visibleLevelBounds = viewBounds();

    LevelBounds entireLevelBounds = pLevel->bounds();
    ScreenCoords levelTopLeftInScreen(this, entireLevelBounds.topLeft());
    ScreenCoords levelBottomRightInScreen(this, entireLevelBounds.bottomRight());

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
    visibleLevelBounds = visibleLevelBounds.intersected(entireLevelBounds);

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

        int screeny = ScreenCoords(this, tile).y();

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

        int screenx = ScreenCoords(this, tile).x();

        QPen pen = mEditor->config().getGridPen(tilex, pixelsPerTileX);
        if (pen.color().alpha() > 0)
        {
            painter.setPen(pen);
            painter.drawLine(screenx, screenBounds.top(), screenx, screenBounds.bottom());
        }
    }

    painter.restore();
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::drawDebug(QPainter& painter, const LevelData* pLevel)
{
    painter.save();

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

    painter.restore();
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::onLevelChanged()
{
    onTilesetChanged();
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::onTilesetChanged()
{
    auto pLevel = level();

    if (mTileRenderer && pLevel)
    {
        mTileRenderer->clearCache();
        mTileRenderer->updateTileset(pLevel->tileset());
    }

    //Force refresh
    update();
}

///////////////////////////////////////////////////////////////////////////

void EditorWidget::onTilesChanged()
{
    auto pLevel = level();

    if (mTileRenderer && pLevel)
    {
        mTileRenderer->clearCache();
    }

    if (mMinimapRenderer)
    {
        auto pImage = mMinimapRenderer->image();
        if (pImage)
            pImage->bits(); //does something
    }

    update();
}

///////////////////////////////////////////////////////////////////////////

void EditorWidget::onTilesChanged(const LevelBounds& bounds)
{
    onTilesChanged();
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::zoomAt(const LevelCoords& center, float newZoomFactor)
{
    auto pLevel = level();
    if (!pLevel)
        return;

    //make sure the level is still in the view
    LevelCoords targetCenter = pLevel->boundPixelToLevel(center);

    //upper zoom limit
    if (newZoomFactor > mEditor->config().maxZoomFactor())
        newZoomFactor = mEditor->config().maxZoomFactor();

    //lower limit of zoom
    if (newZoomFactor < mEditor->config().minZoomFactor())
        newZoomFactor = mEditor->config().minZoomFactor();

    mTargetZoomFactor = newZoomFactor;

    setViewTargetAndZoomSmooth(targetCenter, boundScreenPixelToLevel(pLevel.get(), screenCenter()), newZoomFactor);
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::zoomTowards(const LevelCoords& target, const ScreenCoords& screenTarget, float newZoomFactor)
{
    auto pLevel = level();
    if (!pLevel)
        return;

    //make sure the level is still in the view
    LevelCoords targetCenter = pLevel->boundPixelToLevel(target);

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
    auto pLevel = level();
    if (!pLevel)
        return;

    Q_ASSERT(mSmoothView);

    float targetZoom = zoomFactor();

    //If animation is still running, keep the previous zoom target
    if (mSmoothView->state() == QAbstractAnimation::State::Running)
    {
        targetZoom = mSmoothView->endValue().value<SmoothViewBounds>().mZoomFactor;
    }

    setViewTargetAndZoomSmooth(targetLevel, boundScreenPixelToLevel(pLevel.get(), targetScreen), targetZoom);
}

///////////////////////////////////////////////////////////////////////////

void EditorWidget::setViewCenterSmooth(const LevelCoords& targetCenter)
{
    auto pLevel = level();
    if (!pLevel)
        return;

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

    setViewTargetAndZoomSmooth(targetCenter, boundScreenPixelToLevel(pLevel.get(), screenCenter()), targetZoom);
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
    svbInitial.mLevelTarget = targetScreen.toLevel();
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
    auto pLevel = level();
    if (!pLevel)
        return;

    if (!mSmoothViewStopPan)
        mCenter = pLevel->boundPixelToLevel(boundsAndZoom.mViewBounds.center());

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

