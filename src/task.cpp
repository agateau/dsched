#include "task.h"

#include <QDebug>
#include <QProcess>

void Task::run()
{
    Q_ASSERT(!mProcess);
    qInfo("Starting \"%s\"", qPrintable(name));

    mProcess = new QProcess();
    QObject::connect(mProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                     this, &Task::onFinished);
    QObject::connect(mProcess, &QObject::destroyed, this, [this] {
            runningChanged(false);
    });

    mLastRun = QDateTime::currentDateTime();
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

void Task::onFinished(int exitCode)
{
    qInfo() << name << "finished with code" << exitCode;
    mProcess->deleteLater();
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
