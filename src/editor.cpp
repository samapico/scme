#include "editor.h"
#include "glWidget.h"
#include "appver.h"

//////////////////////////////////////////////////////////////////////////

Editor::Editor(QWidget *parent, Qt::WFlags flags) :
    QMainWindow(parent, flags)
{
    ui.setupUi(this);

    setCentralWidget(new GLWidget(this));

    setWindowTitle(tr("SCME v%1.%2.%3.%4%5").arg(
        QString::number(APP_VERSION_MAJOR),
        QString::number(APP_VERSION_MINOR),
        QString::number(APP_VERSION_REV),
        QString::number(APP_VERSION_BUILD),
#ifdef _DEBUG
        tr(" (DEBUG)")
#else
        QString()
#endif
        ));
}

//////////////////////////////////////////////////////////////////////////

Editor::~Editor()
{

}
