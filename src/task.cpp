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
    qDebug() << "TODO canRun";
    return true;
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
