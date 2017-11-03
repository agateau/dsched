#include "maincontroller.h"

#include <QColor>
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QMenu>
#include <QPainter>
#include <QPixmap>
#include <QProcess>
#include <QTimer>

#include "mainwindow.h"
#include "taskmodel.h"

static const int TIMER_INTERVAL = 30 * 1000;

MainController::MainController(const QList<TaskPtr>& tasks)
: mTimer(new QTimer(this))
, mTray(new QSystemTrayIcon(this))
, mMenu(new QMenu)
, mTaskModel(new TaskModel(this))
{
    mTaskModel->setTasks(tasks);
    setupTimer();
    setupTray();
    QTimer::singleShot(0, this, &MainController::run);
    mTray->show();
    mTimer->start();
}

void MainController::setupTimer()
{
    mTimer->setInterval(TIMER_INTERVAL);
    mTimer->setSingleShot(false);
    connect(mTimer, &QTimer::timeout, this, &MainController::run);
}

void MainController::setupTray()
{
    mIdleIcon = QIcon(QPixmap(":/idle.svg"));
    mBusyIcon = QIcon(QPixmap(":/busy.svg"));

    mMenu->addAction(tr("&Show"), this, &MainController::showWindow);
    mMenu->addAction(tr("&Quit"), QCoreApplication::instance(), &QCoreApplication::exit);
    mTray->setContextMenu(mMenu.data());

    connect(mTaskModel, &TaskModel::runningTasksChanged, this, &MainController::updateTray);

    connect(mTray, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger) {
            toggleWindow();
        }
    });

    updateTray();
}

void MainController::updateTray()
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

void MainController::run()
{
    QDateTime now = QDateTime::currentDateTime();
    for (auto& task : mTaskModel->tasks()) {
        QDateTime nextRun = task->nextRun();
        if ((nextRun.isNull() || nextRun <= now) && task->canRun()) {
            task->run();
        }
    }
}

void MainController::toggleWindow()
{
    if (mWindow) {
        mWindow->close();
    } else {
        showWindow();
    }
}

void MainController::showWindow()
{
    if (mWindow) {
        mWindow->raise();
        return;
    }
    mWindow = new MainWindow(mTaskModel);
    mWindow->show();
}
