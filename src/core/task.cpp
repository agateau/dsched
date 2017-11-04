#include "task.h"

#include <QDebug>
#include <QDir>
#include <QProcess>

void Task::setTasksLogDirName(const QString& dirName)
{
    if (!QDir(dirName).mkpath(".")) {
        qCritical("Cannot create dir \"%s\", command output won't be logged", qPrintable(dirName));
        return;
    }

    QString path = dirName + "/" + name + ".log";
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
        qInfo("Not starting \"%s\", it is already running", qPrintable(name));
        return;
    }
    qInfo("Starting \"%s\"", qPrintable(name));

    mProcess = new QProcess();
    mProcess->setProcessChannelMode(QProcess::MergedChannels);

    connect(mProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, &Task::onFinished);

    connect(mProcess, &QProcess::readyReadStandardOutput, this, &Task::readProcessOutput);

    mLastRun = QDateTime::currentDateTime();
    writeTitleLog(QString("Starting %1").arg(command));
    mProcess->start("/bin/sh", {"-c", command}, QIODevice::ReadOnly);
    runningChanged(true);
}

bool Task::isRunning() const
{
    return mProcess;
}

bool Task::canRun() const
{
    if (requires.isNull()) {
        return true;
    }
    qInfo("Running requirement check \"%s\"", qPrintable(requires));
    int exitCode = QProcess::execute("/bin/sh", {"-c", requires});
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
    return mLastRun.addSecs(interval.count());
}

Task::Status Task::status() const
{
    if (isRunning()) {
        return Status::Running;
    }
    return mExitCode == 0 ? Status::Idle : Status::Error;
}

QString Task::logFilePath() const
{
    return mLogFile ? mLogFile->fileName() : QString();
}

void Task::onFinished(int exitCode)
{
    qInfo() << name << "finished with code" << exitCode;
    mExitCode = exitCode;
    mProcess->deleteLater();
    mProcess = nullptr;
    writeTitleLog(QString("Finished with code %1").arg(exitCode));
    runningChanged(false);
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
    ostr << qPrintable(task.name) << "\n";
    ostr << "- command: " << qPrintable(task.command) << "\n";
    ostr << "- interval: " << task.interval.count() << "s\n";
    if (!task.requires.isEmpty()) {
        ostr << "- requires: " << qPrintable(task.requires) << "\n";
    }
    return ostr;
}
