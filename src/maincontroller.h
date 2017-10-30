#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>
#include <QSet>
#include <QTimer>

#include "task.h"

class MainController : public QObject
{
    Q_OBJECT
public:
    explicit MainController(const QList<Task>& tasks);

private:
    void setupTimer();
    void setupTray();
    void updateTray();
    void run();
    void onFinished(QProcess* process, int exitCode);

    QTimer* mTimer;
    QList<Task> mTasks;
    QSet<QProcess*> mProcesses;
};

#endif /* MAINCONTROLLER_H */
