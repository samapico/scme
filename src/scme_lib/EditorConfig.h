#ifndef INC_EditorConfig_H
#define INC_EditorConfig_H

#include "Global.h"

#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtGui/QPen>


///////////////////////////////////////////////////////////////////////////

namespace SCME {

///////////////////////////////////////////////////////////////////////////

class SCME_LIB_DLL EditorConfig : public QObject
{
    Q_OBJECT
signals:

    void configChanged(EditorConfig* config);

public:

    static EditorConfig sGlobalConfig;

    EditorConfig();
    ~EditorConfig();

    QPen getGridPen(int tile, float pixelsPerTile) const;

    void setDefaultConfig();

    enum GridPreset
    {
        Grey,
        RG,
        RB,
        GR,
        GB,
        BR,
        BG,

        Custom,

        GridPresetCount
    };
    void setGridPreset(GridPreset);

    /// Lowest possible zoom factor (zoomed out)
    float minZoomFactor() const;

    /// Highest possible zoom factor (zoomed in)
    float maxZoomFactor() const;

    /// Zoom value at a given zoom index, where 0 is no zoom, greater than 0 is a zoom in, and less than 0 is a zoom out
    float zoomFactorAtIndex(int zoomIndex) const;

    int zoomIndexMin() const;

    int zoomIndexMax() const;

    /// Time it takes to complete the transition from one view to another (0 means no smoothing; instant transition)
    int smoothCameraTime() const;

    /// Scrolling speed when we let go of the mouse while panning (0 means it stops instantly)
    /// @todo This should end up in the panning tool's config
    float smoothDragSpeed() const;

    float pixelViewOpacityAtZoom(float zoomFactor) const;

private:

    void addZoomLevels(float minZoom, float maxZoom, float mult);

    float mGridPresetAlphaNear = .20f;
    float mGridPresetAlphaFar = .40f;
    float mGridPresetHueShift = 75.f;
    float mGridPresetSaturationFactor = .9f;
    float mGridPresetValueFactor = .9f;

    QVector<int> mGridSizes;
    QVector<QPen> mGridPens;

    QPen mDefaultPen;

    QVector<float> mZoomOutFactors;
    QVector<float> mZoomInFactors;

    int   mSmoothCameraTime;
    float mSmoothDragSpeed;
    float mPixelsPerGridFadeIn; //0% opacity if pixels per grid is <= this value
    float mPixelsPerGridFadeOut; //100% opacity if pixels per grid is >= this value

    float mPixelViewZoomFadeIn; //0% pixel view when zoom is >= this value
    float mPixelViewZoomFadeOut; //100% pixel view when zoom is <= this value
};


///////////////////////////////////////////////////////////////////////////

} // End namespace SCME

#endif // INC_EditorConfig_H
