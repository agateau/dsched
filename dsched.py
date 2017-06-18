#!/usr/bin/env python3
import argparse
import logging
import os
import signal
import sys

from configparser import ConfigParser

import arrow

from PyQt5 import QtCore
from PyQt5 import QtGui
from PyQt5 import QtWidgets


__appname__ = 'dsched'
__version__ = '0.2.0'
__license__ = 'Apache 2.0'


TIMER_INTERVAL = 30 * 1000


TASK_SECTION_PREFIX = 'task '


DURATION_FOR_UNIT = {
    's': 1,
    'm': 60,
    'h': 3600
}


class Task:
    def __init__(self, name, command, interval, requires):
        self.name = name
        self.command = command
        self.interval = interval
        self.requires = requires
        self._last_run = None

    def next_run(self):
        if self._last_run is None:
            return None
        return self._last_run.replace(seconds=self.interval)

    def run(self):
        logging.info('Running "%s"', self)
        proc = QtCore.QProcess()
        proc.start('/bin/sh', ['-c', self.command], QtCore.QIODevice.ReadOnly)
        self._last_run = arrow.now()
        logging.info('Done')
        return proc

    def can_run(self):
        if not self.requires:
            return True
        logging.info('Running requirement check "%s"', self.requires)
        returncode = QtCore.QProcess.execute('/bin/sh', ['-c', self.requires])
        ok = returncode == 0
        logging.info('Result: %d => %s', returncode, 'Yes' if ok else 'No')
        return ok

    def __str__(self):
        return '{s.name}: c="{s.command}" r="{s.requires}" i={s.interval}' \
            .format(s=self)


class Controller(QtCore.QObject):
    def __init__(self, tasks):
        super().__init__()
        self.tasks = tasks
        self.processes = set()
        self.setup_timer()
        self.setup_tray()

        self.timer.start()
        QtCore.QTimer.singleShot(0, self.run)
        self.tray.show()

    def setup_timer(self):
        self.timer = QtCore.QTimer(self)
        self.timer.setInterval(TIMER_INTERVAL)
        self.timer.setSingleShot(False)
        self.timer.timeout.connect(self.run)

    def setup_tray(self):
        self.tray = QtWidgets.QSystemTrayIcon(self)
        self.tray.setIcon(QtGui.QIcon.fromTheme('oxygen'))

        self.menu = QtWidgets.QMenu()
        # Would be nicer to call QCoreApplication.exit instead, but that causes
        # a segfault
        self.menu.addAction(self.tr('&Quit'), sys.exit)

        self.tray.setContextMenu(self.menu)

    def run(self):
        now = arrow.now()
        for task in self.tasks:
            next_run = task.next_run()
            if (not next_run or now >= next_run) and task.can_run():
                process = task.run()
                process.finished.connect(
                    lambda returncode, status:
                    self.on_finished(process, returncode, status))
                self.processes.add(process)

    def on_finished(self, process, returncode, status):
        cmd = process.arguments()[1]
        logging.info('"%s" finished with code %d', cmd, returncode)
        self.processes.remove(process)


def parse_interval(txt):
    tokens = txt.split(' ')
    interval = 0
    for token in tokens:
        unit = token[-1]
        value = int(token[:-1])
        interval += value * DURATION_FOR_UNIT[unit]
    return interval


def load_config(config_path):
    parser = ConfigParser()
    parser.read(config_path)
    for section in parser.sections():
        if not section.startswith(TASK_SECTION_PREFIX):
            continue
        name = section[len(TASK_SECTION_PREFIX):]
        command = parser.get(section, 'command')
        interval = parser.get(section, 'interval')
        requires = parser.get(section, 'requires', fallback=None)
        yield Task(name, command, parse_interval(interval), requires)


def setup_logger(logfile, debug):
    level = logging.DEBUG if debug else logging.INFO
    if logfile == '-':
        logfile = None
    logging.basicConfig(
        filename=logfile, level=level,
        format='%(asctime)s:%(levelname)s:%(funcName)s: %(message)s')


def main():
    signal.signal(signal.SIGINT, signal.SIG_DFL)

    config_path = os.path.expandvars('$HOME/.config/dsched/dsched.conf')
    logfile_path = os.path.expandvars('$HOME/.cache/dsched/dsched.log')

    parser = argparse.ArgumentParser()
    parser.description = 'Desktop task scheduler'
    parser.add_argument('-c', '--config', default=config_path)
    parser.add_argument('-l', '--list', action='store_true')
    parser.add_argument('--debug', action='store_true')
    parser.add_argument('--logfile', default=logfile_path)

    args = parser.parse_args()

    if not os.path.exists(args.config):
        parser.error('"{}" does not exist'.format(args.config))

    if not os.path.exists(logfile_path):
        os.makedirs(os.path.dirname(logfile_path), exist_ok=True)

    tasks = list(load_config(args.config))

    if args.list:
        for task in tasks:
            print(task)
        return 0

    setup_logger(logfile=args.logfile, debug=args.debug)

    qapp = QtWidgets.QApplication(sys.argv)
    controller = Controller(tasks)  # noqa
    return qapp.exec_()


if __name__ == '__main__':
    sys.exit(main())
# vi: ts=4 sw=4 et
