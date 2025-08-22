#ifndef INC_Global_H
#define INC_Global_H

///////////////////////////////////////////////////////////////////////////

#define GL_MULTISAMPLE  0x809D

#include <memory>

///////////////////////////////////////////////////////////////////////////

namespace SCME {

    typedef unsigned char TileId;

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


} // End namespace SCME

#endif // INC_Global_H
