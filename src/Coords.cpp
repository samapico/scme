#include "Coords.h"

#include <QtGui/QMouseEvent>

#include <QtCore/QPropertyAnimation>

#include "EditorWidget.h"


///////////////////////////////////////////////////////////////////////////

using namespace ::SCME;


//////////////////////////////////////////////////////////////////////////

ScreenCoords::ScreenCoords(const EditorWidget* w, const QSinglePointEvent* pointEvent) :
    QPointF(pointEvent->position()),
    mWidget(w)
{
}

ScreenCoords::ScreenCoords(const EditorWidget* w, const LevelCoords& levelCoord) :
    QPointF((levelCoord - w->viewTopLeft()) * w->zoomFactor()),
    mWidget(w)
{
}

//////////////////////////////////////////////////////////////////////////

LevelCoords::LevelCoords(const ScreenCoords& screenPixel) :
    LevelCoords(screenPixel.toLevel())
{
}

//////////////////////////////////////////////////////////////////////////

QPointF ScreenCoords::uv() const
{
    return QPointF(QPointF::x() / mWidget->width(), QPointF::y() / mWidget->height());
}

//////////////////////////////////////////////////////////////////////////

QSize ScreenCoords::screenSize() const
{
    if (!mWidget)
        return QSize();
    return mWidget->size();
}

//////////////////////////////////////////////////////////////////////////

ScreenCoords LevelCoords::toScreen(const EditorWidget* w)
{
    // Use the ScreenCoords::ScreenCoords(const EditorWidget*, const LevelCoords&) constructor
    return ScreenCoords(w, *this);
}

//////////////////////////////////////////////////////////////////////////

LevelCoords ScreenCoords::toLevel() const
{
    if (!mWidget)
        return LevelCoords(0, 0);

    return LevelCoords(
        QPointF( (*this / mWidget->zoomFactor()) + mWidget->viewTopLeft() )
    );
}

//////////////////////////////////////////////////////////////////////////

ScreenCoords ScreenCoords::boundToLevel() const
{
    LevelCoords level = toLevel();

    return ScreenCoords();
}



//////////////////////////////////////////////////////////////////////////

LevelCoords LevelBounds::bounded(const LevelCoords& coord) const
{
    LevelCoords bounded = coord;

    if (bounded.x() < 0)
        bounded.setX(0);
    else if (bounded.x() > width())
        bounded.setX(width());

    if (bounded.y() < 0)
        bounded.setY(0);
    else if (bounded.y() > height())
        bounded.setY(height());

    return bounded;
}
