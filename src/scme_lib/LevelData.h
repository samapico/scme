#ifndef INC_LevelData_H
#define INC_LevelData_H

//////////////////////////////////////////////////////////////////////////

#include "Global.h"

#include "Array2D.h"
#include "Tile.h"
#include "LevelTiles.h"
#include "Tileset.h"
#include "ExtraLevelData.h"
#include "Coords.h"


///////////////////////////////////////////////////////////////////////////

class QString;
class QDataStream;

//////////////////////////////////////////////////////////////////////////

namespace SCME {

//////////////////////////////////////////////////////////////////////////

class SCME_LIB_DLL LevelData
{
public:

    LevelData();
    virtual ~LevelData();

    bool loadFromFile(const QString& filepath);

    bool saveToFile(const QString& filepath) const;

    inline const LevelTiles& tiles() const;

    inline LevelTiles& tiles();

    const Tileset& tileset() const;

    inline QSize size() const;

    inline const LevelBounds& bounds() const;

    LevelCoords boundPixelToLevel(const LevelCoords& pixel) const;

    void boundTileToLevel(int& tileX, int& tileY) const;

    TileCoords boundTileToLevel(const TileCoords& tileXY) const;

protected:

    /// A tile, as it is represented in a .lvl file
    union SerializedTile
    {
        SerializedTile() = default;
        SerializedTile(int x, int y, TileId tileId) :
            mX(x),
            mY(y),
            mTileId(tileId)
        {
        }

        uint32_t mBytes = 0;
        struct
        {
            uint32_t mX         : 10; //0..1023
            uint32_t _unused1   :  2; //Should always be zero, otherwise it is an error
            uint32_t mY         : 10; //0..1023
            uint32_t _unused2   :  2; //Should always be zero, otherwise it is an error
            uint32_t mTileId    :  8; //0..255
        };
    };
    static_assert(sizeof(SerializedTile) == 4);

    bool load(QDataStream& in);

    LevelTiles mTiles;

    LevelBounds mBounds;

    ExtraLevelData mExtraLevelData;

    Tileset mTileset;
};


//////////////////////////////////////////////////////////////////////////

QSize LevelData::size() const
{
    return mTiles.size2d();
}


//////////////////////////////////////////////////////////////////////////

const LevelBounds& LevelData::bounds() const
{
    return mBounds;
}


//////////////////////////////////////////////////////////////////////////

LevelTiles& SCME::LevelData::tiles()
{
    return mTiles;
}

//////////////////////////////////////////////////////////////////////////

const LevelTiles& LevelData::tiles() const
{
    return mTiles;
}

//////////////////////////////////////////////////////////////////////////

} // End namespace SCME

///////////////////////////////////////////////////////////////////////////

#endif // INC_LevelData_H
