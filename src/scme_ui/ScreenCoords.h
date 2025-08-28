#ifndef INC_ScreenCoords_H
#define INC_ScreenCoords_H

#include "UiGlobal.h"
#include "Coords.h"


///////////////////////////////////////////////////////////////////////////

class QSinglePointEvent;


///////////////////////////////////////////////////////////////////////////

class QSinglePointEvent;


namespace SCME {

class EditorWidget;


///////////////////////////////////////////////////////////////////////////

class SCME_UI_DLL ScreenCoords : public QPointF
{
public:

    static constexpr inline float levelToScreen(float levelCoord, float screenFirstLevelCoord, float zoomFactor) { return (levelCoord - screenFirstLevelCoord) * zoomFactor; }

    static constexpr inline float screenToLevel(float screenPixel, float screenFirstLevelCoord, float zoomFactor) { return screenPixel / zoomFactor + screenFirstLevelCoord; }

    ScreenCoords() :
        QPointF(),
        mWidget(nullptr)
    {
    }

    ScreenCoords(const EditorWidget* w, const QPointF& xy) :
        QPointF(xy),
        mWidget(w)
    {
    }

    ScreenCoords(const EditorWidget* w, int x, int y) : ScreenCoords(w, QPointF(x, y)) {}

    ScreenCoords(const EditorWidget* w, const QPoint& xy) : ScreenCoords(w, xy.toPointF()) {}

    /// Screen coordinates from mouse event
    ScreenCoords(const EditorWidget* w, const QSinglePointEvent* pointEvent);

    /// Level coordinates to screen coordinates
    ScreenCoords(const EditorWidget* w, const LevelCoords& levelCoord);

    /// Copy constructor
    ScreenCoords(const ScreenCoords& orig) : ScreenCoords(orig.mWidget, orig) {}

    /// (x,y) coordinate in the screen as a ratio from 0 to 1
    QPointF uv() const;

    QSize screenSize() const;

    /// Screen coordinates to level coordinates
    LevelCoords toLevel() const;

    const EditorWidget* widget() const { return mWidget; }

private:

    const EditorWidget* mWidget = nullptr;
};


///////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////

} // End namespace SCME

//////////////////////////////////////////////////////////////////////////

#endif // INC_ScreenCoords_H
