#include "maincontroller.h"

#include <QDateTime>
#include <QDebug>
#include <QTimer>

#include "mainwindow.h"
#include "taskmodel.h"

static const int TIMER_INTERVAL = 30 * 1000;

MainController::MainController(TaskModel* taskModel)
: mTaskModel(taskModel)
, mTimer(new QTimer(this))
{
    setupTimer();
    QTimer::singleShot(0, this, &MainController::run);
    mTimer->start();
}

void MainController::setupTimer()
{
    mTimer->setInterval(TIMER_INTERVAL);
    mTimer->setSingleShot(false);
    connect(mTimer, &QTimer::timeout, this, &MainController::run);
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
