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
