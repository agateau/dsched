#include "mainwindow.h"

#include "taskmodel.h"

MainWindow::MainWindow(TaskModel* model)
: mUi(new Ui_MainWindow)
{
    mUi->setupUi(this);

    mUi->listView->setModel(model);
}
