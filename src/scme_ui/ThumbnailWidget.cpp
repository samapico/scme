#include "ThumbnailWidget.h"

#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>

#include "Editor.h"
#include "LevelData.h"


///////////////////////////////////////////////////////////////////////////

using namespace ::SCME;

//////////////////////////////////////////////////////////////////////////

ThumbnailWidget::ThumbnailWidget(Editor* editor, QWidget *parent) :
    QWidget(parent),
    mEditor(editor),
    mThumbnailArea(0, 0, 100, 100),
    mLevelBoundsPen(QColor(Qt::darkMagenta)),
    mViewBoundsPen (QColor(255, 0, 0, 128), 2.f)
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

void ThumbnailWidget::resizeEvent(QResizeEvent* event)
{
    int width = event->size().width();
    int height = event->size().height();

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
    auto pLevel = mEditor->level();

    if (!pLevel)
        return;

    QSizeF levelPixelSize = pLevel->bounds().size();

    mThumbnailScale = QPointF((qreal)mThumbnailArea.width () / levelPixelSize.width(),
                              (qreal)mThumbnailArea.height() / levelPixelSize.height());
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
    QPainter painter(this);

    drawLevelBounds(painter);

    drawLevel(painter);

    drawViewBounds(painter);
}

//////////////////////////////////////////////////////////////////////////

void ThumbnailWidget::drawLevelBounds(QPainter& painter)
{
    painter.save();

    painter.fillRect(mThumbnailArea, Qt::black);

    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(mLevelBoundsPen);

    painter.drawRect(mThumbnailArea);

    painter.restore();
}

//////////////////////////////////////////////////////////////////////////

void ThumbnailWidget::drawLevel(QPainter& painter)
{
    auto pLevel = mEditor ? mEditor->level() : nullptr;
    if (!pLevel)
        return;

    const QImage* img = pLevel->tiles().image();
    if (!img)
        return;

    painter.save();

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::LosslessImageRendering, true);

    painter.drawImage(mThumbnailArea, *img);

    painter.restore();
}

//////////////////////////////////////////////////////////////////////////

void ThumbnailWidget::drawViewBounds(QPainter& painter)
{
    painter.save();

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

    painter.restore();
}

//////////////////////////////////////////////////////////////////////////

void ThumbnailWidget::redrawThumbnail()
{
    update();
}

//////////////////////////////////////////////////////////////////////////

void ThumbnailWidget::redrawViewBounds(const LevelBounds& viewBounds)
{
    mViewBounds = viewBounds;

    update();
}

//////////////////////////////////////////////////////////////////////////

void ThumbnailWidget::redrawLevel(const LevelData* level)
{
    update();
}

//////////////////////////////////////////////////////////////////////////

LevelCoords ThumbnailWidget::screenToLevelPixel(const ThumbnailScreenCoords& screenxy)
{
    auto pLevel = mEditor->level();

    LevelCoords px(
        (screenxy.x() - mThumbnailArea.left()) / mThumbnailScale.x(),
        (screenxy.y() - mThumbnailArea.top ()) / mThumbnailScale.y());

    if (pLevel)
        return (pLevel->boundPixelToLevel(px));
    return px;
}
