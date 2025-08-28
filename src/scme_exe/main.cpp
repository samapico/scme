#include "Editor.h"

#include <QtCore/QDir>
#include <QtWidgets/QApplication>

///////////////////////////////////////////////////////////////////////////

using namespace ::SCME;

///////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Editor w;
    w.show();
    return a.exec();
}
