#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>

#include <iostream>

#include "task.h"
#include "tasktools.h"
#include "window.h"

using namespace std;

struct CommandLineArgs
{
    QString configPath;
    QString logPath;
    bool list = false;
    bool debug = false;

    CommandLineArgs()
    : configPath(QDir::home().absoluteFilePath(".config/dsched/dsched.conf"))
    , logPath(QDir::home().absoluteFilePath(".cache/dsched/dsched.log"))
    {}

    void parseArguments(const QCoreApplication& app)
    {
        QCommandLineParser parser;
        parser.addOption({{"c", "config"}, "Path to Dsched configuration file.",
                "CONFIG_FILE", configPath});
        parser.addOption({{"l", "list"}, "List tasks."});
        parser.addOption({"debug", "Start in debug mode."});
        parser.addOption({"logfile", "Path to Dsched log file.", "LOG_FILE", logPath});
        parser.addHelpOption();

        parser.process(app);

        debug = parser.isSet("debug");
        list = parser.isSet("list");
        configPath = parser.value("config");
        logPath = parser.value("logfile");
    }
};

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    CommandLineArgs args;
    args.parseArguments(app);

    QString error;
    QList<Task> tasks = TaskTools::load(args.configPath, &error);
    if (!error.isEmpty()) {
        cerr << qPrintable(error) << '\n';
        return 2;
    }

    if (args.list) {
        for(const auto& task : tasks) {
            cout << task;
        }
        return 0;
    }

    Window window;
    window.show();
    return app.exec();
}
