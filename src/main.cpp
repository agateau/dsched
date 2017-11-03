#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QMessageBox>

#include <iostream>

#include "logger.h"
#include "maincontroller.h"
#include "task.h"
#include "taskmodel.h"
#include "tasktools.h"
#include "traycontroller.h"

using namespace std;

struct CommandLineArgs
{
    QString configPath;
    QString logDirName;
    bool list = false;
    bool debug = false;

    CommandLineArgs()
    : configPath(QDir::home().absoluteFilePath(".config/dsched/dsched.conf"))
    , logDirName(QDir::home().absoluteFilePath(".cache/dsched"))
    {}

    void parseArguments(const QCoreApplication& app)
    {
        QCommandLineParser parser;
        parser.addOption({{"c", "config"}, "Path to Dsched configuration file.",
                "CONFIG_FILE", configPath});
        parser.addOption({{"l", "list"}, "List tasks."});
        parser.addOption({"debug", "Start in debug mode."});
        parser.addOption({"logdir", "Path to Dsched log dir.", "LOG_DIR", logDirName});
        parser.addHelpOption();

        parser.process(app);

        debug = parser.isSet("debug");
        list = parser.isSet("list");
        configPath = parser.value("config");
        logDirName = parser.value("logdir");
    }
};

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    CommandLineArgs args;
    args.parseArguments(app);
    Logger::setup(args.logDirName + "/dsched.log", args.debug ? Logger::Mode::Debug : Logger::Mode::Normal);

    QString error;
    QList<TaskPtr> tasks = TaskTools::load(args.configPath, &error);
    if (!error.isEmpty()) {
        QString message = QObject::tr("<b>Failed to parse %1</b><br><br>%2").arg(args.configPath).arg(error);
        QMessageBox::critical(nullptr, QObject::tr("Dsched"), message);
        return 2;
    }

    if (args.list) {
        for(const auto& task : tasks) {
            cout << *task;
        }
        return 0;
    }

    for(auto& task : tasks) {
        task->setTasksLogDirName(args.logDirName + "/tasks/");
    }

    TaskModel model;
    model.setTasks(tasks);
    MainController controller(&model);
    TrayController trayController(&model);
    return app.exec();
}
