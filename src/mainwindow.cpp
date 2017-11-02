#include "mainwindow.h"

#include <QDebug>
#include <QFile>

#include "taskmodel.h"

MainWindow::MainWindow(TaskModel* model)
: mUi(new Ui_MainWindow)
, mModel(model)
{
    setAttribute(Qt::WA_DeleteOnClose);
    mUi->setupUi(this);

    mUi->listView->setModel(model);
    connect(mUi->listView, &QAbstractItemView::clicked, this, &MainWindow::onTaskSelected);
    connect(mUi->startButton, &QPushButton::clicked, this, &MainWindow::startTask);

    mUi->stackedWidget->setCurrentWidget(mUi->welcomePage);
}

void MainWindow::onTaskSelected(const QModelIndex& index)
{
    int row = index.row();
    TaskPtr task;
    if (row >= 0) {
        task = mModel->tasks().at(row);
    }
    setCurrentTask(task);
}

void MainWindow::setCurrentTask(const TaskPtr& task)
{
    if (mCurrentTask == task) {
        return;
    }
    if (mCurrentTask) {
        disconnect(mCurrentTask.data(), nullptr, this, nullptr);
    }
    mCurrentTask = task;

    if (!mCurrentTask) {
        mUi->stackedWidget->setCurrentWidget(mUi->welcomePage);
        return;
    }
    mUi->stackedWidget->setCurrentWidget(mUi->detailPage);

    QString logFilePath = task->logFilePath();
    if (!logFilePath.isEmpty()) {
        QFile file(logFilePath);
        file.open(QIODevice::ReadOnly);
        QByteArray log = file.readAll();
        mUi->logTextEdit->clear();
        mUi->logTextEdit->appendPlainText(log);
    }
    connect(mCurrentTask.data(), &Task::runningChanged, this, &MainWindow::updateTaskView);
    connect(mCurrentTask.data(), &Task::taskLogged, this, &MainWindow::appendToTaskLog);
    updateTaskView();
}

void MainWindow::updateTaskView()
{
    if (!mCurrentTask) {
        return;
    }
    QString status = mCurrentTask->isRunning() ? tr("Running") : tr("Not running");
    QString lastRun = mCurrentTask->lastRun().isNull() ? tr("Never") : mCurrentTask->lastRun().toString();

    mUi->statusLabel->setText(status);
    mUi->lastRunLabel->setText(lastRun);
    mUi->startButton->setEnabled(!mCurrentTask->isRunning());
}

void MainWindow::appendToTaskLog(const QByteArray& data)
{
    mUi->logTextEdit->appendPlainText(QString::fromUtf8(data));
}

void MainWindow::startTask()
{
    Q_ASSERT(mCurrentTask);
    Q_ASSERT(!mCurrentTask->isRunning());
    mCurrentTask->run();
}
