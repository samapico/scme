#include "editorConfig.h"

//////////////////////////////////////////////////////////////////////////

EditorConfig::EditorConfig() :
    mGridPens(0),
    mGridSizes(0),
    mGridSizeCount(0),
    mMinZoom(1.0f/16.0f),
    mMaxZoom(4.0f),
    mWheelZoomSpeed(0.00208333333f) //25% per mouse wheel increment (typically delta = 120; 0.25 / 120 = 0.208333)
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

    QPen pen(QColor(128,128,128,128));
    mGridPens[0] = pen;

    pen.setColor(QColor(160,160,160,160));
    mGridPens[1] = pen;

    pen.setColor(QColor(192,192,192,192));
    mGridPens[2] = pen;

    pen.setColor(Qt::blue);
    mGridPens[3] = pen;

    pen.setColor(Qt::red);
    mGridPens[4] = pen;
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
