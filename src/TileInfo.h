#ifndef INC_TileInfo_H
#define INC_TileInfo_H

#include "Global.h"


#include <QtCore/QSize>
#include <QtGui/QColor>

#include <array>


//////////////////////////////////////////////////////////////////////////

namespace SCME {

class LevelObject;

///////////////////////////////////////////////////////////////////////////

/// Properties of a tile, given its id
class TileInfo
{
public:

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
    bool mIsObject = false;
    bool mIsLarge = false;

    int mSize = 1;

    PixelColor mPixelColor = PixelColorVoid;

    QColor pixelColor() const;

    static QColor pixelColor(PixelColor paletteColor);
};


/// Defines a color palette to use for each TileInfo::PixelColor role
class TilePixelColorPalette
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

    for (size_t i = 0; i < tileInfoArray.size(); i++)
    {
        tileInfoArray[i].mIsSolid = true;
    }

    return tileInfoArray;
}

constexpr TileInfoArray TileInfoDict = initTileInfo();


///////////////////////////////////////////////////////////////////////////

} // End namespace SCME


///////////////////////////////////////////////////////////////////////////

#endif // INC_TileInfo_H
