#include "taskmodel.h"

#include <QDebug>

TaskModel::TaskModel(QObject* parent)
: QAbstractListModel(parent)
{
}

void TaskModel::setTasks(const QList<TaskPtr>& tasks)
{
    beginResetModel();
    mTasks = tasks;
    endResetModel();
}

QList<TaskPtr> TaskModel::tasks() const
{
    return mTasks;
}

int TaskModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : mTasks.count();
}

QVariant TaskModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    if (row < 0 || row >= mTasks.count()) {
        qWarning() << "Invalid row" << row;
        return QVariant();
    }
    TaskPtr task = mTasks.at(row);

    switch (role) {
    case Qt::DisplayRole:
        return task->name;
    default:
        return QVariant();
    }
}
