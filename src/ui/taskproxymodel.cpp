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
