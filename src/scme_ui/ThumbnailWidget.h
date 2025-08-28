#ifndef INC_ThumbnailWidget_H
#define INC_ThumbnailWidget_H

#include "UiGlobal.h"

#include <QtOpenGLWidgets/QOpenGLWidget>

#include <QtCore/QPoint>
#include <QtCore/QPointF>
#include <QtCore/QRect>

#include <QtGui/QPen>

#include "Coords.h"


//////////////////////////////////////////////////////////////////////////

namespace SCME {

//////////////////////////////////////////////////////////////////////////

class SCME_LIB_DLL Editor;

//////////////////////////////////////////////////////////////////////////

class ThumbnailWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    ThumbnailWidget(Editor* editor, QWidget *parent = nullptr);
    virtual ~ThumbnailWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

public slots:

    void  redrawThumbnail();

    void  redrawView(const LevelBounds& viewBounds);

signals:

    void  doCenterView(const LevelCoords& center);

protected:
    void initializeGL();
    void resizeGL(int width, int height);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    void paintEvent(QPaintEvent *event);

    void drawDebug(QPainter& painter);

    void drawLevelBounds(QPainter& painter);

    void drawViewBounds(QPainter& painter);

    void drawObjects(QPainter& painter);

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
