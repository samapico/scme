#include "ViewBounds.h"
#include <QtCore/QVariantAnimation>


///////////////////////////////////////////////////////////////////////////

using namespace ::SCME;

struct DoOnce_t
{
    DoOnce_t()
    {
        qRegisterMetaType<SmoothViewBounds>();
        qRegisterAnimationInterpolator(SmoothViewBounds::interpolate);
    }
};

static DoOnce_t DoOnce;

///////////////////////////////////////////////////////////////////////////


QRectF SmoothViewBounds::interpolate(const QRectF& a, const QRectF& b, qreal t)
{
    return QRectF(
        interpolate(a.topLeft(), b.topLeft(), t),
        interpolate(a.bottomRight(), b.bottomRight(), t)
    );
}


///////////////////////////////////////////////////////////////////////////

QVariant SmoothViewBounds::interpolate(const SmoothViewBounds& a, const SmoothViewBounds& b, qreal t)
{
    if (t <= 0.0)
        return QVariant::fromValue(a);
    else if (t >= 1.0)
        return QVariant::fromValue(b);

    //Keep the interpolation simple if there is no zoom involved
    SmoothViewBounds newBounds(b.mScreenTarget);

    newBounds.mLevelTarget = LevelCoords(interpolate(a.mLevelTarget, b.mLevelTarget, t));

    //qDebug() << a.mScreenTarget.uv() << b.mScreenTarget.uv() << t << a.mZoomFactor << b.mZoomFactor;

    if (a.mZoomFactor == b.mZoomFactor)
    {
        newBounds.mViewBounds = LevelBounds(interpolate(a.mViewBounds, b.mViewBounds, t));
        newBounds.mZoomFactor = a.mZoomFactor; //no need to interpolate it
    }
    else
    {
        newBounds.mZoomFactor = interpolate(a.mZoomFactor, b.mZoomFactor, t);

        QPointF uv = interpolate(a.mScreenTarget.uv(), b.mScreenTarget.uv(), t);
        uv.setX(std::clamp(0.0, uv.x(), 1.0));
        uv.setY(std::clamp(0.0, uv.y(), 1.0));

        //Calculate what the bounds should be at the interpolated zoom factor,
        //if we want to keep the target pixel at the same place on the screen
        newBounds.mViewBounds = LevelBounds::fromTargetAndZoom(newBounds.mLevelTarget, uv, newBounds.mScreenTarget.screenSize(), newBounds.mZoomFactor);
    }

    return QVariant::fromValue(newBounds);
}
