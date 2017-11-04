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
    for (const auto& task : mTasks) {
        connect(task.data(), &Task::runningChanged, this, &TaskModel::updateStatus);
    }
    endResetModel();
}

QList<TaskPtr> TaskModel::tasks() const
{
    return mTasks;
}

QList<TaskPtr> TaskModel::runningTasks() const
{
    QList<TaskPtr> lst;
    for (const auto& task : mTasks) {
        if (task->isRunning()) {
            lst << task;
        }
    }
    return lst;
}

Task::Status TaskModel::status() const
{
    return mStatus;
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

void TaskModel::updateStatus()
{
    Task::Status status = [this] {
        auto status = Task::Status::Idle;
        for (const auto& task : mTasks) {
            Task::Status taskStatus = task->status();
            if (taskStatus == Task::Status::Running) {
                // No need to go to the end of the list
                return Task::Status::Running;
            }
            if (taskStatus == Task::Status::Error) {
                // One error so we will at least return error, unless we have a running task
                status = Task::Status::Error;
            }
        }
        return status;
    }();
    if (mStatus != status) {
        mStatus = status;
        statusChanged();
    }
}
