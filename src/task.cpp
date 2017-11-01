#include "task.h"

#include <QDebug>
#include <QProcess>

void Task::run()
{
    if (mProcess) {
        qInfo("Not starting \"%s\", it is already running", qPrintable(name));
        return;
    }
    qInfo("Starting \"%s\"", qPrintable(name));

    mProcess = new QProcess();
    connect(mProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, &Task::onFinished);

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
    mProcess = nullptr;
    runningChanged(false);
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
