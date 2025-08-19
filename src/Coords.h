#ifndef Coords_H
#define Coords_H

#include "Global.h"

#include <QtCore/QPointF>
#include <QtCore/QRectF>
#include <QtCore/QSizeF>
#include <QMetaType>

///////////////////////////////////////////////////////////////////////////

class QSinglePointEvent;

///////////////////////////////////////////////////////////////////////////

namespace SCME {

constexpr int TILE_W = 16;
constexpr int TILE_H = 16;
constexpr QSize TILE_SIZE(16, 16);


///////////////////////////////////////////////////////////////////////////

class LevelCoords;
class LevelTileCoords;
class ScreenCoords;
class EditorWidget;

class ScreenCoords : public QPointF
{
public:

    static constexpr inline float levelToScreen(float levelCoord, float screenFirstLevelCoord, float zoomFactor) { return (levelCoord - screenFirstLevelCoord) * zoomFactor; }

    static constexpr inline float screenToLevel(float screenPixel, float screenFirstLevelCoord, float zoomFactor) { return screenPixel / zoomFactor + screenFirstLevelCoord; }

    ScreenCoords(const EditorWidget* w, const QPointF& xy) :
        QPointF(xy),
        mWidget(w)
    {
    }

    ScreenCoords(const EditorWidget* w, int x, int y) : ScreenCoords(w, QPointF(x, y)) {}

    ScreenCoords(const EditorWidget* w, const QPoint& xy) : ScreenCoords(w, xy.toPointF()) {}

    /// Screen coordinates from mouse event
    ScreenCoords(const EditorWidget* w, const QSinglePointEvent* pointEvent);

    /// Level coordinates to screen coordinates
    ScreenCoords(const EditorWidget* w, const LevelCoords& levelCoord);

    /// Copy constructor
    ScreenCoords(const ScreenCoords& orig) : ScreenCoords(orig.mWidget, orig) {}

    /// Screen coordinates to level coordinates
    LevelCoords toLevel() const;

private:

    const EditorWidget* mWidget = nullptr;
};


///////////////////////////////////////////////////////////////////////////

class LevelCoords : public QPointF
{
public:

    static constexpr inline int tileToPixel(int tile, int tilesize) { return tile * tilesize; }

    static constexpr inline float tileToPixel(float tile, int tilesize) { return tile * tilesize; }

    explicit LevelCoords() : QPointF() {}

    explicit LevelCoords(const QPointF& pixelxy) : QPointF(pixelxy) {}

    explicit LevelCoords(const QPoint& pixelxy) : QPointF(pixelxy) {}

    LevelCoords(int pixelx, int pixely) : QPointF(pixelx, pixely) {}

    /// Screen coordinates to level coordinates
    LevelCoords(const ScreenCoords& screenPixel);

    inline int pixelX() const { return static_cast<int>(QPointF::x()); }

    inline int pixelY() const { return static_cast<int>(QPointF::y()); }

    inline int tileX() const { return pixelX() / TILE_W; }

    inline int tileY() const { return pixelY() / TILE_H; }

    inline float pixelXf() const { return QPointF::x(); }

    inline float pixelYf() const { return QPointF::y(); }

    inline float tileXf() const { return QPointF::x() / TILE_W; }

    inline float tileYf() const { return QPointF::y() / TILE_H; }

    /// Level coordinates to screen coordinates
    ScreenCoords toScreen(const EditorWidget* w);

    inline static LevelCoords fromTile(int x, int y) { return LevelCoords(tileToPixel(x, TILE_W), tileToPixel(y, TILE_H)); }

    inline static LevelCoords fromTile(float x, float y) { return LevelCoords(tileToPixel(x, TILE_W), tileToPixel(y, TILE_H)); }
};


///////////////////////////////////////////////////////////////////////////

class LevelBounds : public QRectF
{
public:

    /// Default constructor (null bounds)
    LevelBounds() : QRectF() {}

    explicit LevelBounds(const QRectF& bounds) : QRectF(bounds) {}

    LevelBounds(const LevelCoords& topLeft, const LevelCoords& bottomRight) : QRectF(topLeft, bottomRight) {}

    LevelBounds(const LevelCoords& topLeft, const QSizeF& size) : QRectF(topLeft, size) {}

    /// Bound \a coord within these bounds
    LevelCoords bounded(const LevelCoords& coord) const;

    inline LevelCoords topLeft() const { return LevelCoords(QRectF::topLeft()); }

    inline LevelCoords topRight() const { return LevelCoords(QRectF::topRight()); }

    inline LevelCoords bottomLeft() const { return LevelCoords(QRectF::bottomLeft()); }

    inline LevelCoords bottomRight() const { return LevelCoords(QRectF::bottomRight()); }

    inline LevelCoords center() const { return LevelCoords(QRectF::center()); }

    inline LevelBounds intersected(const LevelBounds& other) const { return LevelBounds(QRectF::intersected(other)); }

    static inline LevelBounds fromTopLeftAndZoom(const LevelCoords& topLeft, const QSize& size, float zoomFactor)
    {
        return LevelBounds(topLeft, size.toSizeF() / zoomFactor);
    }

    static LevelBounds fromCenterAndZoom(const LevelCoords& center, const QSize& size, float zoomFactor)
    {
        QPointF halfSize = QPointF(size.width(), size.height()) / (zoomFactor * 2);

        return LevelBounds(LevelCoords(center - halfSize), LevelCoords(center + halfSize));
    }
};


///////////////////////////////////////////////////////////////////////////

} // End namespace SCME

Q_DECLARE_METATYPE(SCME::ScreenCoords);
Q_DECLARE_METATYPE(SCME::LevelCoords);
Q_DECLARE_METATYPE(SCME::LevelBounds);

//////////////////////////////////////////////////////////////////////////

#endif // Coords_H
