#include "mainwindow.h"

#include "taskmodel.h"

MainWindow::MainWindow(TaskModel* model)
: mUi(new Ui_MainWindow)
, mModel(model)
{
    mUi->setupUi(this);

    mUi->listView->setModel(model);
    connect(mUi->listView, &QAbstractItemView::clicked, this, &MainWindow::onTaskSelected);
}

void MainWindow::onTaskSelected(const QModelIndex& index)
{
    int row = index.row();
    if (row >= 0) {
        TaskPtr task = mModel->tasks().at(row);
        setCurrentTask(task);
    }
}

void MainWindow::setCurrentTask(const TaskPtr& task)
{
    QString status = task->isRunning() ? tr("Running") : tr("Not running");
    QString lastRun = task->lastRun().isNull() ? tr("Never") : task->lastRun().toString();

    mUi->statusLabel->setText(status);
    mUi->lastRunLabel->setText(lastRun);
}
