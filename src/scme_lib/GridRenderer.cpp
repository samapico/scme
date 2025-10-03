#include "GridRenderer.h"

#include "EditorConfig.h"
#include "Coords.h"
#include "LevelData.h"



///////////////////////////////////////////////////////////////////////////

using namespace ::SCME;

//////////////////////////////////////////////////////////////////////////


GridRenderer::~GridRenderer()
{
    mProgram.removeAllShaders();
}

//////////////////////////////////////////////////////////////////////////

void GridRenderer::init()
{
    Renderer::init();

    glGenVertexArrays(1, &mVao);

    // --- Load shaders ---
    mProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/grid_fs.glsl");
    mProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/grid_vs.glsl");
    mProgram.link();
}


//////////////////////////////////////////////////////////////////////////

void GridRenderer::render(const LevelData* level, const LevelBounds& visibleArea, float zoomFactor)
{
    if (!level)
        return;

    const EditorConfig& cfg = EditorConfig::getConfig();

    glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

    // Enable blending
    glEnable(GL_BLEND);
    // Choose a blending function
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Prevent shimmering when zooming by forcing the pixel boundaries to be exact
    LevelBounds visibleAreaSnapped = visibleArea;

    if (zoomFactor <= 1.0f)
        visibleAreaSnapped = visibleAreaSnapped.rounded();

    // Bind program and set uniforms
    mProgram.bind();

    QMatrix4x4 mvp = visibleAreaSnapped.orthoPixels();

    mProgram.setUniformValue("uMVP", mvp);
    mProgram.setUniformValue("uZoom", zoomFactor);

    mProgram.setUniformValue("uBounds", QVector4D(visibleAreaSnapped.left(), visibleAreaSnapped.top(), visibleAreaSnapped.right(), visibleAreaSnapped.bottom()));
    mProgram.setUniformValue("uDrawZ", cfg.mGridDrawOverTiles ? -ZCOORD_GRID_OVERTILES : -ZCOORD_GRID_UNDERTILES);


    const float pixelsPerTile = TILE_W * zoomFactor;

    GLfloat gridSpacings[EditorConfig::GRIDLEVELS];
    GLfloat gridThickness[EditorConfig::GRIDLEVELS];
    QVector4D gridColors[EditorConfig::GRIDLEVELS];

    //Adjust colors depending on current zoom; smallest grid squares fade out when zooming out
    int gridCount = 0;
    for (int i = 0; i < EditorConfig::GRIDLEVELS; i++)
    {
        int spacing = cfg.mGridSizes[i];
        gridSpacings[i] = static_cast<GLfloat>(spacing);

        QPen gridPen = cfg.getGridPen(spacing, pixelsPerTile);

        gridColors[i] = colorToVec4(gridPen.color());
        gridThickness[i] = gridPen.widthF();

        if (gridColors[i].w() > 0)
            gridCount++;
    }

    if (gridCount == 0)
        gridCount = 1; //force at least 1 level

    mProgram.setUniformValue("uTileSize", TILE_SIZE.toSizeF());

    static_assert(sizeof(float) == sizeof(GLfloat));
    mProgram.setUniformValueArray("uGridSpacings", gridSpacings, EditorConfig::GRIDLEVELS, 1);
    mProgram.setUniformValue("uSmallestGrid", gridSpacings[gridCount - 1]);

    mProgram.setUniformValueArray("uGridColors", gridColors, EditorConfig::GRIDLEVELS);
    mProgram.setUniformValueArray("uGridThickness", gridThickness, EditorConfig::GRIDLEVELS, 1);

    glBindVertexArray(mVao);

    //Draw a single large triangle that covers the screen
    glDrawArrays(GL_TRIANGLES, 0, 3);

    mProgram.release();
    glBindVertexArray(0);

    glPopAttrib();
}
