#ifndef TASK_H
#define TASK_H

#include <QString>

#include <chrono>
#include <iostream>

struct Task
{
    QString name;
    QString command;
    std::chrono::seconds interval;
    QString requires;
};

std::ostream& operator<<(std::ostream& ostr, const Task& task);

#endif /* TASK_H */
