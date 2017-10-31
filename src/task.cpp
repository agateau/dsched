#include "task.h"

#include <QDebug>
#include <QProcess>

QProcess* Task::run()
{
    qInfo() << "Starting" << name;
    QProcess* process = new QProcess();
    process->start("/bin/sh", {"-c", command}, QIODevice::ReadOnly);
    mLastRun = QDateTime::currentDateTime();
    return process;
}

bool Task::canRun() const
{
    if (requires.isNull()) {
        return true;
    }
    qInfo() << "Running requirement check:" << requires;
    int exitCode = QProcess::execute("/bin/sh", {"-c", requires});
    bool ok = exitCode == 0;
    qInfo() << "Exit code:" << exitCode << "=>" << (ok ? "Yes" : "No");
    return ok;
}

QDateTime Task::nextRun() const
{
    if (mLastRun.isNull()) {
        return QDateTime();
    }
    return mLastRun.addSecs(interval.count());
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
