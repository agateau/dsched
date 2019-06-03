#ifndef TASKLOGFILE_H
#define TASKLOGFILE_H

#include <QString>

#include <memory>

class QFile;

class TaskLogFile
{
public:
    TaskLogFile(const QString& path);
    ~TaskLogFile();

    void write(const QByteArray& data);

    QByteArray read() const;

private:
    const QString mPath;
    std::unique_ptr<QFile> mFile;
};

#endif // TASKLOGFILE_H
