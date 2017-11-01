#include "task.h"

#include <QDebug>
#include <QProcess>

QProcess* Task::run()
{
    Q_ASSERT(!mProcess);
    qInfo("Starting \"%s\"", qPrintable(name));

    mProcess = new QProcess();
    QObject::connect(mProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                     mProcess, &QObject::deleteLater);

    mLastRun = QDateTime::currentDateTime();
    mProcess->start("/bin/sh", {"-c", command}, QIODevice::ReadOnly);
    return mProcess;
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
