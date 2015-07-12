#include "EditorWidget.h"

#include <QtGui/QPaintEvent>
#include <QtCore/QPropertyAnimation>
#include <QtCore/QElapsedTimer>

#include <QtCore/QDebug>

#include "Editor.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

//////////////////////////////////////////////////////////////////////////

EditorWidget::EditorWidget(Editor* editor, QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent),
    mEditor(editor),
    mTopLeft(0, 0),
    mZoomFactor(1),
    mTargetZoomFactor(1),
    mSmoothView(0),
    mLastSmoothViewStart(0),
    mDragging(false)
{
    this->setMouseTracking(true);
}

//////////////////////////////////////////////////////////////////////////

EditorWidget::~EditorWidget()
{
    delete mLastSmoothViewStart;
    delete mSmoothView;
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
    QGLWidget::initializeGL();
    /*
    qglClearColor(QColor::fromCmykF(0.39, 0.39, 0.0, 0.0));

    //logo = new QtLogo(this, 64);
    //logo->setColor(qtGreen.dark());

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_FLAT);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_MULTISAMPLE);
    static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    */
}

//////////////////////////////////////////////////////////////////////////
/*
void EditorWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -10.0);
    //glRotatef(xRot / 16.0, 1.0, 0.0, 0.0);
    //glRotatef(yRot / 16.0, 0.0, 1.0, 0.0);
    //glRotatef(zRot / 16.0, 0.0, 0.0, 1.0);
    //logo->draw();
}
*/
//////////////////////////////////////////////////////////////////////////

void EditorWidget::resizeGL( int width, int height )
{
    emit viewMoved(viewBounds());

    //QGLWidget::resizeGL(width, height);
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
        qDebug() << "Drag@ " << event->posF();
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

    float zoomMultiplier = 1 + (qAbs(event->delta())*mEditor->config().wheelZoomSpeed());

    if (event->delta() > 0) //zoom in
        zoomAt(screenToLevelPixel(event->pos()), zoomMultiplier);
    else //zoom out
        zoomAt(screenToLevelPixel(event->pos()), 1/zoomMultiplier);
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
            qDebug() << "Release@ " << event->posF();
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
    QPainter painter;
    painter.begin(this);
    
    painter.fillRect(event->rect(), QColor(Qt::black));

    if (mEditor->level())
    {
        drawGrid(painter);
    
#ifdef _DEBUG
        drawDebug(painter);
#endif
    }

    painter.end();
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

    for (int tiley = firstTiley; tiley <= lastTiley; tiley++)
    {
        int screeny = (tiley*Editor::TILE_HEIGHT - mTopLeft.y())*mZoomFactor;

        painter.setPen(mEditor->config().getGridPen(tiley));

        painter.drawLine(screenBounds.left(), screeny, screenBounds.right(), screeny);
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

        painter.setPen(mEditor->config().getGridPen(tilex));

        painter.drawLine(screenx, screenBounds.top(), screenx, screenBounds.bottom());
    }
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::drawDebug(QPainter& painter)
{
    QString str;

    painter.setPen(QColor(Qt::green));
    str = QString("(%1,%2)x%3").arg(
        QString::number(mTopLeft.x()),
        QString::number(mTopLeft.y()),
        QString::number(mZoomFactor, 'f', 3));
    
    painter.drawText(0, 10, str);
    
    str = QString("(%1,%2)").arg(
        QString::number(mCursor.x()),
        QString::number(mCursor.y()));

    painter.drawText(0, 30, str);
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::zoomAt(const QPoint& center, float zoomMultiplier)
{
    //make sure the level is still in the view
    QPoint targetCenter = mEditor->boundPixelToLevel(center);
    QPoint screenTarget = levelPixelToScreen(targetCenter);

    float newZoomFactor = mTargetZoomFactor * zoomMultiplier;

    //upper zoom limit
    if (newZoomFactor > mEditor->config().maxZoom())
        newZoomFactor = mEditor->config().maxZoom();

    //lower limit of zoom
    if (newZoomFactor < mEditor->config().minZoom())
        newZoomFactor = mEditor->config().minZoom();

    mTargetZoomFactor = newZoomFactor;

    QRect newView = QRect(targetCenter - (screenTarget / newZoomFactor), QWidget::size()/newZoomFactor);

    //Make sure the view's center is in the level
    QPoint boundedViewCenter = mEditor->boundPixelToLevel(newView.center());

    newView.translate(boundedViewCenter - newView.center());

    setViewBoundsSmooth(newView, false);
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
    mZoomFactor = (float)QWidget::height()/(float)bounds.height();

    if (updatesEnabled())
    {
        update();
        emit viewMoved(viewBounds());
    }
}

///////////////////////////////////////////////////////////////////////////

void EditorWidget::setViewBoundsSmooth(const QRect& bounds, bool forceFinishPreviousAnimation /*= true*/)
{
    if (mSmoothView && mSmoothView->state() == QAbstractAnimation::Running)
    {
        if (forceFinishPreviousAnimation)
            setViewBounds(mSmoothView->endValue().toRect()); //Jump to the end of the previous transition

        mSmoothView->stop();
    }

    int duration = mEditor->config().smoothCameraTime();

    if (!duration)
    {
        // 0 ms duration; do not bother using an animation
        setViewBounds(bounds);
        return;
    }

    if (!mSmoothView)
    {
        mSmoothView = new QPropertyAnimation(this, "viewBounds");
    }

    mSmoothView->setEasingCurve(QEasingCurve(QEasingCurve::InOutCubic));
    mSmoothView->setDuration(mEditor->config().smoothCameraTime());
    mSmoothView->setStartValue(viewBounds());
    mSmoothView->setEndValue(bounds);

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
    setViewBoundsSmooth(QRect(topLeft, QWidget::size()/mZoomFactor));
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
        mLastSmoothViewStart = new QElapsedTimer;
        mLastSmoothViewStart->start();
    }
    else
    {
        qint64 dt = mLastSmoothViewStart->restart();

        bFinishPreviousAnimation = (dt < 100); ///< 10ms minimum animation time?
    }

    setViewBoundsSmooth(calcBoundsFromCenterAndZoom(centerPixel, mZoomFactor), bFinishPreviousAnimation);
}

///////////////////////////////////////////////////////////////////////////

void EditorWidget::setDefaultZoom()
{
    setZoomFactor(1.0);
    mTargetZoomFactor = zoomFactor();
}

//////////////////////////////////////////////////////////////////////////

void EditorWidget::setZoomFactor(float factor)
{
    mZoomFactor = factor;
    if (updatesEnabled())
    {
        update();
        emit viewMoved(viewBounds());
    }
}

///////////////////////////////////////////////////////////////////////////

void EditorWidget::setZoomFactorSmooth(float factor)
{
    setViewBoundsSmooth(calcBoundsFromCenterAndZoom(viewCenter(), factor));
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