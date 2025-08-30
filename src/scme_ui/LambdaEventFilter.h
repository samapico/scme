#ifndef INC_LambdaEventFilter_H
#define INC_LambdaEventFilter_H


#include "UiGlobal.h"

#include <QtCore/QObject>

#include <functional>


///////////////////////////////////////////////////////////////////////////

class QTimer;
class QOpenGLWidget;

///////////////////////////////////////////////////////////////////////////

namespace SCME {

//////////////////////////////////////////////////////////////////////////

class SCME_UI_DLL LambdaEventFilter : public QObject
{
public:
    using FilterFunc = std::function<bool(QObject*, QEvent*)>;

    explicit LambdaEventFilter(FilterFunc func, QObject* parent = nullptr);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    FilterFunc func;
};


///////////////////////////////////////////////////////////////////////////

} // End namespace SCME

///////////////////////////////////////////////////////////////////////////


#endif // INC_LambdaEventFilter_H
