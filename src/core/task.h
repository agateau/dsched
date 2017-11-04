#ifndef TASK_H
#define TASK_H

#include <QFile>
#include <QDateTime>
#include <QObject>
#include <QScopedPointer>
#include <QSharedData>
#include <QString>

#include <chrono>
#include <iostream>

class QProcess;

class Task : public QObject, public QSharedData
{
    Q_OBJECT
public:
    enum Status {
        Idle,
        Error,
        Running,
    };

    Q_ENUM(Status)

    QString name() const;
    void setName(const QString& name);

    QString command() const;
    void setCommand(const QString& command);

    std::chrono::seconds interval() const;
    void setInterval(const std::chrono::seconds& interval);

    QString requires() const;
    void setRequires(const QString& requires);

    void setTasksLogDirName(const QString& dirName);
    void run();

    bool canRun() const;
    QDateTime nextRun() const;
    QDateTime lastRun() const;
    Status status() const;

    QString logFilePath() const;

Q_SIGNALS:
    void statusChanged(Status status);
    void taskLogged(const QByteArray& data);

private:
    void onFinished(int exitCode);
    void readProcessOutput();
    void writeTitleLog(const QString& title);
    void writeLog(const QByteArray& data);

    QString mName;
    QString mCommand;
    std::chrono::seconds mInterval;
    QString mRequires;

    QDateTime mLastRun;
    QScopedPointer<QFile> mLogFile;
    QProcess* mProcess = nullptr;
    int mExitCode = 0;
};

using TaskPtr = QExplicitlySharedDataPointer<Task>;

std::ostream& operator<<(std::ostream& ostr, const Task& task);

#endif /* TASK_H */
