#include "traycontroller.h"

#include <QCoreApplication>
#include <QDebug>
#include <QMenu>
#include <QSystemTrayIcon>

#include "mainwindow.h"
#include "taskmodel.h"

TrayController::TrayController(TaskModel* taskModel)
: mTaskModel(taskModel)
, mTray(new QSystemTrayIcon(this))
, mMenu(new QMenu)
{
    setupTray();
    mTray->show();
}

void TrayController::setupTray()
{
    mIdleIcon = QIcon(QPixmap(":/idle.svg"));
    mBusyIcon = QIcon(QPixmap(":/busy.svg"));

    mMenu->addAction(tr("&Show"), this, &TrayController::showWindow);
    mMenu->addAction(tr("&Quit"), QCoreApplication::instance(), &QCoreApplication::exit);
    mTray->setContextMenu(mMenu.data());

    connect(mTaskModel, &TaskModel::runningTasksChanged, this, &TrayController::updateTray);

    connect(mTray, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger) {
            toggleWindow();
        }
    });

    updateTray();
}

void TrayController::updateTray()
{
    auto tasks = mTaskModel->runningTasks();
    if (tasks.empty()) {
        mTray->setIcon(mIdleIcon);
        mTray->setToolTip(tr("Idle"));
        return;
    }
    QStringList lst;
    for (const auto& task : tasks) {
        lst << QString("• %1").arg(task->name);
    }
    mTray->setIcon(mBusyIcon);
    mTray->setToolTip(lst.join('\n'));
}


void TrayController::toggleWindow()
{
    if (mWindow) {
        mWindow->close();
    } else {
        showWindow();
    }
}

void TrayController::showWindow()
{
    if (mWindow) {
        mWindow->raise();
        return;
    }
    mWindow = new MainWindow(mTaskModel);
    mWindow->show();
}
