#include "TileRenderer.h"

#include "Tileset.h"
#include "TileInfo.h"
#include "Coords.h"
#include "LevelData.h"
#include "EditorConfig.h"

#include <QtGui/QImage>
#include <QtCore/QElapsedTimer>



///////////////////////////////////////////////////////////////////////////

using namespace ::SCME;

//////////////////////////////////////////////////////////////////////////


TileRenderer::~TileRenderer()
{
    mProgram.removeAllShaders();
}

//////////////////////////////////////////////////////////////////////////

void TileRenderer::clearCache()
{
    mCachedInstances.clear();
    mCachedBoundsTopLeft = {};
    mCachedBoundsBottomRight = {};
}

//////////////////////////////////////////////////////////////////////////

void TileRenderer::setTileQuadVertexAttributes(GLuint& layoutIndex)
{
    // aPos
    glEnableVertexAttribArray(layoutIndex);
    glVertexAttribPointer(layoutIndex, 2, GL_FLOAT, GL_FALSE, sizeof(TileQuad), (void*)0);
    layoutIndex++;

    // aUVUnit
    glEnableVertexAttribArray(layoutIndex);
    glVertexAttribPointer(layoutIndex, 2, GL_FLOAT, GL_FALSE, sizeof(TileQuad), (void*)(2 * sizeof(float)));
    layoutIndex++;
}

//////////////////////////////////////////////////////////////////////////

void TileRenderer::setTileInstanceVertexAttributes(GLuint& layoutIndex)
{
    // iTileXY (2x short)
    glEnableVertexAttribArray(layoutIndex);
    glVertexAttribIPointer(layoutIndex, 2, GL_SHORT, sizeof(TileInstanceData), (void*)0);
    glVertexAttribDivisor(layoutIndex, 1);
    layoutIndex++;

    // iTileIndex (1x unsigned short)
    glEnableVertexAttribArray(layoutIndex);
    glVertexAttribIPointer(layoutIndex, 1, GL_UNSIGNED_SHORT, sizeof(TileInstanceData), (void*)(2 * sizeof(int16_t)));
    glVertexAttribDivisor(layoutIndex, 1);
    layoutIndex++;

    // iTileSizeMultiplier
    glEnableVertexAttribArray(layoutIndex);
    glVertexAttribIPointer(layoutIndex, 1, GL_UNSIGNED_SHORT, sizeof(TileInstanceData), (void*)(3 * sizeof(int16_t)));
    glVertexAttribDivisor(layoutIndex, 1);
    layoutIndex++;
}

//////////////////////////////////////////////////////////////////////////

