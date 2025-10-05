#include "Editor.h"

#include "Global.h"

#include "Logger.h"
#include <QtCore/QDir>
#include <QtCore/QSysInfo>
#include <QtWidgets/QApplication>

#include <QSurfaceFormat>


///////////////////////////////////////////////////////////////////////////

using namespace ::SCME;

///////////////////////////////////////////////////////////////////////////

static bool testArg(const QString& arg)
{
    return arg.startsWith("-" + arg, Qt::CaseInsensitive);
}

///////////////////////////////////////////////////////////////////////////

static bool testArg(const QString& arg, QString& outArgValue)
{
    if (testArg(arg))
    {
        int split = arg.indexOf('=');

        if (split >= 0)
        {
            outArgValue = arg.mid(split + 1);
        }

        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("SCME");
    a.setApplicationVersion(applicationVersionString());
    a.setWindowIcon(QIcon(":/ui/scme_icon_256.png"));

    Logger::cleanupLogDir(5 * 1024 * 1024); //5 MB max total logs
    Logger::initLogger(Logger::defaultLogFile());

    LogInfo() << "========================";

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
            LogWarn() << "Unknown argument:" << arg;
        }
        else
        {
            if (parsedArgs.filename.isEmpty())
            {
                parsedArgs.filename = arg;
            }
            else
            {
                LogWarn() << "Cannot specify multiple arguments. Ignored:" << arg;
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

    LogInfo() << "Starting" << a.applicationName() << a.applicationVersion() << "...";
    LogInfo() << "args:" << args;
    LogInfo() << QSysInfo::prettyProductName() << QSysInfo::currentCpuArchitecture();

    Editor w(parsedArgs.filename);
    w.showMaximized();

    /// Parse arguments
    a.arguments();

    int ret = a.exec();

    LogInfo() << "Closed" << a.applicationName() << a.applicationVersion();
    LogInfo() << "------------------------";

    return ret;
}
