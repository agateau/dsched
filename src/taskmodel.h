#ifndef TASKMODEL_H
#define TASKMODEL_H

#include <QAbstractListModel>

#include "task.h"

class TaskModel : public QAbstractListModel {
public:
    TaskModel(QObject* parent = nullptr);

    void setTasks(const QList<Task>& tasks);

    QList<Task> tasks() const;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

private:
    QList<Task> mTasks;
};

#endif /* TASKMODEL_H */
