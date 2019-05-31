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
#include "taskproxymodel.h"

#include "task.h"
#include "taskmodel.h"

TaskProxyModel::TaskProxyModel(QObject* parent)
: QIdentityProxyModel(parent)
, mIdleIcon(":/idle.svg")
, mRunningIcon(":/busy.svg")
, mErrorIcon(":/error.svg")
{
}

QVariant TaskProxyModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DecorationRole) {
        Task::Status status = QIdentityProxyModel::data(index, TaskModel::StatusRole).value<Task::Status>();
        QIcon icon;
        switch (status) {
        case Task::Idle:
            icon = mIdleIcon;
            break;
        case Task::Running:
            icon = mRunningIcon;
            break;
        case Task::Error:
            icon = mErrorIcon;
            break;
        }
        return icon;
    } else {
        return QIdentityProxyModel::data(index, role);
    }
}
