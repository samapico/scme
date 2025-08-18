#include "FrameCounter.h"

#include <QtCore/QTimer>
#include <QtCore/QDebug>

///////////////////////////////////////////////////////////////////////////

using namespace ::SCME;

//////////////////////////////////////////////////////////////////////////

FrameCounter::FrameCounter(QObject* parent /*= nullptr*/) :
    QObject(parent)
{
}

//////////////////////////////////////////////////////////////////////////

FrameCounter::~FrameCounter()
{
    stop();
}

//////////////////////////////////////////////////////////////////////////

void FrameCounter::start()
{
    stop();

    if (!mTimerFpsUpdate)
    {
        mTimerFpsUpdate = new QTimer(this);
        mTimerFpsUpdate->setTimerType(Qt::TimerType::PreciseTimer);
        mTimerFpsUpdate->setInterval(1000);
    }

    connect(mTimerFpsUpdate, &QTimer::timeout, this, &FrameCounter::onTimerExpired);
    mTimerFpsUpdate->start();
}

//////////////////////////////////////////////////////////////////////////

void FrameCounter::stop()
{
    //stop counting
    delete mTimerFpsUpdate;
    mTimerFpsUpdate = nullptr;
}

//////////////////////////////////////////////////////////////////////////

void FrameCounter::onFrameRendered()
{
    mFrameCount++;
}

//////////////////////////////////////////////////////////////////////////

double FrameCounter::lastFPS() const
{
    return mLastFPS;
}

//////////////////////////////////////////////////////////////////////////

void FrameCounter::onTimerExpired()
{
    double secsElapsed = 1.0; //Assume timer is accurate, for now

    double fps = static_cast<double>(mFrameCount) / secsElapsed;
    mLastFPS = fps;
    emit framesCounted(fps);

    mFrameCount = 0;
    mTimerFpsUpdate->start();
}

//////////////////////////////////////////////////////////////////////////