void TileRenderer::init(const Tileset& tileset)
{
    Renderer::init();

    // --- Load tileset texture ---
    {
        QImage img = tileset.pixmapWithExtraTiles().toImage(); // 304x224 bitmap

        if (!img.isNull())
        {
            QImage rgba = img.convertToFormat(QImage::Format_RGBA8888);

            mTexTileset = std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D);
            mTexTileset->create();
            mTexTileset->bind();

            Q_ASSERT(rgba.width() == TILESET_W);
            Q_ASSERT(rgba.height() == TILESET_WITH_EXTRA_H);

            mTexTileset->setSize(rgba.width(), rgba.height());
            mTexTileset->setFormat(QOpenGLTexture::RGBA8_UNorm);
            mTexTileset->allocateStorage();
            mTexTileset->setMinificationFilter(QOpenGLTexture::Linear);
            mTexTileset->setMagnificationFilter(QOpenGLTexture::Nearest);
            mTexTileset->setWrapMode(QOpenGLTexture::ClampToEdge);
            mTexTileset->setData(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, rgba.constBits());
            mTexTileset->release();
        }

        LogDebug() << "@TileRenderer::init" << img.format();
    }

    // --- Load shaders ---

    mProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/tile_vs.glsl");
    mProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/tile_fs.glsl");
    mProgram.link();

    // Set shader uniforms that never change
    mProgram.bind();
    mProgram.setUniformValue("uAtlasSize", QVector2D(TILESET_W, TILESET_WITH_EXTRA_H));
    mProgram.setUniformValue("uTilePx", QVector2D(TILE_W, TILE_H));

    GLint loc = mProgram.uniformLocation("uGridDim");
    glUniform2ui(loc, TILESET_COUNT_W, TILESET_COUNT_H + TILESET_EXTRA_COUNT_H);

    mProgram.setUniformValue("uAtlas", 0);

    mProgram.release();

    // --- Geometry: a 16x16 quad, triangle strip ---
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    TileQuad quad[4] = {
        {     0,      0, 0, 0},
        {TILE_W,      0, 1, 0},
        {     0, TILE_H, 0, 1},
        {TILE_W, TILE_H, 1, 1},
    };

    glGenBuffers(1, &quadVbo_);
    glBindBuffer(GL_ARRAY_BUFFER, quadVbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    GLuint dynamicVbaLayout = 0;
    // aPos
    // aUVUnit
    setTileQuadVertexAttributes(dynamicVbaLayout);

    // --- Instance buffer ---
    glGenBuffers(1, &instanceVbo_);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVbo_);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW); // we’ll resize per frame

    // iTileXY (2x short)
    // iTileIndex (1x unsigned short)
    // iTileSizeMultiplier
    setTileInstanceVertexAttributes(dynamicVbaLayout);

    // --- Static map border instances ---
    std::vector<TileInstanceData> borderTiles;
    borderTiles.reserve(BORDER_TILE_COUNT);

    for (int16_t x = -1; x < MAP_W + 1; x++) //horizontal border, top and bottom (including corners)
    {
        borderTiles.push_back(TileInstanceData{ x, (int16_t)-1, (uint16_t)(Tile::SpecialTileMapBorder - 1), (uint16_t)1 });
        borderTiles.push_back(TileInstanceData{ x, (int16_t)MAP_H, (uint16_t)(Tile::SpecialTileMapBorder - 1), (uint16_t)1 });
    }

    for (int16_t y = 0; y < MAP_H; y++) //vertical border, left and right (excluding corners)
    {
        borderTiles.push_back(TileInstanceData{ (int16_t)-1,    y, (uint16_t)(Tile::SpecialTileMapBorder - 1), (uint16_t)1 });
        borderTiles.push_back(TileInstanceData{ (int16_t)MAP_W, y, (uint16_t)(Tile::SpecialTileMapBorder - 1), (uint16_t)1 });
    }
    Q_ASSERT(borderTiles.size() == BORDER_TILE_COUNT);

    glGenVertexArrays(1, &borderVao_);
    glBindVertexArray(borderVao_);

    // Reuse the same quad VBO bound at attrib 0/1 (pos/uv) as in your tile renderer:
    glBindBuffer(GL_ARRAY_BUFFER, quadVbo_);


    GLuint borderVbaLayout = 0;
    // aPos
    // aUVUnit
    setTileQuadVertexAttributes(borderVbaLayout);

    glGenBuffers(1, &borderVbo_);
    glBindBuffer(GL_ARRAY_BUFFER, borderVbo_);
    glBufferData(GL_ARRAY_BUFFER, borderTiles.size() * sizeof(TileInstanceData), borderTiles.data(), GL_STATIC_DRAW);

    // iTileXY (2x short)
    // iTileIndex (1x unsigned short)
    // iTileSizeMultiplier
    setTileInstanceVertexAttributes(borderVbaLayout);

    glBindVertexArray(0);
}

//////////////////////////////////////////////////////////////////////////

void TileRenderer::updateTileset(const Tileset& tileset)
{
    Q_ASSERT(mTexTileset);

    QImage img = tileset.pixmapWithExtraTiles().toImage(); // 304x224 bitmap

    if (!img.isNull())
    {
        QImage rgba = img.convertToFormat(QImage::Format_RGBA8888);

        Q_ASSERT(rgba.width() == TILESET_W);
        Q_ASSERT(rgba.height() == TILESET_WITH_EXTRA_H);

        mTexTileset->bind();
        mTexTileset->setData(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, rgba.constBits());
        mTexTileset->release();
    }
}


//////////////////////////////////////////////////////////////////////////

