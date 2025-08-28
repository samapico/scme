#include "TileRenderer.h"

#include "Tileset.h"
#include "Coords.h"
#include "LevelData.h"

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
    cachedInstances.clear();
    cachedBoundsTopLeft = {};
    cachedBoundsBottomRight = {};
}

//////////////////////////////////////////////////////////////////////////

void TileRenderer::init(const Tileset& tileset)
{
    Renderer::init();

    // --- Load tileset texture ---
    {
        QImage img = tileset.image(); // 304x160 bitmap

        if (!img.isNull())
        {
            QImage rgba = img.convertToFormat(QImage::Format_RGBA8888);

            mTexTileset = std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D);
            mTexTileset->create();
            mTexTileset->bind();

            Q_ASSERT(rgba.width() == TILESET_W);
            Q_ASSERT(rgba.height() == TILESET_H);

            mTexTileset->setSize(rgba.width(), rgba.height());
            mTexTileset->setFormat(QOpenGLTexture::RGBA8_UNorm);
            mTexTileset->allocateStorage();
            mTexTileset->setMinificationFilter(QOpenGLTexture::Linear);
            mTexTileset->setMagnificationFilter(QOpenGLTexture::Nearest);
            mTexTileset->setWrapMode(QOpenGLTexture::ClampToEdge);
            mTexTileset->setData(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, rgba.constBits());
            mTexTileset->release();
        }

        qDebug() << "@TileRenderer::init" << img.format();
    }

    // --- Load shaders ---

    mProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/tile_vs.glsl");
    mProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/tile_fs.glsl");
    mProgram.link();

    // --- Geometry: a 16x16 quad, triangle strip ---
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    struct V { float x, y; float u, v; };
    V quad[4] = {
        {     0,      0, 0, 0},
        {TILE_W,      0, 1, 0},
        {     0, TILE_H, 0, 1},
        {TILE_W, TILE_H, 1, 1},
    };

    glGenBuffers(1, &quadVbo_);
    glBindBuffer(GL_ARRAY_BUFFER, quadVbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    // aPos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(V), (void*)0);

    // aUVUnit
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(V), (void*)(2 * sizeof(float)));

    // --- Instance buffer ---
    glGenBuffers(1, &instanceVbo_);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVbo_);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW); // we’ll resize per frame

    // iTileXY (2x unsigned short)
    glEnableVertexAttribArray(2);
    glVertexAttribIPointer(2, 2, GL_UNSIGNED_SHORT, sizeof(TileInstanceData), (void*)0);
    glVertexAttribDivisor(2, 1);

    // iTileIndex (1x unsigned short promoted to uint)
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 1, GL_UNSIGNED_SHORT, sizeof(TileInstanceData), (void*)(2 * sizeof(uint16_t)));
    glVertexAttribDivisor(3, 1);

    glBindVertexArray(0);
}

//////////////////////////////////////////////////////////////////////////

void TileRenderer::updateTileset(const Tileset& tileset)
{
    Q_ASSERT(mTexTileset);

    QImage img = tileset.image(); // 304x160 bitmap

    if (!img.isNull())
    {
        QImage rgba = img.convertToFormat(QImage::Format_RGBA8888);

        Q_ASSERT(rgba.width() == TILESET_W);
        Q_ASSERT(rgba.height() == TILESET_H);

        mTexTileset->bind();
        mTexTileset->setData(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, rgba.constBits());
        mTexTileset->release();
    }
}


//////////////////////////////////////////////////////////////////////////

void TileRenderer::render(const LevelData* level, const LevelBounds& visibleArea, float zoomFactor)
{
    if (!level)
        return;

    // Prevent shimmering when zooming by forcing the pixel boundaries to be exact
    LevelBounds visibleAreaSnapped = visibleArea;
    //visibleAreaSnapped.setLeft(std::floor(visibleAreaSnapped.left()));
    //visibleAreaSnapped.setTop(std::floor(visibleAreaSnapped.top()));

    int tx0 = visibleAreaSnapped.tileLeft();
    int ty0 = visibleAreaSnapped.tileTop();
    int tx1 = visibleAreaSnapped.tileRight();
    int ty1 = visibleAreaSnapped.tileBottom();

    level->boundTileToLevel(tx0, ty0);
    level->boundTileToLevel(tx1, ty1);

    TileCoords tbTopLeft(tx0, ty0);
    TileCoords tbBottomRight(tx1, ty1);

    // Build instance data for visible tiles
    std::vector<TileInstanceData> instances;

    if (tbTopLeft == cachedBoundsTopLeft && tbBottomRight == cachedBoundsBottomRight)
    {
        instances = cachedInstances;
    }
    else
    {
        instances.reserve((tx1 - tx0 + 1) * (ty1 - ty0 + 1));

        for (int y = ty0; y <= ty1; ++y)
        {
            const Tile* row = level->tiles().rowPtr(y);

            for (int x = tx0; x <= tx1; ++x)
            {
                TileId v = row[x];

                if (v && v <= TILESET_COUNT)
                    instances.push_back(TileInstanceData{
                        (uint16_t)x, (uint16_t)y, (uint16_t)(v - 1), (uint16_t)0
                        });
                //else empty tile
            }
        }

        cachedInstances = instances;
        cachedBoundsTopLeft = tbTopLeft;
        cachedBoundsBottomRight = tbBottomRight;
    }

    glBindVertexArray(vao_);

    // Update instance buffer
    glBindBuffer(GL_ARRAY_BUFFER, instanceVbo_);
    glBufferData(GL_ARRAY_BUFFER,
        instances.size() * sizeof(TileInstanceData),
        instances.empty() ? nullptr : instances.data(),
        GL_DYNAMIC_DRAW);

    // Bind program and set uniforms
    mProgram.bind();

    QMatrix4x4 mvp = visibleAreaSnapped.orthoPixels();

    mProgram.setUniformValue("uMVP", mvp);
    mProgram.setUniformValue("uAtlasSize", QVector2D(TILESET_W, TILESET_H));
    mProgram.setUniformValue("uTilePx", QVector2D(TILE_W, TILE_H));

    GLint loc = mProgram.uniformLocation("uGridDim");
    glUniform2ui(loc, TILESET_COUNT_W, TILESET_COUNT_H);

    mProgram.setUniformValue("uAtlas", 0);
    glActiveTexture(GL_TEXTURE0);
    mTexTileset->bind();

    // Draw 4-vertex quad instanced N times
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, GLsizei(instances.size()));

    mTexTileset->release();
    mProgram.release();
    glBindVertexArray(0);
}
