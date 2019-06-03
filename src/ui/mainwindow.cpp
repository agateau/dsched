/*
 * Copyright 2017 Aurélien Gâteau <mail@agateau.com>
 *
 * This file is part of Dsched.
 *
 * Dsched is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "mainwindow.h"

#include <QAction>
#include <QDebug>
#include <QItemSelectionModel>
#include <QKeySequence>

#include "taskmodel.h"
#include "taskproxymodel.h"
#include "loghighlighter.h"

MainWindow::MainWindow(TaskModel* model)
: mUi(new Ui_MainWindow)
, mModel(model)
{
    setAttribute(Qt::WA_DeleteOnClose);
    mUi->setupUi(this);

    TaskProxyModel* proxyModel = new TaskProxyModel(this);
    proxyModel->setSourceModel(mModel);
    mUi->listView->setModel(proxyModel);
    connect(mUi->listView->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::onCurrentChanged);
    connect(mUi->startButton, &QPushButton::clicked, this, &MainWindow::startTask);

    mUi->actionQuit->setShortcut(QKeySequence::Quit);
    connect(mUi->actionQuit, &QAction::triggered, QCoreApplication::instance(), &QCoreApplication::exit);

    mUi->stackedWidget->setCurrentWidget(mUi->welcomePage);

    new LogHighlighter(mUi->logTextEdit->document());
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

    mUi->logTextEdit->clear();
    mUi->logTextEdit->appendPlainText(task->readLogFile());

    connect(mCurrentTask.data(), &Task::statusChanged, this, &MainWindow::updateTaskView);
    connect(mCurrentTask.data(), &Task::taskLogged, this, &MainWindow::appendToTaskLog);
    updateTaskView();
}

void MainWindow::updateTaskView()
{
    if (!mCurrentTask) {
        return;
    }
    bool isRunning = mCurrentTask->status() == Task::Running;
    QString status = isRunning ? tr("Running") : tr("Not running");
    QString lastRun = mCurrentTask->lastRun().isNull() ? tr("Never") : mCurrentTask->lastRun().toString();

    mUi->statusLabel->setText(status);
    mUi->lastRunLabel->setText(lastRun);
    mUi->startButton->setEnabled(!isRunning);
}

void MainWindow::appendToTaskLog(const QByteArray& data)
{
    QString text = QString::fromUtf8(data);
    // appendPlainText adds a \n at the end, so remove it if it is already there
    if (text.endsWith("\n")) {
        text.chop(1);
    }
    mUi->logTextEdit->appendPlainText(text);
}

void MainWindow::startTask()
{
    Q_ASSERT(mCurrentTask);
    Q_ASSERT(mCurrentTask->status() != Task::Running);
    mCurrentTask->run();
}
