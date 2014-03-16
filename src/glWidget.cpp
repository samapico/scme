#include "glWidget.h"

#include <QtGui/QPaintEvent>

#include "editor.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

//////////////////////////////////////////////////////////////////////////

GLWidget::GLWidget(Editor* editor, QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent),
    mEditor(editor),
    mTopLeft(0, 0),
    mZoomFactor(1)
{
}

//////////////////////////////////////////////////////////////////////////

GLWidget::~GLWidget()
{

}

//////////////////////////////////////////////////////////////////////////

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

//////////////////////////////////////////////////////////////////////////

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}

//////////////////////////////////////////////////////////////////////////

void GLWidget::initializeGL()
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
void GLWidget::paintGL()
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

void GLWidget::resizeGL( int width, int height )
{
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

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    mDragStart = event->pos();
    mCenterOrig = getViewCenter();
    mDragging  = true;
}

//////////////////////////////////////////////////////////////////////////

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (mDragging)
    {
        setCenter(mEditor->boundPixelToLevel(mCenterOrig - ((event->pos() - mDragStart) / mZoomFactor)));
    }
}

//////////////////////////////////////////////////////////////////////////

void GLWidget::wheelEvent(QWheelEvent *event)
{
    if (event->delta() > 0)
        zoomInAt(screenToLevelPixel(event->pos()), 1 + (event->delta()*mEditor->config().wheelZoomSpeed()));
    else
        zoomOutAt(screenToLevelPixel(event->pos()), 1 - (event->delta()*mEditor->config().wheelZoomSpeed()));
}

//////////////////////////////////////////////////////////////////////////

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    mDragging = false;
}

//////////////////////////////////////////////////////////////////////////

void GLWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    
    painter.fillRect(event->rect(), QColor(Qt::black));

    drawGrid(painter);
    
#ifdef _DEBUG
    drawDebug(painter);
#endif

    painter.end();
}

//////////////////////////////////////////////////////////////////////////

void GLWidget::drawGrid(QPainter& painter)
{
    //painter.setRenderHints(QPainter::Antialiasing);

    QSize levelSize = mEditor->levelSize();
    QSize levelPxSize = mEditor->levelPixelSize();

    //visible level bounds
    QRect visibleLevelBounds = getViewBounds();

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

void GLWidget::setTopLeft(const QPoint& topLeft, bool redraw /*= true*/)
{
    mTopLeft = topLeft;

    if (redraw)
        update();
}

//////////////////////////////////////////////////////////////////////////

void GLWidget::drawDebug(QPainter& painter)
{
    QString str;

    painter.setPen(QColor(Qt::green));
    str = QString("(%1,%2)x%3").arg(
        QString::number(mTopLeft.x()),
        QString::number(mTopLeft.y()),
        QString::number(mZoomFactor, 'f', 3));
    
    painter.drawText(0, 10, str);
}

//////////////////////////////////////////////////////////////////////////

void GLWidget::zoomInAt(const QPoint& center, float zoomMultiplier, bool redraw /*= true*/)
{
    QPoint targetCenter = mEditor->boundPixelToLevel(center);
    QPoint screenTarget = levelPixelToScreen(targetCenter);

    mZoomFactor *= zoomMultiplier;

    /// @todo Proper upper zoom limit
    if (mZoomFactor > mEditor->config().maxZoom())
        mZoomFactor = mEditor->config().maxZoom();

    //move center towards new center
    alignView(screenTarget, targetCenter, false);

    if (redraw)
        update();
}

//////////////////////////////////////////////////////////////////////////

void GLWidget::zoomOutAt(const QPoint& center, float zoomMultiplier, bool redraw /*= true*/)
{
    QPoint targetCenter = mEditor->boundPixelToLevel(center);
    QPoint screenTarget = levelPixelToScreen(targetCenter);
    
    mZoomFactor /= zoomMultiplier;

    //lower limit of zoom
    if (mZoomFactor < mEditor->config().minZoom())
        mZoomFactor = mEditor->config().minZoom();

    //move center towards new center
    alignView(screenTarget, targetCenter, false);

    if (redraw)
        update();
}

//////////////////////////////////////////////////////////////////////////

QPoint GLWidget::getViewCenter() const
{
    return getViewBounds().center();
}

//////////////////////////////////////////////////////////////////////////

void GLWidget::setCenter(const QPoint& centerPixel, bool redraw /*= true*/)
{
    mTopLeft.setX(centerPixel.x() - QWidget::width() /(mZoomFactor*2));
    mTopLeft.setY(centerPixel.y() - QWidget::height()/(mZoomFactor*2));

    if (redraw)
        update();
}

//////////////////////////////////////////////////////////////////////////

QRect GLWidget::getViewBounds() const
{
    return QRect(mTopLeft, QWidget::size()/mZoomFactor);
}

//////////////////////////////////////////////////////////////////////////

void GLWidget::alignView(const QPoint& screenPixel, const QPoint& levelPixel, bool redraw /*= true*/)
{
     mTopLeft = levelPixel - (screenPixel / mZoomFactor);

     //make sure the level is still in the view
     QPoint center = getViewCenter();
     QPoint boundedCenter = mEditor->boundPixelToLevel(center);

     if (center != boundedCenter)
         setCenter(boundedCenter, false);

     if (redraw)
         update();
}
