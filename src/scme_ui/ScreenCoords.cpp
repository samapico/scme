#include "ScreenCoords.h"

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

LevelCoords ScreenCoords::toLevel() const
{
    if (!mWidget)
        return LevelCoords(0, 0);

    return LevelCoords(
        QPointF( (*this / mWidget->zoomFactor()) + mWidget->viewTopLeft() )
    );
}

