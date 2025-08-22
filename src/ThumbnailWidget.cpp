#include "ThumbnailWidget.h"

#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>

#include "Editor.h"


///////////////////////////////////////////////////////////////////////////

using namespace ::SCME;

//////////////////////////////////////////////////////////////////////////

ThumbnailWidget::ThumbnailWidget(Editor* editor, QWidget *parent) :
    QOpenGLWidget(parent),
    mEditor(editor),
    mThumbnailArea(0, 0, 100, 100),
    mLevelBoundsPen(QColor(Qt::white)),
    mViewBoundsPen (QColor(255, 0, 0, 128))
{
    setMouseTracking(true);
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
    QOpenGLWidget::initializeGL();
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

    Q_ASSERT(mEditor);
    QSizeF  levelPixelSize = mEditor->levelBounds().size();

    mThumbnailScale = QPointF((qreal)mThumbnailArea.width () / levelPixelSize.width(),
                              (qreal)mThumbnailArea.height() / levelPixelSize.height());

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
    if (event->button() == Qt::LeftButton ||
        event->button() == Qt::MiddleButton)
    {
        emit doCenterView(screenToLevelPixel(event->position()));
    }
}

//////////////////////////////////////////////////////////////////////////

void ThumbnailWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & (Qt::LeftButton | Qt::MiddleButton))
    {
        emit doCenterView(screenToLevelPixel(event->position()));
    }
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

    viewBoundsScreen.setLeft((qreal)mThumbnailArea.left() + (qreal)(mViewBounds.left() * mThumbnailScale.x()));
    viewBoundsScreen.setWidth((qreal)(mViewBounds.width()*mThumbnailScale.x()));

    viewBoundsScreen.setTop((qreal)mThumbnailArea.top() + (qreal)(mViewBounds.top() * mThumbnailScale.y()));
    viewBoundsScreen.setHeight((qreal)(mViewBounds.height()*mThumbnailScale.y()));

    painter.drawRect(viewBoundsScreen);

    /// Draw a crosshair in the center of the view area if the edges are not all visible
    if (viewBoundsScreen.left() < 0 || viewBoundsScreen.right() >= width() ||
        viewBoundsScreen.top() < 0 || viewBoundsScreen.bottom() >= height())
    {
        constexpr qreal crosshairSize = 8;
        QPointF c = viewBoundsScreen.center();
        painter.drawLine(c - QPointF(crosshairSize, 0), c + QPointF(crosshairSize, 0));
        painter.drawLine(c - QPointF(0, crosshairSize), c + QPointF(0, crosshairSize));
    }
}

//////////////////////////////////////////////////////////////////////////

void ThumbnailWidget::redrawThumbnail()
{
    update();
}

//////////////////////////////////////////////////////////////////////////

void ThumbnailWidget::redrawView(const LevelBounds& viewBounds)
{
    mViewBounds = viewBounds;

    update();
}

//////////////////////////////////////////////////////////////////////////

LevelCoords ThumbnailWidget::screenToLevelPixel(const ThumbnailScreenCoords& screenxy)
{
    LevelCoords px(
        (screenxy.x() - mThumbnailArea.left()) / mThumbnailScale.x(),
        (screenxy.y() - mThumbnailArea.top ()) / mThumbnailScale.y());

    return (mEditor->boundPixelToLevel(px));
}
