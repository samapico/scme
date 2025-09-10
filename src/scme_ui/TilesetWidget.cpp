#include "TilesetWidget.h"

#include "Tile.h"
#include "Tileset.h"
#include <QtGui/QMouseEvent>
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>


///////////////////////////////////////////////////////////////////////////

using namespace ::SCME;


//////////////////////////////////////////////////////////////////////////

TilesetWidget::TilesetWidget(QWidget *parent) :
    QWidget(parent),
    mSelection(
        {
            { LMB, 1 },
            { RMB, Tile::Void }
        }
    )
{
    setMouseTracking(true);
}

//////////////////////////////////////////////////////////////////////////

TilesetWidget::~TilesetWidget()
{
}

//////////////////////////////////////////////////////////////////////////

QSize TilesetWidget::minimumSizeHint() const
{
    return sizeHint();
}

//////////////////////////////////////////////////////////////////////////

QSize TilesetWidget::sizeHint() const
{
    return QSize(TILESET_W, TILESET_H + TILESET_EXTRA_H);
}

//////////////////////////////////////////////////////////////////////////

void TilesetWidget::mousePressEvent(QMouseEvent *event)
{
    if (isSelectionButton(event->button()))
    {
        if (inTilesetBounds(event->position()))
        {
            setSelection(event->button(), widgetCoordToTileId(event->position()));

            mDraggingButtons.setFlag(event->button());
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void TilesetWidget::mouseMoveEvent(QMouseEvent *event)
{
    //Test each mouse button
    if (inTilesetBounds(event->position()))
    {
        if (mDraggingButtons.testFlag(LMB))
            setSelection(LMB, widgetCoordToTileId(event->position()));

        if (mDraggingButtons.testFlag(RMB))
            setSelection(RMB, widgetCoordToTileId(event->position()));
    }
}

//////////////////////////////////////////////////////////////////////////

void TilesetWidget::mouseReleaseEvent(QMouseEvent *event)
{
    mDraggingButtons.setFlag(event->button(), false);
}

//////////////////////////////////////////////////////////////////////////

void TilesetWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    drawTilesetImage(painter);

    drawSelection(painter);
}

//////////////////////////////////////////////////////////////////////////

void TilesetWidget::drawTilesetImage(QPainter& painter)
{
    painter.save();

    Q_ASSERT(displayScale() == 1.0f); //< If we ever want to support scaled tileset, we need to keep a separate scaled pixmap
    painter.drawPixmap(0, 0, mCurrentTileset);

    painter.restore();
}

//////////////////////////////////////////////////////////////////////////

void TilesetWidget::drawSelection(QPainter& painter)
{
    painter.save();

    TileId idLeft = mSelection.value(LMB);
    TileId idRight = mSelection.value(RMB);

    if (idLeft == idRight)
    {
        painter.setPen(selectionColor(LMB | RMB));
        painter.drawRect(QRectF(tileIdToWidgetCoord(idLeft), TILE_SIZE.toSizeF() * displayScale()));
    }
    else
    {
        painter.setPen(selectionColor(LMB));
        painter.drawRect(QRectF(tileIdToWidgetCoord(idLeft), TILE_SIZE.toSizeF() * displayScale()));

        painter.setPen(selectionColor(RMB));
        painter.drawRect(QRectF(tileIdToWidgetCoord(idRight), TILE_SIZE.toSizeF() * displayScale()));
    }

    painter.restore();
}

//////////////////////////////////////////////////////////////////////////

bool TilesetWidget::inTilesetBounds(const QPointF& pixel) const
{
    return pixel.x() >= 0
        && pixel.y() >= 0
        && pixel.x() < TILESET_W
        && pixel.y() < TILESET_H + TILESET_EXTRA_H;
}

//////////////////////////////////////////////////////////////////////////

TileId TilesetWidget::widgetCoordToTileId(const QPointF& pixel) const
{
    int c = static_cast<int>(pixel.x() / TILE_W); //column index
    int r = static_cast<int>(pixel.y() / TILE_H); //row index
    Q_ASSERT(c >= 0);
    Q_ASSERT(c < TILESET_COUNT_W);
    Q_ASSERT(r >= 0);
    Q_ASSERT(r < TILESET_COUNT_H + TILESET_EXTRA_COUNT_H);

    int tileId = 1 + c + r * TILESET_COUNT_W;

    if (tileId > 255)
        return Tile::Void;
    return static_cast<TileId>(tileId);
}

//////////////////////////////////////////////////////////////////////////

QPointF TilesetWidget::tileIdToWidgetCoord(TileId id) const
{
    uint8_t asInt = (id - 1); //< TileId 0 is index 255 visually; TileId 1 is index 0 visually

    return QPointF(
        (asInt % TILESET_COUNT_W) * TILE_W,
        (asInt / TILESET_COUNT_W) * TILE_H
    );
}

//////////////////////////////////////////////////////////////////////////

void TilesetWidget::onTilesetChanged(const Tileset& tileset)
{
    mCurrentTileset = tileset.pixmapWithExtraTiles();

    // Force widget redraw
    update();
}

//////////////////////////////////////////////////////////////////////////

TileId TilesetWidget::selection(Qt::MouseButton button) const
{
    return mSelection.value(button, Tile::Void);
}

//////////////////////////////////////////////////////////////////////////

void TilesetWidget::setSelection(Qt::MouseButton button, TileId id)
{
    mSelection.insert(button, id);

    update();
}

//////////////////////////////////////////////////////////////////////////

QColor TilesetWidget::selectionColor(Qt::MouseButtons buttons) const
{
    switch (buttons)
    {
    case LMB:
        return Qt::red;

    case RMB:
        return Qt::yellow;

    case LMB|RMB:
        return Qt::white;
    }

    Q_ASSERT(0);
    return Qt::cyan;
}
