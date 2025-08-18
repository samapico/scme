#ifndef THUMBNAILWIDGET_H
#define THUMBNAILWIDGET_H

#include "Global.h"

#include <QtOpenGLWidgets/QOpenGLWidget>

#include <QtCore/QPoint>
#include <QtCore/QPointF>
#include <QtCore/QRect>

#include <QtGui/QPen>


//////////////////////////////////////////////////////////////////////////

namespace SCME {

//////////////////////////////////////////////////////////////////////////

class Editor;

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

    void  redrawView(const QRect& viewBounds);

signals:

    void  doCenterView(const QPoint& center);

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

    QPoint  screenToLevelPixel(const QPoint& screenxy);

    Editor* mEditor;

    /// Current area of our widget in which the thumbnail is drawn
    QRect   mThumbnailArea;
    QPointF mThumbnailScale;

    QRect   mViewBounds;

    QPen    mLevelBoundsPen;

    QPen    mViewBoundsPen;
};


///////////////////////////////////////////////////////////////////////////

} // End namespace SCME

///////////////////////////////////////////////////////////////////////////

#endif // THUMBNAILWIDGET_H
