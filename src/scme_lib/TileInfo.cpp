#include "TileInfo.h"

#include "Tile.h"

///////////////////////////////////////////////////////////////////////////

using namespace ::SCME;


///////////////////////////////////////////////////////////////////////////

TilePixelColorPalette TilePixelColorPalette::sCurrentPalette = getDefaultPalette();


///////////////////////////////////////////////////////////////////////////

QColor TileInfo::pixelColor() const
{
    return pixelColor(mPixelColor);
}


///////////////////////////////////////////////////////////////////////////

QColor TileInfo::pixelColor(PixelColor paletteColor)
{
    return TilePixelColorPalette::sCurrentPalette.mPixelColorPalette[paletteColor];
}

///////////////////////////////////////////////////////////////////////////

TilePixelColorPalette TilePixelColorPalette::getDefaultPalette()
{
    LogDebug() << "@getDefaultPalette()";

    return { {
        QColor(Qt::transparent), // PixelColorVoid
        QColor(192, 192, 192), // PixelColorNormal   (gray)
        QColor(  0,   0, 255), // PixelColorDoor     (blue)
        QColor(255, 255,   0), // PixelColorFlag     (yellow)
        QColor(  0, 255,   0), // PixelColorSafe     (green)
        QColor(255,   0,   0), // PixelColorGoal     (red)
        QColor(128, 128,   0), // PixelColorFlyOver  (dark yellow)
        QColor(160,   0,   0), // PixelColorFlyUnder (dark red)
        QColor(255, 255,   0), // PixelColorSpecial  (magenta)
    } };
};

//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
