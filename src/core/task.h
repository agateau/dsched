/*
 * Copyright 2017 Aurélien Gâteau <mail@agateau.com>
 *
 * This file is part of Dsched.
 *
 * Dsched is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef TASK_H
#define TASK_H

#include "tasklogfile.h"

#include <QFile>
#include <QDateTime>
#include <QObject>
#include <QScopedPointer>
#include <QSharedData>
#include <QString>

#include <chrono>
#include <iostream>
#include <memory>

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

    ~Task();

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

    QByteArray readLogFile() const;

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
    std::unique_ptr<TaskLogFile> mLogFile;
    QProcess* mProcess = nullptr;
    int mExitCode = 0;
};

using TaskPtr = QExplicitlySharedDataPointer<Task>;

std::ostream& operator<<(std::ostream& ostr, const Task& task);

#endif /* TASK_H */
