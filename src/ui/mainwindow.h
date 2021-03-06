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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScopedPointer>

#include "task.h"
#include "ui_mainwindow.h"

class TaskModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(TaskModel* model);

private:
    void onCurrentChanged(const QModelIndex& index);
    void setCurrentTask(const TaskPtr& task);
    void updateTaskView();
    void appendToTaskLog(const QByteArray& data);
    void startTask();

    QScopedPointer<Ui_MainWindow> mUi;
    TaskModel* mModel;

    TaskPtr mCurrentTask;
};

#endif // MAINWINDOW_H
