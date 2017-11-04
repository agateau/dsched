#include "task.h"

#include <QDebug>
#include <QDir>
#include <QProcess>

using namespace std::chrono;

QString Task::name() const
{
    return mName;
}

void Task::setName(const QString& name)
{
    Q_ASSERT(mName.isEmpty());
    mName = name;
}

QString Task::command() const
{
    return mCommand;
}

void Task::setCommand(const QString& command)
{
    Q_ASSERT(mCommand.isEmpty());
    mCommand = command;
}

seconds Task::interval() const
{
    return mInterval;
}

void Task::setInterval(const seconds& interval)
{
    mInterval = interval;
}

QString Task::requires() const
{
    return mRequires;
}

void Task::setRequires(const QString& requires)
{
    Q_ASSERT(mRequires.isEmpty());
    mRequires = requires;
}

void Task::setTasksLogDirName(const QString& dirName)
{
    if (!QDir(dirName).mkpath(".")) {
        qCritical("Cannot create dir \"%s\", command output won't be logged", qPrintable(dirName));
        return;
    }

    QString path = dirName + "/" + mName + ".log";
    QScopedPointer<QFile> logFile(new QFile(path));
    if (!logFile->open(QIODevice::Append)) {
        qCritical("Cannot create log file \"%s\", command output won't be logged", qPrintable(path));
        return;
    }
    mLogFile.reset(logFile.take());
}

void Task::run()
{
    if (mProcess) {
        qInfo("Not starting \"%s\", it is already running", qPrintable(mName));
        return;
    }
    qInfo("Starting \"%s\"", qPrintable(mName));

    mProcess = new QProcess();
    mProcess->setProcessChannelMode(QProcess::MergedChannels);

    connect(mProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, &Task::onFinished);

    connect(mProcess, &QProcess::readyReadStandardOutput, this, &Task::readProcessOutput);

    mLastRun = QDateTime::currentDateTime();
    writeTitleLog(QString("Starting %1").arg(mCommand));
    mProcess->start("/bin/sh", {"-c", mCommand}, QIODevice::ReadOnly);
    statusChanged(status());
}

bool Task::canRun() const
{
    if (mRequires.isNull()) {
        return true;
    }
    qInfo("Running requirement check \"%s\"", qPrintable(mRequires));
    int exitCode = QProcess::execute("/bin/sh", {"-c", mRequires});
    bool ok = exitCode == 0;
    qInfo("Exit code: %d => %s", exitCode, ok ? "Yes" : "No");
    return ok;
}

QDateTime Task::lastRun() const
{
    return mLastRun;
}

QDateTime Task::nextRun() const
{
    if (mLastRun.isNull()) {
        return QDateTime();
    }
    return mLastRun.addSecs(mInterval.count());
}

Task::Status Task::status() const
{
    if (mProcess) {
        return Running;
    }
    return mExitCode == 0 ? Idle : Error;
}

QString Task::logFilePath() const
{
    return mLogFile ? mLogFile->fileName() : QString();
}

void Task::onFinished(int exitCode)
{
    qInfo() << mName << "finished with code" << exitCode;
    mExitCode = exitCode;
    mProcess->deleteLater();
    mProcess = nullptr;
    writeTitleLog(QString("Finished with code %1").arg(exitCode));
    statusChanged(status());
}

void Task::readProcessOutput()
{
    Q_ASSERT(mProcess);
    QByteArray output = mProcess->readAllStandardOutput();
    writeLog(output);
}

void Task::writeTitleLog(const QString& message)
{
    QString timeStamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    QString msg = QString("-- %1: %2 --\n").arg(timeStamp).arg(message);
    writeLog(msg.toUtf8());
}

void Task::writeLog(const QByteArray& data)
{
    if (mLogFile) {
        mLogFile->write(data);
        mLogFile->flush();
    }
    taskLogged(data);
}

std::ostream& operator<<(std::ostream& ostr, const Task& task)
{
    ostr << qPrintable(task.name()) << "\n";
    ostr << "- command: " << qPrintable(task.command()) << "\n";
    ostr << "- interval: " << task.interval().count() << "s\n";
    if (!task.requires().isEmpty()) {
        ostr << "- requires: " << qPrintable(task.requires()) << "\n";
    }
    return ostr;
}
