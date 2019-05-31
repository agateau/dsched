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
#include "logger.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMutex>
#include <QScopedPointer>

#include <iostream>

using namespace std;

namespace Logger {

static const char LOG_MESSAGE_PATTERN[] = "%{time} %{type}: %{message}";

class LogInfo
{
public:
    void setup(const QString& logPath, Mode mode)
    {
        qInstallMessageHandler(LogInfo::handler);
        qSetMessagePattern(LOG_MESSAGE_PATTERN);
        mMode = mode;
        if (logPath.isEmpty() || logPath == "-") {
            return;
        }
        QString logDirName = QFileInfo(logPath).canonicalPath();
        if (!QDir(logDirName).mkpath(".")) {
            qWarning() << "Could not create dir" << logDirName;
            return;
        }
        mLogFile.reset(new QFile(logPath));
        if (!mLogFile->open(QIODevice::Append)) {
            qWarning() << "Could not open" << logPath;
            mLogFile.reset();
            return;
        }
    }

    static void handler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

private:
    QMutex mMutex;
    Mode mMode;
    QScopedPointer<QFile> mLogFile;

    void log(QtMsgType type, const QMessageLogContext& context, const QString& msg)
    {
        QString message = qFormatLogMessage(type, context, msg) + "\n";
        QMutexLocker lock(&mMutex);
        if (mLogFile) {
            mLogFile->write(qPrintable(message));
            mLogFile->flush();
        }
        cerr << qPrintable(message);
    }
};

Q_GLOBAL_STATIC(LogInfo, logInfo);

void setup(const QString& logPath, Mode mode)
{
    logInfo->setup(logPath, mode);
}

void LogInfo::handler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    logInfo->log(type, context, msg);
}

} // namespace
