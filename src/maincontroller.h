#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QMenu>
#include <QIcon>
#include <QObject>
#include <QPointer>
#include <QScopedPointer>
#include <QSet>
#include <QSystemTrayIcon>
#include <QTimer>

#include "task.h"

class MainWindow;
class TaskModel;

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
    void showWindow();

    QTimer* mTimer;
    QSystemTrayIcon* mTray;
    QScopedPointer<QMenu> mMenu;
    QIcon mIdleIcon;
    QIcon mBusyIcon;
    QPointer<MainWindow> mWindow;
    TaskModel* mTaskModel;
    QSet<QProcess*> mProcesses;
};

#endif /* MAINCONTROLLER_H */
