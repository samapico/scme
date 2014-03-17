#include "ThumbnailWidget.h"

#include <QtGui/QPaintEvent>

#include "Editor.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

//////////////////////////////////////////////////////////////////////////

ThumbnailWidget::ThumbnailWidget(Editor* editor, QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent),
    mEditor(editor),
    mThumbnailArea(0, 0, 100, 100),
    mLevelBoundsPen(QColor(Qt::white)),
    mViewBoundsPen (QColor(Qt::red  ))
{
}

//////////////////////////////////////////////////////////////////////////

ThumbnailWidget::~ThumbnailWidget()
{

}

//////////////////////////////////////////////////////////////////////////

QSize ThumbnailWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

//////////////////////////////////////////////////////////////////////////

QSize ThumbnailWidget::sizeHint() const
{
    return QSize(150, 150);
}

//////////////////////////////////////////////////////////////////////////

void ThumbnailWidget::initializeGL()
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
void ThumbnailWidget::paintGL()
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

void ThumbnailWidget::resizeGL( int width, int height )
{
    const int margin = 3;

    if (width < height)
    {
        mThumbnailArea.setLeft(margin);
        mThumbnailArea.setWidth(width - margin*2);

        mThumbnailArea.setTop((height - width) / 2 + margin);
        mThumbnailArea.setHeight(width - margin*2);
    }
    else
    {
        mThumbnailArea.setTop(margin);
        mThumbnailArea.setHeight(height - margin*2);

        mThumbnailArea.setLeft((width - height) / 2 + margin);
        mThumbnailArea.setWidth(height - margin*2);
    }


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

void ThumbnailWidget::mousePressEvent(QMouseEvent *event)
{
}

//////////////////////////////////////////////////////////////////////////

void ThumbnailWidget::mouseMoveEvent(QMouseEvent *event)
{
}

//////////////////////////////////////////////////////////////////////////

void ThumbnailWidget::wheelEvent(QWheelEvent *event)
{
}

//////////////////////////////////////////////////////////////////////////

void ThumbnailWidget::mouseReleaseEvent(QMouseEvent *event)
{
}

//////////////////////////////////////////////////////////////////////////

void ThumbnailWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    
    painter.fillRect(event->rect(), QColor(Qt::black));

    drawLevelBounds(painter);

    drawViewBounds(painter);
    
#ifdef _DEBUG
    drawDebug(painter);
#endif

    painter.end();
}

//////////////////////////////////////////////////////////////////////////

void ThumbnailWidget::drawDebug(QPainter& painter)
{
    QString str;

    //painter.setPen(QColor(Qt::green));
    //str = QString("(%1,%2)x%3").arg(
    //    QString::number(mTopLeft.x()),
    //    QString::number(mTopLeft.y()),
    //    QString::number(mZoomFactor, 'f', 3));
    //
    //painter.drawText(0, 10, str);
}

//////////////////////////////////////////////////////////////////////////

void ThumbnailWidget::drawLevelBounds(QPainter& painter)
{
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(mLevelBoundsPen);

    painter.drawRect(mThumbnailArea);
}

//////////////////////////////////////////////////////////////////////////

void ThumbnailWidget::drawViewBounds(QPainter& painter)
{
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(mViewBoundsPen);
    
    QRectF viewBoundsScreen;
    QSize  levelPixelSize = mEditor->levelPixelSize();
    
    qreal thumbnailScaleX = (qreal)mThumbnailArea.width () / (qreal)levelPixelSize.width ();
    qreal thumbnailScaleY = (qreal)mThumbnailArea.height() / (qreal)levelPixelSize.height();

    viewBoundsScreen.setLeft((qreal)mThumbnailArea.left() + (qreal)(mViewBounds.left() * thumbnailScaleX));
    viewBoundsScreen.setWidth((qreal)(mViewBounds.width()*thumbnailScaleX));
    
    viewBoundsScreen.setTop((qreal)mThumbnailArea.top() + (qreal)(mViewBounds.top() * thumbnailScaleY));
    viewBoundsScreen.setHeight((qreal)(mViewBounds.height()*thumbnailScaleY));

    painter.drawRect(viewBoundsScreen);
}

//////////////////////////////////////////////////////////////////////////

void ThumbnailWidget::redrawThumbnail()
{
    update();
}

//////////////////////////////////////////////////////////////////////////

void ThumbnailWidget::redrawView(const QRect& viewBounds)
{
    mViewBounds = viewBounds;

    update();
}
