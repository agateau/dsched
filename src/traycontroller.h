#ifndef TRAYCONTROLLER_H
#define TRAYCONTROLLER_H

#include <QIcon>
#include <QMenu>
#include <QObject>
#include <QPointer>
#include <QScopedPointer>

class MainWindow;
class TaskModel;

class QSystemTrayIcon;

class TrayController : public QObject
{
    Q_OBJECT
public:
    TrayController(TaskModel* taskModel);

private:
    void setupTray();
    void updateTray();
    void toggleWindow();
    void showWindow();

    TaskModel* mTaskModel;
    QSystemTrayIcon* mTray;
    QScopedPointer<QMenu> mMenu;
    QIcon mIdleIcon;
    QIcon mBusyIcon;
    QPointer<MainWindow> mWindow;
};

#endif /* TRAYCONTROLLER_H */
