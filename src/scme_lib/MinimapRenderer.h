#ifndef INC_MinimapRenderer_H
#define INC_MinimapRenderer_H

//////////////////////////////////////////////////////////////////////////

#include "Global.h"
#include "Tile.h"
#include "Coords.h"


#include "Renderer.h"

#include <QtGui/QImage>


//////////////////////////////////////////////////////////////////////////

class QOpenGLTexture;


//////////////////////////////////////////////////////////////////////////

namespace SCME {

//////////////////////////////////////////////////////////////////////////

class Tileset;
class LevelBounds;
class LevelData;


//////////////////////////////////////////////////////////////////////////

class SCME_LIB_DLL MinimapRenderer : public Renderer
{
public:


    MinimapRenderer() = default;

    virtual ~MinimapRenderer();

    void init();

    void refreshMinimap(std::shared_ptr<LevelData> level);
    void refreshMinimap(std::shared_ptr<LevelData> level, const LevelBounds& bounds);

    std::shared_ptr<QImage> image() const;

    void render(const LevelData* level, const LevelBounds& visibleArea, float zoomFactor);

protected:

    void allocateImage(std::shared_ptr<LevelData> level);

    std::shared_ptr<QImage> mRadarImage;

    std::shared_ptr<LevelData> mLevel; //Keep instance of level to ensure the bytes used in mRadarImage remain valid
};


//////////////////////////////////////////////////////////////////////////

} // End namespace SCME

///////////////////////////////////////////////////////////////////////////

#endif // INC_MinimapRenderer_H
