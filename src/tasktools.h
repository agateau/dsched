#ifndef TASKTOOLS_H
#define TASKTOOLS_H

#include <QList>

#include <chrono>

#include "task.h"

namespace TaskTools
{

QList<Task> load(const QString& path, QString* error);

std::chrono::seconds parseInterval(const QString& txt, QString* error);

} // namespace

#endif /* TASKTOOLS_H */
