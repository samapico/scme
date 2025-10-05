#include "Logger.h"

#include <QtCore/QDir>
#include <QtCore/QDateTime>
#include <QtCore/QStandardPaths>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QApplication>


///////////////////////////////////////////////////////////////////////////

using namespace ::SCME;


//////////////////////////////////////////////////////////////////////////

std::unique_ptr<Logger> Logger::sGlobalLogger;


//////////////////////////////////////////////////////////////////////////

QString Logger::defaultLogDir()
{
    return QDir(QStandardPaths::writableLocation(QStandardPaths::StandardLocation::AppLocalDataLocation)).filePath("logs");
}

//////////////////////////////////////////////////////////////////////////

QString Logger::defaultLogFile()
{
    return QDir(defaultLogDir()).filePath("log_" + QDate::currentDate().toString("yyyy-MM-dd") + ".log");
}

//////////////////////////////////////////////////////////////////////////

void Logger::cleanupLogDir(int64_t maxBytes)
{
    QDir logDir(defaultLogDir());

    //List all previous logs, sorted by descending name, which corresponds to newest first
    QFileInfoList previousLogs = logDir.entryInfoList(QDir::Files, QDir::SortFlag::Name | QDir::SortFlag::Reversed);

    for (const QFileInfo& fi : previousLogs)
    {
        maxBytes -= fi.size();

        if (maxBytes < 0)
        {
            qDebug() << "Removing log file" << fi.filePath() << fi.size() << maxBytes;
            if (!QFile::remove(fi.filePath()))
                qWarning() << "Failed to remove log file";
        }
        else
        {
            qDebug() << "Keeping log file" << fi.filePath() << fi.size() << maxBytes;
        }
    }
}


//////////////////////////////////////////////////////////////////////////

void Logger::fMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    Q_ASSERT(sGlobalLogger);
    if (sGlobalLogger)
        sGlobalLogger->log(type, context, msg);
}

//////////////////////////////////////////////////////////////////////////

void Logger::initLogger(const QString& logFilepath)
{
    sGlobalLogger = std::make_unique<Logger>(logFilepath);

    qInstallMessageHandler(Logger::fMessageHandler);
}

//////////////////////////////////////////////////////////////////////////

QString Logger::currentLogFilepath()
{
    if (sGlobalLogger && sGlobalLogger->mLogFile)
        return sGlobalLogger->mLogFile->fileName();

    return QString();
}


//////////////////////////////////////////////////////////////////////////

Logger::Logger(const QString& logFilepath)
{
    //Make sure the directory exists
    QDir logDir(QFileInfo(logFilepath).path());
    if (!logDir.exists())
        logDir.mkpath(".");

    mLogFile = std::make_unique<QFile>(logFilepath);
}

//////////////////////////////////////////////////////////////////////////

Logger::~Logger()
{
}


//////////////////////////////////////////////////////////////////////////

void Logger::log(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    Q_ASSERT(mLogFile);

    if (!mLogFile->isOpen())
    {
        if (!mLogFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        {
            //Cannot open output file
            QString strError = mLogFile->errorString();
            QString outFile = mLogFile->fileName();

            //Avoid an error on each subsequent message
            qInstallMessageHandler(nullptr);
            mLogFile.reset();

            QMessageBox::warning(nullptr, QObject::tr("Error"), QObject::tr("Cannot open log file: %1 (%2)").arg(outFile, strError));
            return;
        }
    }

    if (!mLogStream)
        mLogStream = std::make_unique<QTextStream>(mLogFile.get());

    (*mLogStream) << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") << " - " << msg << '\n';
    mLogStream->flush();
}