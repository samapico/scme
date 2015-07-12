#include "EditorConfig.h"

#include <QtCore/QDebug>

//////////////////////////////////////////////////////////////////////////

EditorConfig::EditorConfig() :
    mGridPens(0),
    mGridSizes(0),
    mGridSizeCount(0)
{
    setDefaultConfig();
}

//////////////////////////////////////////////////////////////////////////

EditorConfig::~EditorConfig()
{
    delete[] mGridPens;
    delete[] mGridSizes;
}

//////////////////////////////////////////////////////////////////////////

const QPen& EditorConfig::getGridPen(int tile) const
{
    Q_ASSERT(mGridSizeCount > 0);

    int i = mGridSizeCount;

    while (i)
    {
        --i;
        if (!(tile % mGridSizes[i]))
            return mGridPens[i];
    }

    if (mGridSizeCount)
        return mGridPens[0];
    
    Q_ASSERT(0);
    return mDefaultPen;
}

//////////////////////////////////////////////////////////////////////////

void EditorConfig::setDefaultConfig()
{
    delete[] mGridPens;
    delete[] mGridSizes;

    //Default config
    mGridSizeCount = 5;

    mGridPens = new QPen[mGridSizeCount];
    mGridSizes= new int[mGridSizeCount];

    mGridSizes[0] = 1;
    mGridSizes[1] = 4;
    mGridSizes[2] = 16;
    mGridSizes[3] = 64;
    mGridSizes[4] = 256;

    setGridPreset(Grey);

    mWheelZoomSpeed = 0.00208333333f; //25% per mouse wheel increment (typically delta = 120; 0.25 / 120 = 0.00208333333f)
    mMinZoom = 0.01152921504606846976f; //Power of 1.25
    mMaxZoom = 4.76837158203125f; //Power of 1.25
    
    mSmoothCameraTime = 500;

    mSmoothDragSpeed = 0.0; //Multiplier; 0 disables it
}

//////////////////////////////////////////////////////////////////////////

void EditorConfig::setGridPreset(GridPreset preset)
{
    QColor c;

    for (int i = 0; i < mGridSizeCount; i++)
    {
        //primary color = 0.5 + 0.5*i/(count-1)
        float p = qMin(1.0f, 0.5f + 0.5f * ((float)(i)) / (mGridSizeCount - 1));

        //secondary color = 0 + 0.5*(i-1)/(count-1)
        float s = qMax(0.0f, 0.5f * ((float)(i))/(mGridSizeCount - 1));

        float t = qMax(0.0f, 0.25f * ((float)(i-2))/(mGridSizeCount - 1));

        float a = 0.25f + 0.50f * ((float)i / (mGridSizeCount - 1));

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
        c.setHsv((c.hue() + (int)(60*((float)(i))/(mGridSizeCount - 1)))%360, c.saturation()*.75, c.value()*.75, a*255);

        mGridPens[i].setColor(c);
    }
}

//////////////////////////////////////////////////////////////////////////

float EditorConfig::wheelZoomSpeed() const
{
    return mWheelZoomSpeed;
}

//////////////////////////////////////////////////////////////////////////

float EditorConfig::minZoom() const
{
    return mMinZoom;
}

//////////////////////////////////////////////////////////////////////////

float EditorConfig::maxZoom() const
{
    return mMaxZoom;
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
