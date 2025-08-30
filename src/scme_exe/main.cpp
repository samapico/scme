#include "Editor.h"

#include "Global.h"
#include <QtCore/QDir>
#include <QtWidgets/QApplication>

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

    Editor w(parsedArgs.filename);
    w.show();

    /// Parse arguments
    a.arguments();

    return a.exec();
}
