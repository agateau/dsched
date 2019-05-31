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
#ifndef TRAYCONTROLLER_H
#define TRAYCONTROLLER_H

#include <QIcon>
#include <QMenu>
#include <QObject>
#include <QPointer>
#include <QScopedPointer>

class MainWindow;
class TaskModel;

class QSystemTrayIcon;

class TrayController : public QObject
{
    Q_OBJECT
public:
    TrayController(TaskModel* taskModel);

private:
    void setupTray();
    void updateTray();
    void toggleWindow();
    void showWindow();

    TaskModel* mTaskModel;
    QSystemTrayIcon* mTray;
    QScopedPointer<QMenu> mMenu;
    QIcon mIdleIcon;
    QIcon mBusyIcon;
    QIcon mErrorIcon;
    QPointer<MainWindow> mWindow;
};

#endif /* TRAYCONTROLLER_H */
