#ifndef INC_FrameCounter_H
#define INC_FrameCounter_H


#include "UiGlobal.h"

#include <QtCore/QObject>

#include <memory>


///////////////////////////////////////////////////////////////////////////

class QTimer;
class QOpenGLWidget;

///////////////////////////////////////////////////////////////////////////

namespace SCME {

//////////////////////////////////////////////////////////////////////////

class SCME_UI_DLL FrameCounter : public QObject
{
    Q_OBJECT

signals:

    void framesCounted(double framesPerSecond);

public:
    FrameCounter(QObject* parent = nullptr);

    virtual ~FrameCounter();

    void stop();

    void start();

    /// Should be called from QOpenGLWidget::paintGL
    void onFrameRendered();

    double lastFPS() const;

private:

    void onTimerExpired();

    int mFrameCount = 0;

    double mLastFPS = 0.0;

    QTimer* mTimerFpsUpdate = nullptr;
};


///////////////////////////////////////////////////////////////////////////

} // End namespace SCME

///////////////////////////////////////////////////////////////////////////


#endif // INC_FrameCounter_H
