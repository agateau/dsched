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

static const QChar MOON_CHAR(0x263E);
static const QChar GEAR_CHAR(0x2699);

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

static QIcon createTrayIcon(const QChar& ch, const QColor& bgColor, const QColor& fgColor)
{
    QPixmap pix(22, 22);
    {
        pix.fill(Qt::transparent);
        QPainter painter(&pix);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(bgColor);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(pix.rect().adjusted(0, 0, -1, -1));
        painter.setPen(fgColor);
        painter.drawText(pix.rect(), Qt::AlignCenter, ch);
    }
    return QIcon(pix);
}

void MainController::setupTray()
{
    mIdleIcon = createTrayIcon(MOON_CHAR, "#667", "white");
    mBusyIcon = createTrayIcon(GEAR_CHAR, "#966", "white");

    mMenu->addAction(tr("&Show"), this, &MainController::showWindow);
    mMenu->addAction(tr("&Quit"), QCoreApplication::instance(), &QCoreApplication::exit);
    mTray->setContextMenu(mMenu.data());

    connect(mTray, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger) {
            showWindow();
        }
    });

    updateTray();
}

void MainController::updateTray()
{
    if (mProcesses.empty()) {
        mTray->setIcon(mIdleIcon);
        mTray->setToolTip(tr("Idle"));
        return;
    }
    QStringList lst;
    for (const auto* process : mProcesses) {
        lst << QString("• %1").arg(process->arguments()[1]);
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
            QProcess* process = task->run();
            connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                    this, [this, process](int exitCode, QProcess::ExitStatus exitStatus) {
                        onFinished(process, exitCode);
            });
            mProcesses.insert(process);
        }
    }
    updateTray();
}

void MainController::onFinished(QProcess* process, int exitCode)
{
    QString cmd = process->arguments()[1];
    qInfo() << cmd << "finished with code" << exitCode;
    mProcesses.remove(process);
    updateTray();
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
