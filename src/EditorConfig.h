#ifndef EDITORCONFIG_H
#define EDITORCONFIG_H

#include <QtGui/QPen>

class EditorConfig
{

public:
    EditorConfig();
    virtual ~EditorConfig();
    
    const QPen& getGridPen(int tile) const;

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

        GridPresetCount
    };
    void setGridPreset(GridPreset);

    /// Wheel zoom multiplier
    float wheelZoomSpeed() const;

    /// Lowest possible zoom factor (zoomed out)
    float minZoom() const;

    /// Highest possible zoom factor (zoomed in)
    float maxZoom() const;

    /// Time it takes to complete the transition from one view to another (0 means no smoothing; instant transition)
    int smoothCameraTime() const;

    /// Scrolling speed when we let go of the mouse while panning (0 means it stops instantly)
    /// @todo This should end up in the panning tool's config
    float smoothDragSpeed() const;

private:

    //do not allow copy or assignment of this class (not yet supported)
    EditorConfig(const EditorConfig&);
    const EditorConfig& operator=(const EditorConfig&);    

    int   mGridSizeCount;
    int * mGridSizes;
    QPen* mGridPens;

    QPen  mDefaultPen;

    float mWheelZoomSpeed;
    float mMinZoom;
    float mMaxZoom;

    int   mSmoothCameraTime;
    float mSmoothDragSpeed;
};

#endif // EDITORCONFIG_H
