#ifndef TASK_H
#define TASK_H

#include <QDateTime>
#include <QObject>
#include <QSharedData>
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

    void run();
    bool isRunning() const;

    bool canRun() const;
    QDateTime nextRun() const;
    QDateTime lastRun() const;

Q_SIGNALS:
    void runningChanged(bool running);

private:
    void onFinished(int exitCode);
    QDateTime mLastRun;
    QProcess* mProcess = nullptr;
};

using TaskPtr = QExplicitlySharedDataPointer<Task>;

std::ostream& operator<<(std::ostream& ostr, const Task& task);

#endif /* TASK_H */
