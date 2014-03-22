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

    float wheelZoomSpeed() const;

    float minZoom() const;

    float maxZoom() const;

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
};

#endif // EDITORCONFIG_H
