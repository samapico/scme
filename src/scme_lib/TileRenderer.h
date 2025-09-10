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

class SCME_LIB_DLL TileRenderer : public Renderer
{
public:
    TileRenderer() = default;
    virtual ~TileRenderer();

    void init(const Tileset& tileset);

    void updateTileset(const Tileset& tileset);

    void render(const LevelData* level, const LevelBounds& visibleArea, float zoomFactor);

    void clearCache();

protected:

    // Instance payload sent per visible tile
#pragma pack(push, 1)
    struct TileInstanceData
    {
        int16_t x;          // tile grid x (0..1023)
        int16_t y;          // tile grid y (0..1023)
        uint16_t tileIndex; // 1..255
        uint16_t sizeMultiply; // size multiplier, for large special tiles
    };

    struct TileQuad
    {
        float x, y;
        float u, v;
    };

#pragma pack(pop)



    void setTileQuadVertexAttributes(GLuint& layoutIndex);

    void setTileInstanceVertexAttributes(GLuint& layoutIndex);

    std::unique_ptr<QOpenGLTexture> mTexTileset;

    std::unique_ptr<QOpenGLTexture> mTexAnimatedSprites;

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

    GLuint borderVbo_ = 0;
    GLuint borderVao_ = 0;

    static constexpr int BORDER_TILE_COUNT = (2 * MAP_W + 2 * MAP_H + 4);

    // Avoid rebuilding the list of tile instances in two consecutive frames with the same bounds
    std::vector<TileInstanceData> mCachedInstances;
    TileCoords mCachedBoundsTopLeft;
    TileCoords mCachedBoundsBottomRight;
};


//////////////////////////////////////////////////////////////////////////

} // End namespace SCME

///////////////////////////////////////////////////////////////////////////

#endif // INC_TileRenderer_H
