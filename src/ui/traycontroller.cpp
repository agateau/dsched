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
    mErrorIcon = QIcon(QPixmap(":/error.svg"));

    mMenu->addAction(tr("&Show"), this, &TrayController::showWindow);
    mMenu->addAction(tr("&Quit"), QCoreApplication::instance(), &QCoreApplication::exit);
    mTray->setContextMenu(mMenu.data());

    connect(mTaskModel, &TaskModel::statusChanged, this, &TrayController::updateTray);

    connect(mTray, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger) {
            toggleWindow();
        }
    });

    updateTray();
}

void TrayController::updateTray()
{
    auto generateToolTip = [this] {
        QStringList lst;
        for (const auto& task : mTaskModel->runningTasks()) {
            lst << QString("• %1").arg(task->name);
        }
        return lst.join('\n');
    };

    switch (mTaskModel->status()) {
    case Task::Status::Idle:
        mTray->setIcon(mIdleIcon);
        mTray->setToolTip(tr("Idle"));
        return;
    case Task::Status::Error:
        mTray->setIcon(mErrorIcon);
        mTray->setToolTip(tr("Error"));
        return;
    case Task::Status::Running:
        mTray->setIcon(mBusyIcon);
        mTray->setToolTip(generateToolTip());
        return;
    }
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
