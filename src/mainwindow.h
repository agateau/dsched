#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScopedPointer>

#include "task.h"
#include "ui_mainwindow.h"

class TaskModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(TaskModel* model);

private:
    void onTaskSelected(const QModelIndex& index);
    void setCurrentTask(const TaskPtr& task);

    QScopedPointer<Ui_MainWindow> mUi;
    TaskModel* mModel;
};

#endif // MAINWINDOW_H
