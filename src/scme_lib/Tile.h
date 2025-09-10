#ifndef INC_Tile_H
#define INC_Tile_H

#include "Global.h"

#include <QtCore/QMetaType>
#include <QtCore/QTypeInfo>



//////////////////////////////////////////////////////////////////////////

namespace SCME {

class LevelObject;
class TileInfo;

///////////////////////////////////////////////////////////////////////////

class SCME_LIB_DLL Tile
{
public:

    enum SpecialTile : TileId
    {
        Void = 0,

        SpecialTileMapBorder = 20,

        SpecialTileDoorA1 = 162,
        SpecialTileDoorA2 = 163,
        SpecialTileDoorA3 = 164,
        SpecialTileDoorA4 = 165,

        SpecialTileDoorB1 = 166,
        SpecialTileDoorB2 = 167,
        SpecialTileDoorB3 = 168,
        SpecialTileDoorB4 = 169,

        SpecialTileFlag = 170,
        SpecialTileSafety = 171,
        SpecialTileGoal = 172,

        SpecialTileFlyOver_0 = 173,
        SpecialTileFlyOver_n = 175,

        SpecialTileFlyUnder_0 = 176,
        SpecialTileFlyUnder_n = 190, // Tileset sprites end here

        SpecialTileInvisibleItemWall = 191, ///< Ships go through, blocks items

        // 192 -> 215: Invisible solid tiles, visible on radar

        SpecialTileSmallAsteroid1 = 216,
        SpecialTileLargeAsteroid = 217,
        SpecialTileSmallAsteroid2 = 218,

        SpecialTileStation = 219,
        SpecialTileWormhole = 220,

        // 221 -> 240: Invisible solid tiles, visible on radar

        SpecialTileInvisibleItemVoid = 241, ///< Ships go through, makes items disappear

        SpecialTileAntiThor = 242, ///< Ships are warped on contact, Thors are destroyed on contact

        // 243 -> 251: Invisible solid tiles, invisible on radar

        SpecialTileBrickSpriteEnemy = 252,
        SpecialTileBrickSpriteTeam = 253,

        SpecialTileAntiBrick = 254,

        SpecialTilePrizeSprite = 255
    };

    TileId mId = Void;

    operator TileId& () { return mId; }
    operator const TileId& () const { return mId; }
    inline Tile& operator=(const Tile& copy)
    {
        mId = copy.mId;
        return *this;
    };

    inline Tile& operator=(TileId id)
    {
        mId = id;
        return *this;
    };

    const TileInfo& getInfo() const;

    static const TileInfo& getInfo(TileId id);

    std::shared_ptr<LevelObject> getObject() const;
};

/// We must be able to send a 1024x1024 array of Tile to the GPU as a 1024x1024 array of uint8_t
static_assert(sizeof(Tile) == sizeof(TileId));


///////////////////////////////////////////////////////////////////////////

} // End namespace SCME

//Let QList/QVector operate as optimally as possible on a list of tiles
Q_DECLARE_TYPEINFO(SCME::Tile, Q_PRIMITIVE_TYPE);


///////////////////////////////////////////////////////////////////////////

#endif // INC_Tile_H
