#include "Editor.h"

#include "Global.h"
#include <QtCore/QDir>
#include <QtCore/QSysInfo>
#include <QtWidgets/QApplication>

#include <QSurfaceFormat>


///////////////////////////////////////////////////////////////////////////

using namespace ::SCME;

///////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("SCME");
    a.setApplicationVersion(applicationVersionString());
    a.setWindowIcon(QIcon(":/ui/scme_icon_256.png"));

    QStringList args = a.arguments();
    args.removeFirst(); //first arg is the executable itself

    struct
    {
        QString filename;
    } parsedArgs;

    for (const QString& arg : args)
    {
        if (arg.startsWith('-'))
        {
            //looks like a switch
            qWarning() << "Unknown argument:" << arg;
        }
        else
        {
            if (parsedArgs.filename.isEmpty())
            {
                parsedArgs.filename = arg;
            }
            else
            {
                qWarning() << "Cannot specify multiple arguments. Ignored:" << arg;
            }
        }
    }



    // Request core 3.3 for #version 330
    QSurfaceFormat fmt;
    fmt.setRenderableType(QSurfaceFormat::OpenGL);
    fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setDepthBufferSize(0);
    QSurfaceFormat::setDefaultFormat(fmt);

    qDebug().noquote() << "Starting" << a.applicationName() << a.applicationVersion() << "...";
    qDebug().noquote() << QSysInfo::prettyProductName() << QSysInfo::currentCpuArchitecture();

    Editor w(parsedArgs.filename);
    w.showMaximized();

    /// Parse arguments
    a.arguments();

    return a.exec();
}
