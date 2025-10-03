#ifndef INC_Global_H
#define INC_Global_H

///////////////////////////////////////////////////////////////////////////

#ifdef SCME_LIB_EXPORT_SYMBOLS //defined in scme.vcxproj
#define SCME_LIB_DLL __declspec(dllexport)
#else
#define SCME_LIB_DLL __declspec(dllimport)
#endif

#define GL_MULTISAMPLE  0x809D

#include <memory>
#include <QtGui/QMatrix4x4>
#include <QtGui/QColor>

#include "../appver.h"


///////////////////////////////////////////////////////////////////////////

namespace SCME {

    typedef unsigned char TileId;

    constexpr int TILE_W = 16;
    constexpr int TILE_H = 16;
    constexpr QSize TILE_SIZE(TILE_W, TILE_H);


    constexpr int TILESET_COUNT_W = 19; ///< Number of tiles in a tileset, horizontally
    constexpr int TILESET_COUNT_H = 10; ///< Number of tiles in a tileset, vertically

    constexpr int TILESET_W = TILESET_COUNT_W * TILE_W; ///< Width of a tileset, in pixels
    constexpr int TILESET_H = TILESET_COUNT_H * TILE_H; ///< Height of a tileset, in pixels
    constexpr QSize TILESET_SIZE(TILESET_W, TILESET_H);

    constexpr int TILESET_COUNT = TILESET_COUNT_W * TILESET_COUNT_H; ///< Total number of tiles in a tileset

    constexpr int TILESET_EXTRA_COUNT_H = 4;
    constexpr int TILESET_EXTRA_H = TILESET_EXTRA_COUNT_H * TILE_H;

    constexpr int TILESET_WITH_EXTRA_H = TILESET_H + TILESET_EXTRA_H;

    constexpr QSize TILESET_SIZE_WITH_EXTRA = TILESET_SIZE + QSize(0, TILESET_EXTRA_H);

    constexpr int MAP_W = 1024;
    constexpr int MAP_H = 1024;
    constexpr QSize MAP_SIZE(MAP_W, MAP_H);

    /// In level world coordinates: Z = -1.0: bottom,  Z = +1.0: top
    /// Screen coordinates are inverted;
    /// All these constants are in world coordinates, the rendering has to convert them if needed
    constexpr float ZCOORD_GRID_OVERTILES = 0.1f;
    constexpr float ZCOORD_MAPTILES = 0.0f;
    constexpr float ZCOORD_GRID_UNDERTILES = -0.1f;
    constexpr float ZCOORD_MAPBORDER = -0.5f;

    enum Layer
    {
        LayerBelowAll,
        LayerAfterBackground,
        LayerAfterTiles, ///< Split for flyover, solid, flyunder tiles?
        LayerAfterWeapons,
        LayerAfterShips,
        LayerAfterGauges,
        LayerAfterChat,
        LayerTopMost,
    };


    union RGBA_t
    {
        RGBA_t() :
            RGBA(0)
        {
        }

        RGBA_t(const QColor& c) :
            R(c.red()),
            G(c.green()),
            B(c.blue()),
            A(c.alpha())
        {
        }

        RGBA_t(uint8_t r, uint8_t g, uint8_t b, uint8_t a) :
            R(r),
            G(g),
            B(b),
            A(a)
        {
        }

        RGBA_t(const RGBA_t& copy) :
            RGBA(copy.RGBA)
        {
        }

        uint32_t RGBA;

        struct
        {
            uint8_t R;
            uint8_t G;
            uint8_t B;
            uint8_t A;
        };
    };

    static_assert(sizeof(RGBA_t) == sizeof(uint32_t));

    static constexpr QVector4D colorToVec4(const QColor& c)
    {
        return QVector4D(c.redF(), c.greenF(), c.blueF(), c.alphaF());
    }


    static QString zoomFactorAsString(float zoom, int decimals = 3)
    {
        if (zoom >= 1.f)
            return QString("%1:1").arg(QString::number(zoom, 'f', decimals));
        return QString("1:%1").arg(QString::number(1.f / zoom, 'f', decimals));
    }

    /// Converts an integer to an hexadecimal string, padded with 0's to correspond to the size of the datatype
    template<typename INT_TYPE>
    static QString intAsHexString(INT_TYPE n)
    {
        return QString::number(n, 16).rightJustified(sizeof(INT_TYPE) * 2, '0');
    }

    template<typename INT_TYPE>
    static QByteArray intAsBytes(INT_TYPE n)
    {
        return QByteArray((const char*)(&n), sizeof(INT_TYPE));
    }


    static QString applicationVersionString()
    {
        return QString("%1.%2.%3%4").arg(
            QString::number(APP_VERSION_MAJOR),
            QString::number(APP_VERSION_MINOR),
            QString::number(APP_VERSION_REV),
#ifdef _DEBUG
            QString(" (DEBUG)")
#else
            QString()
#endif
        );
    }


} // End namespace SCME

#endif // INC_Global_H
