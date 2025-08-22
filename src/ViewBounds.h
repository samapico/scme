#ifndef INC_ViewBounds_H
#define INC_ViewBounds_H

#include "Global.h"

#include "Coords.h"
#include <QMetaType>

///////////////////////////////////////////////////////////////////////////

class QSinglePointEvent;

///////////////////////////////////////////////////////////////////////////

namespace SCME {


class SmoothViewBounds
{
public:

    SmoothViewBounds() = default;

    SmoothViewBounds(const ScreenCoords& screenTarget) :
        mScreenTarget(screenTarget)
    {
    }

    /// Rectangular view area that can be interpolated
    /// The center of those bounds is what is used to move the camera
    LevelBounds mViewBounds;

    /// Accurate zoom factor that we want
    /// Recalculating the zoom factor from the view bounds would lead to inaccurate values
    /// that do not match the zoom steps, so we want to accurately interpolate the desired zoom
    float mZoomFactor = 1.0f;

    /// Pixel on the screen that needs to remain aligned with mLevelTarget during a zoom operation
    ScreenCoords mScreenTarget;

    /// Point on the level that needs to remain aligned with mScreenTarget on the screen during a zoom operation
    LevelCoords mLevelTarget;

    static inline qreal interpolate(qreal a, qreal b, qreal t) { return a + (b - a) * t; }
    static inline QPointF interpolate(const QPointF& a, const QPointF& b, qreal t) { return a + (b - a) * t; }
    static QRectF interpolate(const QRectF& a, const QRectF& b, qreal t);

    static QVariant interpolate(const SCME::SmoothViewBounds& a, const SCME::SmoothViewBounds& b, qreal t);
};


///////////////////////////////////////////////////////////////////////////

} // End namespace SCME

Q_DECLARE_METATYPE(SCME::SmoothViewBounds);




//////////////////////////////////////////////////////////////////////////

#endif // INC_ViewBounds_H
