#ifndef TASK_H
#define TASK_H

#include <QDateTime>
#include <QSharedData>
#include <QPointer>
#include <QString>

#include <chrono>
#include <iostream>

class QProcess;

struct Task : public QSharedData
{
    QString name;
    QString command;
    std::chrono::seconds interval;
    QString requires;

    QProcess* run();
    bool isRunning() const;

    bool canRun() const;
    QDateTime nextRun() const;
    QDateTime lastRun() const;

private:
    QDateTime mLastRun;
    QPointer<QProcess> mProcess;
};

using TaskPtr = QExplicitlySharedDataPointer<Task>;

std::ostream& operator<<(std::ostream& ostr, const Task& task);

#endif /* TASK_H */
