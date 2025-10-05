#ifndef INC_Logger_H
#define INC_Logger_H

#include <QtCore/QString>
#include <QtCore/QtLogging>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include <memory>


///////////////////////////////////////////////////////////////////////////

namespace SCME {

    class Logger
    {
    public:

        static QString defaultLogDir();

        static QString defaultLogFile();

        static void cleanupLogDir(int64_t maxBytes);

        static void initLogger(const QString& logFilepath);

        static QString currentLogFilepath();

        explicit Logger(const QString& logFilepath);

        ~Logger();

        void log(QtMsgType, const QMessageLogContext&, const QString&);

    protected:

        static std::unique_ptr<Logger> sGlobalLogger;

        static void fMessageHandler(QtMsgType, const QMessageLogContext&, const QString&);


        std::unique_ptr<QFile> mLogFile;
        std::unique_ptr<QTextStream> mLogStream;
    };


///////////////////////////////////////////////////////////////////////////

} // End namespace SCME

//////////////////////////////////////////////////////////////////////////

#endif // INC_Logger_H
