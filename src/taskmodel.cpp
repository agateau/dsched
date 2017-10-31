#include "taskmodel.h"

#include <QDebug>

TaskModel::TaskModel(QObject* parent)
: QAbstractListModel(parent)
{
}

void TaskModel::setTasks(const QList<Task>& tasks)
{
    beginResetModel();
    mTasks = tasks;
    endResetModel();
}

QList<Task> TaskModel::tasks() const
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
    const Task& task = mTasks.at(row);

    switch (role) {
    case Qt::DisplayRole:
        return task.name;
    default:
        return QVariant();
    }
}
