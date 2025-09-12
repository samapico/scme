#ifndef INC_ThumbnailWidget_H
#define INC_ThumbnailWidget_H

#include "UiGlobal.h"

#include <QtWidgets/QWidget>

#include <QtCore/QPoint>
#include <QtCore/QPointF>
#include <QtCore/QRect>

#include <QtGui/QPen>

#include "Coords.h"


//////////////////////////////////////////////////////////////////////////

namespace SCME {

//////////////////////////////////////////////////////////////////////////

class Editor;
class LevelData;


//////////////////////////////////////////////////////////////////////////

class ThumbnailWidget : public QWidget
{
    Q_OBJECT

public:
    ThumbnailWidget(Editor* editor, QWidget *parent = nullptr);
    virtual ~ThumbnailWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

public slots:

    void redrawThumbnail();

    void redrawViewBounds(const LevelBounds& viewBounds);

    void redrawLevel(const LevelData* level);

signals:

    void doCenterView(const LevelCoords& center);

    void cursorMoved(const LevelCoords& coords);

protected:
    void resizeEvent(QResizeEvent* event) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    void paintEvent(QPaintEvent *event) override;

    void drawLevelBounds(QPainter& painter);

    void drawViewBounds(QPainter& painter);

    void drawLevel(QPainter& painter);

private:

    typedef QPointF ThumbnailScreenCoords;

    LevelCoords  screenToLevelPixel(const ThumbnailScreenCoords& screenxy);

    Editor* mEditor;

    /// Current area of our widget in which the thumbnail is drawn
    QRect   mThumbnailArea;
    QPointF mThumbnailScale;

    LevelBounds mViewBounds;

    QPen    mLevelBoundsPen;

    QPen    mViewBoundsPen;
};


///////////////////////////////////////////////////////////////////////////

} // End namespace SCME

///////////////////////////////////////////////////////////////////////////

#endif // INC_ThumbnailWidget_H
