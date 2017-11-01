#include "tasktools.h"

#include <QDebug>

#include <chrono>

#include "iniparser.h"

using namespace std::chrono;

static const char TASK_SECTION_PREFIX[] = "task ";

namespace TaskTools
{

seconds parseInterval(const QString& txt, QString* error)
{
    Q_ASSERT(error);
    const QHash<QChar, int> DURATION_FOR_UNIT = {
        {'s', 1},
        {'m', 60},
        {'h', 60 * 60},
        {'d', 60 * 60 * 24}
    };

    QStringList tokens = txt.split(' ');
    if (tokens.empty()) {
        *error = "Interval is empty";
        return {};
    }

    seconds interval(0);
    for (const auto& token : tokens) {
        QChar unit = token[token.length() - 1];
        bool ok;
        int value = token.left(token.length() - 1).toInt(&ok);
        if (!ok) {
            *error = QString("Failed to parse %1").arg(token);
            return {};
        }
        auto it = DURATION_FOR_UNIT.find(unit);
        if (it == DURATION_FOR_UNIT.constEnd()) {
            *error = QString("Invalid unit '%1'").arg(unit);
            return {};
        }
        interval += value * seconds(it.value());
    }
    return interval;
}

QList<TaskPtr> load(const QString& path, QString* error)
{
    Q_ASSERT(error);

    IniParser parser;
    bool ok = parser.load(path, error);
    if (!ok) {
        *error = QString("Parse error: ") + *error;
        return {};
    }

    QList<TaskPtr> tasks;
    for (const QString& section : parser.sectionNames()) {
        if (!section.startsWith(TASK_SECTION_PREFIX)) {
            continue;
        }
        TaskPtr task(new Task);
        task->name = section.mid(qstrlen(TASK_SECTION_PREFIX));
        task->command = parser.get(section, "command");
        task->requires = parser.get(section, "requires");
        task->interval = parseInterval(parser.get(section, "interval"), error);
        if (!error->isEmpty()) {
            *error = QString("Error in interval of task %1: %2").arg(task->name).arg(*error);
            return {};
        }
        tasks << task;
    }
    return tasks;
}

} // namespace
