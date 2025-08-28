#ifndef INC_Renderer_H
#define INC_Renderer_H

//////////////////////////////////////////////////////////////////////////

#include "Global.h"

#include "Coords.h"


#include <QtOpenGL/QOpenGLFunctions_3_3_Core>


//////////////////////////////////////////////////////////////////////////

namespace SCME {

//////////////////////////////////////////////////////////////////////////

class LevelBounds;
class LevelData;


//////////////////////////////////////////////////////////////////////////

class SCME_LIB_DLL Renderer : protected QOpenGLFunctions_3_3_Core
{
public:
    // Instance payload sent per visible tile
#pragma pack(push, 1)
    struct TileInstanceData
    {
        uint16_t x;         // tile grid x (0..1023)
        uint16_t y;         // tile grid y (0..1023)
        uint16_t tileIndex; // 0..190
        uint16_t pad;       // pad so stride is 8 bytes (nice and tight)
    };
#pragma pack(pop)

    Renderer() = default;
    virtual ~Renderer() = default;

    virtual void init();

    virtual void render(const LevelData* level, const LevelBounds& visibleArea, float zoomFactor) = 0;
};


//////////////////////////////////////////////////////////////////////////

} // End namespace SCME

///////////////////////////////////////////////////////////////////////////

#endif // INC_Renderer_H