void TileRenderer::render(const LevelData* level, const LevelBounds& visibleArea, float zoomFactor, bool drawLevelTiles, bool drawBorderTiles)
{
    if (!level)
        return;

    if (!drawLevelTiles && !drawBorderTiles)
        return; //nothing to draw

    glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);



    int tx0 = 0;
    int ty0 = 0;
    int tx1 = MAP_W - 1;
    int ty1 = MAP_H - 1;

    // Prevent shimmering when zooming by forcing the pixel boundaries to be exact
    LevelBounds visibleAreaSnapped = visibleArea;

    if (EditorConfig::getConfig().mRenderAllTiles)
    {
        // Let's just build the instances for ALL the tiles
        // On most frames we'll be able to reuse it to save time,
        // When the level changes the cache will be cleared and it will be rebuilt
    }
    else
    {
        tx0 = visibleAreaSnapped.tileLeft();
        ty0 = visibleAreaSnapped.tileTop();
        tx1 = visibleAreaSnapped.tileRight();
        ty1 = visibleAreaSnapped.tileBottom();

        level->boundTileToLevel(tx0, ty0);
        level->boundTileToLevel(tx1, ty1);
    }


    // Bind program and set uniforms
    mProgram.bind();

    QMatrix4x4 mvp = visibleAreaSnapped.orthoPixels();

    mProgram.setUniformValue("uMVP", mvp);
    mProgram.setUniformValue("uZoom", zoomFactor);

    // Offset tiles by half a screen pixel to avoid flickering with the grid lines
    mProgram.setUniformValue("uDrawOffset", QVector2D(-0.25f / zoomFactor, -0.25f / zoomFactor));


    glActiveTexture(GL_TEXTURE0);
    mTexTileset->bind();

    if (drawLevelTiles)
    {
        TileCoords tbTopLeft(tx0, ty0);
        TileCoords tbBottomRight(tx1, ty1);

        if (tbTopLeft == mCachedBoundsTopLeft && tbBottomRight == mCachedBoundsBottomRight)
        {
            //use cached instances as is
        }
        else
        {
            // Build instance data for visible tiles
            // Use the previous cache's capacity
            std::vector<TileInstanceData> instances(std::move(mCachedInstances));
            instances.reserve((tx1 - tx0 + 1) * (ty1 - ty0 + 1));

            for (int y = ty0; y <= ty1; ++y)
            {
                const Tile* row = level->tiles().rowPtr(y);

                for (int x = tx0; x <= tx1; ++x)
                {
                    TileId v = row[x];

                    if (v /* && config.DrawSpecialTiles || v <= TILESET_COUNT */)
                        instances.push_back(TileInstanceData{
                            (int16_t)x, (int16_t)y, (uint16_t)(v - 1), (uint16_t)TileInfoDict[v].mSize
                            });
                    //else empty tile
                }
            }

            mCachedInstances = std::move(instances);
            mCachedBoundsTopLeft = tbTopLeft;
            mCachedBoundsBottomRight = tbBottomRight;
        }

        glBindVertexArray(vao_);

        // Update instance buffer
        glBindBuffer(GL_ARRAY_BUFFER, instanceVbo_);
        glBufferData(GL_ARRAY_BUFFER,
            mCachedInstances.size() * sizeof(TileInstanceData),
            mCachedInstances.empty() ? nullptr : mCachedInstances.data(),
            GL_DYNAMIC_DRAW);

        mProgram.setUniformValue("uDrawZ", ZCOORD_MAPTILES);

        // Draw 4-vertex quad instanced N times
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, GLsizei(mCachedInstances.size()));
    }

    if (drawBorderTiles)
    {
        if (visibleArea.tileLeft() < 0 ||
            visibleArea.tileRight() >= MAP_W ||
            visibleArea.tileTop() < 0 ||
            visibleArea.tileBottom() >= MAP_H)
        {
            mProgram.setUniformValue("uDrawZ", ZCOORD_MAPBORDER);

            glBindVertexArray(borderVao_);
            glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, GLsizei(BORDER_TILE_COUNT));
        }
    }

    mTexTileset->release();
    mProgram.release();
    glBindVertexArray(0);

    glPopAttrib();
}
