#include "mainwindow.h"

#include <QAction>
#include <QDebug>
#include <QFile>
#include <QKeySequence>
#include <QItemSelectionModel>

#include "taskmodel.h"

MainWindow::MainWindow(TaskModel* model)
: mUi(new Ui_MainWindow)
, mModel(model)
{
    setAttribute(Qt::WA_DeleteOnClose);
    mUi->setupUi(this);

    mUi->listView->setModel(model);
    connect(mUi->listView->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::onCurrentChanged);
    connect(mUi->startButton, &QPushButton::clicked, this, &MainWindow::startTask);

    mUi->actionQuit->setShortcut(QKeySequence::Quit);
    connect(mUi->actionQuit, &QAction::triggered, QCoreApplication::instance(), &QCoreApplication::exit);

    mUi->stackedWidget->setCurrentWidget(mUi->welcomePage);
}

void MainWindow::onCurrentChanged(const QModelIndex& index)
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
    QString text = QString::fromUtf8(data);
    qDebug() << text;
    // appendPlainText adds a \n at the end, so remove it if it is already there
    if (text.endsWith("\n")) {
        text.chop(1);
    }
    mUi->logTextEdit->appendPlainText(text);
}

void MainWindow::startTask()
{
    Q_ASSERT(mCurrentTask);
    Q_ASSERT(!mCurrentTask->isRunning());
    mCurrentTask->run();
}
