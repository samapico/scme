#include "EditorConfig.h"

#include <QtCore/QDebug>


///////////////////////////////////////////////////////////////////////////

using namespace ::SCME;

//////////////////////////////////////////////////////////////////////////

EditorConfig::EditorConfig()
{
    mDefaultPen.setColor(QColor(0, 0, 0, 0));
    setDefaultConfig();
}

//////////////////////////////////////////////////////////////////////////

EditorConfig::~EditorConfig()
{
}

//////////////////////////////////////////////////////////////////////////

QPen EditorConfig::getGridPen(int tile, float pixelsPerTile) const
{
    Q_ASSERT(mGridSizes.count() > 0);

    int i = mGridSizes.count();

    while (i)
    {
        --i;
        if (!(tile % mGridSizes[i]))
        {
            QPen pen = mGridPens[i];
            QColor c = pen.color();

            float pixelsPerGrid = pixelsPerTile * mGridSizes[i];

            if (pixelsPerGrid <= mPixelsPerGridFadeIn)
            {
                //Grid too small
                c.setAlpha(0);
                pen.setColor(c);
            }
            else if (pixelsPerGrid >= mPixelsPerGridFadeOut)
            {
                //Keep alpha as is
            }
            else
            {
                //Interpolate alpha between its original value and 0
                c.setAlphaF(c.alphaF() * (pixelsPerGrid - mPixelsPerGridFadeIn) / (mPixelsPerGridFadeOut - mPixelsPerGridFadeIn));
                pen.setColor(c);
            }

            return pen;
        }
    }

    //No grid match
    return mDefaultPen;
}

//////////////////////////////////////////////////////////////////////////

void EditorConfig::setDefaultConfig()
{
    //Default config
    mGridSizes = { 1, 4, 16, 64, 256 };

    setGridPreset(Grey);
    Q_ASSERT(mGridSizes.count() == mGridPens.count());

    addZoomLevels(1.0f/32.0f, 8.0f, 2.0f);

    mPixelsPerGridFadeIn = 2.75f;
    mPixelsPerGridFadeOut = 4.0f;

    mSmoothCameraTime = 350;

    mSmoothDragSpeed = 0.0; //Multiplier; 0 disables it
}

//////////////////////////////////////////////////////////////////////////

void EditorConfig::setGridPreset(GridPreset preset)
{
    QColor c;

    mGridPens.resize(mGridSizes.count());

    for (int i = 0; i < mGridSizes.count(); i++)
    {
        //primary color = 0.5 + 0.5*i/(count-1)
        float p = qMin(1.0f, 0.5f + 0.5f * ((float)(i)) / (mGridSizes.count() - 1));

        //secondary color = 0 + 0.5*(i-1)/(count-1)
        float s = qMax(0.0f, 0.5f * ((float)(i))/(mGridSizes.count() - 1));

        float t = qMax(0.0f, 0.25f * ((float)(i-2))/(mGridSizes.count() - 1));

        float a = 0.25f + 0.50f * ((float)i / (mGridSizes.count() - 1));

        if (preset == Grey)
            c.setRgbF(p, p, p, a);
        else if (preset == RG)
            c.setRgbF(p, s, t, a);
        else if (preset == RB)
            c.setRgbF(p, t, s, a);
        else if (preset == GR)
            c.setRgbF(s, p, t, a);
        else if (preset == GB)
            c.setRgbF(t, p, s, a);
        else if (preset == BR)
            c.setRgbF(s, t, p, a);
        else //BG
            c.setRgbF(t, s, p, a);

        //shift hue and grey it out a bit
        c.setHsv((c.hue() + (int)(60*((float)(i))/(mGridSizes.count() - 1)))%360, c.saturation()*.75, c.value()*.75, a*255);

        mGridPens[i].setColor(c);
    }
}

//////////////////////////////////////////////////////////////////////////

float EditorConfig::minZoomFactor() const
{
    return mZoomOutFactors.isEmpty() ? 1.0f : mZoomOutFactors.last();
}

//////////////////////////////////////////////////////////////////////////

float EditorConfig::maxZoomFactor() const
{
    return mZoomInFactors.isEmpty() ? 1.0f : mZoomInFactors.last();
}

//////////////////////////////////////////////////////////////////////////

float EditorConfig::zoomFactorAtIndex(int zoomIndex) const
{
    if (zoomIndex == 0)
        return 1.0f;

    if (zoomIndex > 0)
        return zoomIndex > mZoomInFactors.count() ? maxZoomFactor() : mZoomInFactors.at(zoomIndex - 1);

    //zoomIndex is negative
    zoomIndex = -zoomIndex; //convert to a usable positive index

    return zoomIndex > mZoomOutFactors.count() ? minZoomFactor() : mZoomOutFactors.at(zoomIndex - 1);
}

//////////////////////////////////////////////////////////////////////////

int EditorConfig::zoomIndexMin() const
{
    return -mZoomOutFactors.count();
}

//////////////////////////////////////////////////////////////////////////

int EditorConfig::zoomIndexMax() const
{
    return mZoomInFactors.count();
}

//////////////////////////////////////////////////////////////////////////

int EditorConfig::smoothCameraTime() const
{
    return mSmoothCameraTime;
}

///////////////////////////////////////////////////////////////////////////

float EditorConfig::smoothDragSpeed() const
{
    return mSmoothDragSpeed;
}

///////////////////////////////////////////////////////////////////////////

void EditorConfig::addZoomLevels(float minZoom, float maxZoom, float mult)
{
    mZoomOutFactors.clear();
    mZoomInFactors.clear();

    float z = 1.0f;

    while (z > minZoom)
    {
        z /= mult;

        mZoomOutFactors.append(z - 1e-6f < minZoom ? minZoom : z);
    }

    z = 1.0f;
    while (z < maxZoom)
    {
        z *= mult;

        mZoomInFactors.append(z + 1e-6f > maxZoom ? maxZoom : z);
    }
}
