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
#include "traycontroller.h"

#include <QCoreApplication>
#include <QDebug>
#include <QMenu>
#include <QSystemTrayIcon>

#include "mainwindow.h"
#include "taskmodel.h"

TrayController::TrayController(TaskModel* taskModel)
: mTaskModel(taskModel)
, mTray(new QSystemTrayIcon(this))
, mMenu(new QMenu)
{
    setupTray();
    mTray->show();
}

void TrayController::setupTray()
{
    mIdleIcon = QIcon(QPixmap(":/idle.svg"));
    mBusyIcon = QIcon(QPixmap(":/busy.svg"));
    mErrorIcon = QIcon(QPixmap(":/error.svg"));

    mMenu->addAction(tr("&Show"), this, &TrayController::showWindow);
    mMenu->addAction(tr("&Quit"), QCoreApplication::instance(), &QCoreApplication::exit);
    mTray->setContextMenu(mMenu.data());

    connect(mTaskModel, &TaskModel::statusChanged, this, &TrayController::updateTray);

    connect(mTray, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger) {
            toggleWindow();
        }
    });

    updateTray();
}

void TrayController::updateTray()
{
    auto generateToolTip = [this] {
        QStringList lst;
        for (const auto& task : mTaskModel->runningTasks()) {
            lst << QString("• %1").arg(task->name());
        }
        return lst.join('\n');
    };

    switch (mTaskModel->status()) {
    case Task::Idle:
        mTray->setIcon(mIdleIcon);
        mTray->setToolTip(tr("Idle"));
        return;
    case Task::Error:
        mTray->setIcon(mErrorIcon);
        mTray->setToolTip(tr("Error"));
        return;
    case Task::Running:
        mTray->setIcon(mBusyIcon);
        mTray->setToolTip(generateToolTip());
        return;
    }
}


void TrayController::toggleWindow()
{
    if (mWindow) {
        mWindow->close();
    } else {
        showWindow();
    }
}

void TrayController::showWindow()
{
    if (mWindow) {
        mWindow->raise();
        return;
    }
    mWindow = new MainWindow(mTaskModel);
    mWindow->show();
}
