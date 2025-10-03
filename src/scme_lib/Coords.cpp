#include "Coords.h"

#include <QtGui/QMouseEvent>

#include <QtCore/QPropertyAnimation>


///////////////////////////////////////////////////////////////////////////

using namespace ::SCME;


//////////////////////////////////////////////////////////////////////////



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



//////////////////////////////////////////////////////////////////////////

LevelBounds LevelBounds::rounded() const
{
    return LevelBounds(QRectF(
        qRound(left()),
        qRound(top()),
        qRound(width()),
        qRound(height())
    ));
}
