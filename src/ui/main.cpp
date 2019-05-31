/*
 * Copyright 2017 Aurélien Gâteau <mail@agateau.com>
 *
 * This file is part of Dsched.
 *
 * Dsched is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QIcon>
#include <QMessageBox>
#include <QStandardPaths>

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
    : configPath(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/dsched.conf")
    , logDirName(QStandardPaths::writableLocation(QStandardPaths::CacheLocation))
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
    QApplication::setApplicationName("dsched");
    app.setWindowIcon(QIcon(":/appicon.svg"));

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
