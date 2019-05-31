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
#ifndef TASKPROXYMODEL_H
#define TASKPROXYMODEL_H

#include <QIdentityProxyModel>
#include <QIcon>

/**
 * Wraps TaskModel, adds an icon in the DecorationRole
 */
class TaskProxyModel : public QIdentityProxyModel {
public:
    TaskProxyModel(QObject* parent);

    QVariant data(const QModelIndex& index, int role) const override;

private:
    QIcon mIdleIcon;
    QIcon mRunningIcon;
    QIcon mErrorIcon;
};


#endif /* TASKPROXYMODEL_H */
