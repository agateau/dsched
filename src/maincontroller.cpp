#include "maincontroller.h"

#include <QDateTime>
#include <QDebug>
#include <QProcess>
#include <QTimer>

static const int TIMER_INTERVAL = 30 * 1000;

MainController::MainController(const QList<Task>& tasks)
: mTimer(new QTimer(this))
, mTasks(tasks)
{
    setupTimer();
    setupTray();
    QTimer::singleShot(0, this, &MainController::run);
}

void MainController::setupTimer()
{
    mTimer->setInterval(TIMER_INTERVAL);
    mTimer->setSingleShot(false);
    connect(mTimer, &QTimer::timeout, this, &MainController::run);
}

void MainController::setupTray()
{
    qDebug() << "TODO setupTray";
}

void MainController::updateTray()
{
    qDebug() << "TODO updateTray";
}

void MainController::run()
{
    QDateTime now = QDateTime::currentDateTime();
    for (Task& task : mTasks) {
        QDateTime nextRun = task.nextRun();
        if ((nextRun.isNull() || nextRun <= now) && task.canRun()) {
            QProcess* process = task.run();
            connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                    this, [this, process](int exitCode, QProcess::ExitStatus exitStatus) {
                        onFinished(process, exitCode);
            });
            mProcesses.insert(process);
        }
    }
}

void MainController::onFinished(QProcess* process, int exitCode)
{
    QString cmd = process->arguments()[1];
    qInfo() << cmd << "finished with code" << exitCode;
    mProcesses.remove(process);
    updateTray();
}
