#ifndef INC_Tile_H
#define INC_Tile_H

#include "Global.h"

#include <QtCore/QMetaType>



//////////////////////////////////////////////////////////////////////////

namespace SCME {

class LevelObject;
class TileInfo;

///////////////////////////////////////////////////////////////////////////

class Tile
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

    inline bool isTileset() const { return mId > 0 && mId <= 190; }

    inline bool isVisible() const;

    inline bool isVisibleOnRadar() const;

    inline bool isSolid() const {
        return mId > 0 && mId <= 161
            || mId >= 192 && mId <= 215
            || mId >= 221 && mId <= 240
            || mId >= 243 && mId <= 251;
    }

    inline bool isObject() const;

    inline bool isFlyOver() const { return mId >= SpecialTileFlyOver_0 && mId <= SpecialTileFlyOver_n; }

    inline bool isFlyUnder() const { return mId >= SpecialTileFlyUnder_0 && mId <= SpecialTileFlyUnder_n; }

    const TileInfo& getInfo() const;

    static const TileInfo& getInfo(TileId id);

    std::shared_ptr<LevelObject> getObject() const;
};



//////////////////////////////////////////////////////////////////////////

inline bool Tile::isObject() const
{
    return
        mId >= SpecialTileDoorA1 && mId <= SpecialTileDoorB4 ///< Animated doors
        || mId == SpecialTileFlag ///< Animated flag
        || mId == SpecialTileGoal ///< Animated goal
        || mId >= SpecialTileSmallAsteroid1 && mId <= SpecialTileSmallAsteroid2 ///< Asteroids
        || mId == SpecialTileStation
        || mId == SpecialTileWormhole;
}



///////////////////////////////////////////////////////////////////////////

} // End namespace SCME

//Let QList/QVector operate as optimally as possible on a list of tiles
Q_DECLARE_TYPEINFO(SCME::Tile, Q_PRIMITIVE_TYPE);


///////////////////////////////////////////////////////////////////////////

#endif // INC_Tile_H
