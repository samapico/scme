#include "MinimapRenderer.h"

#include "Coords.h"
#include "LevelData.h"

#include <QtCore/QRectF>
#include <QtGui/QPainter>
#include <QtGui/QImage>



///////////////////////////////////////////////////////////////////////////

using namespace ::SCME;



//////////////////////////////////////////////////////////////////////////

MinimapRenderer::~MinimapRenderer()
{
}

//////////////////////////////////////////////////////////////////////////

void MinimapRenderer::init()
{
    //Renderer::init();
}

//////////////////////////////////////////////////////////////////////////

void MinimapRenderer::render(QPainter& painter, const LevelData* level, const QRectF& screenPixels, const QRectF& mapTiles, float zoomFactor)
{
    if (!level)
        return;

    painter.save();

    const QImage* img = level->tiles().imageConst();
    if (img)
    {
        painter.beginNativePainting();

        //Use exact pixel scale for 1:16 zoom (or more zoomed in), and use smooth interpolation when zoomed out more
        painter.setRenderHint(QPainter::SmoothPixmapTransform, zoomFactor < 1.f / 17.f);

        painter.drawImage(
            screenPixels,
            *img,
            mapTiles
        );

        painter.endNativePainting();
    }

    painter.restore();
}
