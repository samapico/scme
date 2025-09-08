#ifndef INC_ThumbnailWidget_H
#define INC_ThumbnailWidget_H

#include "UiGlobal.h"

#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QtOpenGL/QOpenGLFunctions_3_3_Core>

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

class ThumbnailWidget : public QOpenGLWidget
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

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    void paintEvent(QPaintEvent *event);

    void drawDebug(QPainter& painter);

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
