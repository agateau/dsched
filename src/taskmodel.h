#ifndef TASKMODEL_H
#define TASKMODEL_H

#include <QAbstractListModel>

#include "task.h"

class TaskModel : public QAbstractListModel {
    Q_OBJECT
public:
    TaskModel(QObject* parent = nullptr);

    void setTasks(const QList<TaskPtr>& tasks);

    QList<TaskPtr> tasks() const;
    QList<TaskPtr> runningTasks() const;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

Q_SIGNALS:
    void runningTasksChanged();

private:
    QList<TaskPtr> mTasks;
};

#endif /* TASKMODEL_H */
