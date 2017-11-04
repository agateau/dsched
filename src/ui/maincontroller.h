#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>
#include <QTimer>

class TaskModel;

class MainController : public QObject
{
    Q_OBJECT
public:
    explicit MainController(TaskModel* taskModel);

private:
    void setupTimer();
    void run();

    TaskModel* mTaskModel;
    QTimer* mTimer;
};

#endif /* MAINCONTROLLER_H */
