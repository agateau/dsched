#include "tasklogfile.h"

#include <QFile>

static constexpr qint64 MAX_FILE_SIZE = 1024 * 1024;
static constexpr char ROTATE_SUFFIX[] = ".1";

TaskLogFile::TaskLogFile(const QString& path)
    : mPath(path)
{
    openFile();
}

TaskLogFile::~TaskLogFile() = default;

void TaskLogFile::write(const QByteArray& data)
{
    rotate();
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

void TaskLogFile::openFile()
{
    auto file = std::make_unique<QFile>(mPath);
    if (!file->open(QIODevice::Append)) {
        qCritical("Cannot create log file \"%s\", command output won't be logged", qPrintable(mPath));
        return;
    }
    mFile = std::move(file);
}

void TaskLogFile::rotate()
{
    if (!mFile) {
        return;
    }
    if (mFile->size() < MAX_FILE_SIZE) {
        return;
    }
    QString rotatedFile = mPath + ROTATE_SUFFIX;
    QFile::remove(rotatedFile);
    if (!QFile::rename(mPath, rotatedFile)) {
        qWarning("Failed to rotate log file \"%s\", continuing with existing log file", qPrintable(mPath));
        return;
    }
    openFile();
}
