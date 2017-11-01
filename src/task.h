#ifndef TASK_H
#define TASK_H

#include <QDateTime>
#include <QSharedData>
#include <QPointer>
#include <QString>

#include <chrono>
#include <iostream>

class QProcess;

class Task : public QObject, public QSharedData
{
    Q_OBJECT
public:
    QString name;
    QString command;
    std::chrono::seconds interval;
    QString requires;

    QProcess* run();
    bool isRunning() const;

    bool canRun() const;
    QDateTime nextRun() const;
    QDateTime lastRun() const;

Q_SIGNALS:
    void runningChanged(bool running);

private:
    QDateTime mLastRun;
    QPointer<QProcess> mProcess;
};

using TaskPtr = QExplicitlySharedDataPointer<Task>;

std::ostream& operator<<(std::ostream& ostr, const Task& task);

#endif /* TASK_H */
