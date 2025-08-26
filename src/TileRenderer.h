#ifndef INC_TileRenderer_H
#define INC_TileRenderer_H

//////////////////////////////////////////////////////////////////////////

#include "Global.h"
#include "Tile.h"
#include "Coords.h"


#include "Renderer.h"
#include <QtOpenGL/QOpenGLShaderProgram>
#include <QtOpenGL/QOpenGLTexture>


//////////////////////////////////////////////////////////////////////////

class QOpenGLTexture;


//////////////////////////////////////////////////////////////////////////

namespace SCME {

//////////////////////////////////////////////////////////////////////////

class Tileset;
class LevelBounds;
class LevelData;


//////////////////////////////////////////////////////////////////////////

class TileRenderer : public Renderer
{
public:
    TileRenderer() = default;
    virtual ~TileRenderer();

    void init(const Tileset& tileset);

    void updateTileset(const Tileset& tileset);

    void render(const LevelData* level, const LevelBounds& visibleArea, float zoomFactor);

protected:

    std::unique_ptr<QOpenGLTexture> mTexTileset;
    std::unique_ptr<QOpenGLTexture> mTexAnimFlag;
    std::unique_ptr<QOpenGLTexture> mTexAnimGoal;
    std::unique_ptr<QOpenGLTexture> mTexAnimAsteroid1;
    std::unique_ptr<QOpenGLTexture> mTexAnimAsteroid2;
    std::unique_ptr<QOpenGLTexture> mTexAnimAsteroid3;
    std::unique_ptr<QOpenGLTexture> mTexAnimStation;
    std::unique_ptr<QOpenGLTexture> mTexAnimWormhole;

    QOpenGLShaderProgram mProgram;

    GLuint quadVbo_ = 0;      // 4-vertex quad
    GLuint instanceVbo_ = 0;  // per-visible-tile data
    GLuint vao_ = 0;

    // Avoid rebuilding the list of tile instances in two consecutive frames with the same bounds
    std::vector<TileInstanceData> cachedInstances;
    TileCoords cachedBoundsTopLeft;
    TileCoords cachedBoundsBottomRight;
};


//////////////////////////////////////////////////////////////////////////

} // End namespace SCME

///////////////////////////////////////////////////////////////////////////

#endif // INC_TileRenderer_H
