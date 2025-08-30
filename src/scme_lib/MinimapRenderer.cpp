#include "MinimapRenderer.h"

#include "Coords.h"
#include "TileInfo.h"
#include "LevelData.h"

#include <QtGui/QImage>



///////////////////////////////////////////////////////////////////////////

using namespace ::SCME;

//////////////////////////////////////////////////////////////////////////

typedef QList<uint> PixelPalette;

static PixelPalette getColorTableForTiles()
{
    qDebug() << "@getPixelPalette"  << TilePixelColorPalette::sCurrentPalette.mPixelColorPalette;

    PixelPalette pp(256, Qt::Uninitialized);
    for (int i = 0; i < 256; i++)
        pp[i] = TileInfoDict.at(i).pixelColor().rgba();
    return pp;
}


//////////////////////////////////////////////////////////////////////////

MinimapRenderer::~MinimapRenderer()
{
}

//////////////////////////////////////////////////////////////////////////

void MinimapRenderer::init()
{
    Renderer::init();

    mRadarImage = nullptr;
}

//////////////////////////////////////////////////////////////////////////

void MinimapRenderer::allocateImage(std::shared_ptr<LevelData> level)
{
    if (level != mLevel)
    {
        if (!level)
        {
            //Clear image
            mRadarImage = nullptr;
        }
        else
        {
            //Reallocate image with the Tile array bytes of the level
            Tile* t0 = level->tiles().rowPtr(0);

            mRadarImage = std::make_shared<QImage>(
                reinterpret_cast<uchar*>(t0),
                level->tiles().width(),
                level->tiles().height(),
                QImage::Format::Format_Indexed8);
            mRadarImage->setColorTable(getColorTableForTiles());

            Q_ASSERT(mRadarImage->sizeInBytes() == MAP_W * MAP_H * size_t(1));

            Q_ASSERT(mRadarImage->bits() == reinterpret_cast<uchar*>(level->tiles().rowPtr(0)));


            qDebug() << "@allocateImage: img data ptr = " << mRadarImage->bits() << "tileptr=" << level->tiles().rowPtr(0);
        }

        mLevel = level;
    }
}

//////////////////////////////////////////////////////////////////////////

void MinimapRenderer::refreshMinimap(std::shared_ptr<LevelData> level)
{
    refreshMinimap(level, level ? level->bounds() : LevelBounds());
}

//////////////////////////////////////////////////////////////////////////

void MinimapRenderer::refreshMinimap(std::shared_ptr<LevelData> level, const LevelBounds& bounds)
{
    allocateImage(level);

    if (!level)
        return;

    /// Image is already mapped to the tile values, nothing to do here
}


//////////////////////////////////////////////////////////////////////////

std::shared_ptr<QImage> MinimapRenderer::image() const
{
    return mRadarImage;
}

//////////////////////////////////////////////////////////////////////////

void MinimapRenderer::render(const LevelData* level, const LevelBounds& visibleArea, float zoomFactor)
{
    if (!mLevel)
        return;
}
