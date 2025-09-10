#ifndef INC_TileInfo_H
#define INC_TileInfo_H

#include "Global.h"


#include "Tile.h"

#include <QtCore/QSize>
#include <QtGui/QColor>

#include <array>


//////////////////////////////////////////////////////////////////////////

namespace SCME {

class LevelObject;

///////////////////////////////////////////////////////////////////////////

/// Properties of a tile, given its id
class SCME_LIB_DLL TileInfo
{
public:

    /// Color category that can be assigned to each tile number
    enum PixelColor
    {
        PixelColorVoid,
        PixelColorNormal,

        PixelColorDoor,
        PixelColorFlag,
        PixelColorSafe,
        PixelColorGoal,

        PixelColorFlyOver,
        PixelColorFlyUnder,

        PixelColorSpecial,

        PixelColor_Count
    };

    bool mIsSolid = false;
    bool mIsTileset = false;
    bool mIsAnimated = false;
    bool mIsFlyOver = false;
    bool mIsFlyUnder = false;
    bool mIsVisible = false;
    bool mIsVisibleOnRadar = false;

    constexpr bool isLarge() { return mSize > 1; }

    int mSize = 1;

    PixelColor mPixelColor = PixelColorVoid;

    QColor pixelColor() const;

    static QColor pixelColor(PixelColor paletteColor);
};


/// Defines a color palette to use for each TileInfo::PixelColor role
class SCME_LIB_DLL TilePixelColorPalette
{
public:
    std::array<QColor, TileInfo::PixelColor_Count> mPixelColorPalette;

    static TilePixelColorPalette sCurrentPalette;

    static TilePixelColorPalette getDefaultPalette();
};

///////////////////////////////////////////////////////////////////////////


// Properties per tile id are fixed at compile-time
typedef std::array<TileInfo, 256> TileInfoArray;

constexpr TileInfoArray initTileInfo()
{
    TileInfoArray tileInfoArray;

    for (size_t i = 1; i < tileInfoArray.size(); i++)
    {
        TileInfo* ti = tileInfoArray.data() + i;

        ti->mIsSolid = (
            i > 0 && i <= 161
            || i >= 192 && i <= 215
            || i >= 221 && i <= 240
            || i >= 243 && i <= 251
        );

        ti->mIsTileset = i > 0 && i <= 190;

        ti->mIsAnimated = (
            i >= Tile::SpecialTileDoorA1 && i <= Tile::SpecialTileDoorB4 ///< Animated doors
            || i == Tile::SpecialTileFlag ///< Animated flag
            || i == Tile::SpecialTileGoal ///< Animated goal
            || i >= Tile::SpecialTileSmallAsteroid1 && i <= Tile::SpecialTileSmallAsteroid2 ///< Asteroids
            || i == Tile::SpecialTileStation
            || i == Tile::SpecialTileWormhole
        );

        ti->mIsFlyOver = i >= Tile::SpecialTileFlyOver_0 && i <= Tile::SpecialTileFlyOver_n;
        ti->mIsFlyUnder = i >= Tile::SpecialTileFlyUnder_0 && i <= Tile::SpecialTileFlyUnder_n;
        ti->mIsVisible = true;
        ti->mIsVisibleOnRadar = true;


        if (i == Tile::SpecialTileLargeAsteroid)
            ti->mSize = 2;
        else if (i == Tile::SpecialTileStation)
            ti->mSize = 6;
        else if (i == Tile::SpecialTileWormhole)
            ti->mSize = 5;
        else
            ti->mSize = 1;


        TileInfo::PixelColor c = TileInfo::PixelColorVoid;

        if (i >= Tile::SpecialTileDoorA1 && i <= Tile::SpecialTileDoorB4)
            c = TileInfo::PixelColorDoor;
        else if (i == Tile::SpecialTileGoal)
            c = TileInfo::PixelColorGoal;
        else if (i == Tile::SpecialTileFlag)
            c = TileInfo::PixelColorFlag;
        else if (i == Tile::SpecialTileSafety)
            c = TileInfo::PixelColorSafe;
        else
            c = (i <= TILESET_COUNT) ? TileInfo::PixelColorNormal : TileInfo::PixelColorSpecial;

        ti->mPixelColor = c;
    }

    return tileInfoArray;
}

constexpr TileInfoArray TileInfoDict = initTileInfo();


///////////////////////////////////////////////////////////////////////////

} // End namespace SCME


///////////////////////////////////////////////////////////////////////////

#endif // INC_TileInfo_H
