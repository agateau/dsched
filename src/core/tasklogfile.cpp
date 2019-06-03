#include "tasklogfile.h"

#include <QFile>

TaskLogFile::TaskLogFile(const QString& path)
    : mPath(path)
{
    auto file = std::make_unique<QFile>(path);
    if (!file->open(QIODevice::Append)) {
        qCritical("Cannot create log file \"%s\", command output won't be logged", qPrintable(path));
        return;
    }
    mFile = std::move(file);
}

TaskLogFile::~TaskLogFile() = default;

void TaskLogFile::write(const QByteArray& data)
{
    mFile->write(data);
    mFile->flush();
}

QByteArray TaskLogFile::read() const
{
    if (!mFile) {
        return {};
    }
    QFile file(mPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Failed to open \"%s\": %s", qPrintable(mPath), qPrintable(file.errorString()));
        return {};
    }
    QByteArray log = file.readAll();
    log.replace('\0', '?');
    return log;
}
