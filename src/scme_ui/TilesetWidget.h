#ifndef INC_TilesetWidget_H
#define INC_TilesetWidget_H

#include "UiGlobal.h"

#include <QtCore/QMap>
#include <QtWidgets/QWidget>


//////////////////////////////////////////////////////////////////////////

namespace SCME {

class Tileset;

//////////////////////////////////////////////////////////////////////////

class TilesetWidget : public QWidget
{
    Q_OBJECT

public:

    TilesetWidget(QWidget *parent = nullptr);
    virtual ~TilesetWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    TileId selection(Qt::MouseButton button) const;

    void setSelection(Qt::MouseButton button, TileId id);

public slots:

    void onTilesetChanged(const Tileset& tileset);

protected:

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent *event);

    void drawTilesetImage(QPainter& painter);

    void drawSelection(QPainter& painter);

private:

    static constexpr Qt::MouseButton LMB = Qt::MouseButton::LeftButton;
    static constexpr Qt::MouseButton RMB = Qt::MouseButton::RightButton;

    constexpr float displayScale() { return 1.0f; }

    constexpr bool isSelectionButton(Qt::MouseButton mb)
    {
        return mb == Qt::MouseButton::LeftButton || mb == Qt::MouseButton::RightButton;
    }

    static QColor blendColors(const QColor& color1, const QColor& color2);

    TileId widgetCoordToTileId(const QPointF& pixel) const;

    QPointF tileIdToWidgetCoord(TileId id) const;

    bool inTilesetBounds(const QPointF& pixel) const;

    QColor selectionColor(Qt::MouseButtons button) const;

    QMap<Qt::MouseButton, TileId> mSelection;

    QPixmap mCurrentTileset;

    Qt::MouseButtons mDraggingButtons = Qt::MouseButton::NoButton;
};


///////////////////////////////////////////////////////////////////////////

} // End namespace SCME

///////////////////////////////////////////////////////////////////////////

#endif // INC_TilesetWidget_H
