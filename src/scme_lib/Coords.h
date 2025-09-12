#ifndef INC_Coords_H
#define INC_Coords_H

#include "Global.h"

#include <QtCore/QPointF>
#include <QtCore/QRectF>
#include <QtCore/QSizeF>
#include <QtGui/QMatrix4x4>


///////////////////////////////////////////////////////////////////////////

namespace SCME {


///////////////////////////////////////////////////////////////////////////

class LevelCoords;
class LevelTileCoords;
class ScreenCoords;

class SCME_LIB_DLL TileCoords : public QPoint
{
public:
    using QPoint::QPoint;
};

///////////////////////////////////////////////////////////////////////////

class SCME_LIB_DLL LevelCoords : public QPointF
{
public:

    static constexpr inline int tileToPixel(int tile, int tilesize) { return tile * tilesize; }

    static constexpr inline float tileToPixel(float tile, int tilesize) { return tile * tilesize; }

    explicit LevelCoords() : QPointF() {}

    explicit LevelCoords(const QPointF& pixelxy) : QPointF(pixelxy) {}

    explicit LevelCoords(const QPoint& pixelxy) : QPointF(pixelxy) {}

    LevelCoords(int pixelx, int pixely) : QPointF(pixelx, pixely) {}

    inline int pixelX() const { return static_cast<int>(QPointF::x()); }

    inline int pixelY() const { return static_cast<int>(QPointF::y()); }

    inline int tileX() const { return pixelX() / TILE_W; }

    inline int tileY() const { return pixelY() / TILE_H; }

    inline float pixelXf() const { return QPointF::x(); }

    inline float pixelYf() const { return QPointF::y(); }

    inline float tileXf() const { return QPointF::x() / TILE_W; }

    inline float tileYf() const { return QPointF::y() / TILE_H; }

    inline TileCoords tile() const { return TileCoords(tileX(), tileY()); }

    inline QPointF tilef() const { return QPointF(tileXf(), tileYf()); }

    inline static LevelCoords fromTile(int x, int y) { return LevelCoords(tileToPixel(x, TILE_W), tileToPixel(y, TILE_H)); }

    inline static LevelCoords fromTile(float x, float y) { return LevelCoords(tileToPixel(x, TILE_W), tileToPixel(y, TILE_H)); }

private:
    /// Catch invalid casts
    /// Use ScreenCoords::toLevel instead
    /// Without this, LevelCoords(screenCoords) would end up in LevelCoords::LevelCoords(const QPointF&), which is incorrect
    explicit LevelCoords(const ScreenCoords& screen) = delete;
};


///////////////////////////////////////////////////////////////////////////

class SCME_LIB_DLL LevelBounds : public QRectF
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

    inline int tileLeft() const { return QRectF::left() / TILE_W; }

    inline int tileRight() const { return QRectF::right() / TILE_W; }

    inline int tileTop() const { return QRectF::top() / TILE_H; }

    inline int tileBottom() const { return QRectF::bottom() / TILE_H; }

    static inline QMatrix4x4 orthoPixels(float l, float r, float b, float t)
    {
        QMatrix4x4 m;
        m.ortho(l, r, b, t, -1.0f, 1.0f);
        return m;
    }

    inline QMatrix4x4 orthoPixels() const
    {
        return orthoPixels(left(), right(), bottom(), top());
    }

    static inline LevelBounds fromTopLeftAndZoom(const LevelCoords& topLeft, const QSize& size, float zoomFactor)
    {
        return LevelBounds(topLeft, size.toSizeF() / zoomFactor);
    }

    static LevelBounds fromCenterAndZoom(const LevelCoords& center, const QSize& size, float zoomFactor)
    {
        QPointF halfSize = QPointF(size.width(), size.height()) / (zoomFactor * 2);

        return LevelBounds(LevelCoords(center - halfSize), LevelCoords(center + halfSize));
    }

    static LevelBounds fromTargetAndZoom(const LevelCoords& target, const QPointF& targetPosUV, const QSize& size, float zoomFactor)
    {
        qreal w = size.width() / zoomFactor;
        qreal h = size.height() / zoomFactor;

        qreal left = target.pixelXf() - (targetPosUV.x() * w);
        qreal right = target.pixelXf() + ((1 - targetPosUV.x()) * w);

        qreal top = target.pixelYf() - (targetPosUV.y() * h);
        qreal bottom = target.pixelYf() + ((1 - targetPosUV.y()) * h);

        QPointF halfSize = QPointF(size.width(), size.height()) * 0.5 / zoomFactor;

        return LevelBounds(QRectF(QPointF(left, top), QPointF(right, bottom)));
    }
};


///////////////////////////////////////////////////////////////////////////

} // End namespace SCME

//////////////////////////////////////////////////////////////////////////

#endif // INC_Coords_H
