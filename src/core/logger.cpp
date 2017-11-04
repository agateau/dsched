#include "logger.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMutex>
#include <QScopedPointer>

#include <iostream>

using namespace std;

namespace Logger {

static const char LOG_MESSAGE_PATTERN[] = "%{time} %{type}: %{message}";

class LogInfo
{
public:
    void setup(const QString& logPath, Mode mode)
    {
        qInstallMessageHandler(LogInfo::handler);
        qSetMessagePattern(LOG_MESSAGE_PATTERN);
        mMode = mode;
        if (logPath.isEmpty() || logPath == "-") {
            return;
        }
        QString logDirName = QFileInfo(logPath).canonicalPath();
        if (!QDir(logDirName).mkpath(".")) {
            qWarning() << "Could not create dir" << logDirName;
            return;
        }
        mLogFile.reset(new QFile(logPath));
        if (!mLogFile->open(QIODevice::Append)) {
            qWarning() << "Could not open" << logPath;
            mLogFile.reset();
            return;
        }
    }

    static void handler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

private:
    QMutex mMutex;
    Mode mMode;
    QScopedPointer<QFile> mLogFile;

    void log(QtMsgType type, const QMessageLogContext& context, const QString& msg)
    {
        QString message = qFormatLogMessage(type, context, msg) + "\n";
        QMutexLocker lock(&mMutex);
        if (mLogFile) {
            mLogFile->write(qPrintable(message));
            mLogFile->flush();
        }
        cerr << qPrintable(message);
    }
};

Q_GLOBAL_STATIC(LogInfo, logInfo);

void setup(const QString& logPath, Mode mode)
{
    logInfo->setup(logPath, mode);
}

void LogInfo::handler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    logInfo->log(type, context, msg);
}

} // namespace
