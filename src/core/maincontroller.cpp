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
#include "maincontroller.h"

#include <QDateTime>
#include <QDebug>
#include <QTimer>

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
