#!/usr/bin/env python3
import argparse
import logging
import os
import subprocess
import sys
import time

from configparser import ConfigParser

import arrow


TASK_SECTION_PREFIX = 'task '


DURATION_FOR_UNIT = {
    's': 1,
    'm': 60,
    'h': 3600
}


class Task:
    def __init__(self, name, command, interval):
        self.name = name
        self.command = command
        self.interval = interval
        self._last_run = None

    def next_run(self):
        if self._last_run is None:
            return None
        return self._last_run.replace(seconds=self.interval)

    def run(self):
        logging.info('Running "%s"', self)
        subprocess.Popen(self.command, shell=True)
        self._last_run = arrow.now()
        logging.info('Done')

    def __str__(self):
        return '{s.name}: {s.command} - {s.interval}'.format(s=self)


def run(tasks):
    while True:
        now = arrow.now()
        for task in tasks:
            next_run = task.next_run()
            if not next_run or now >= next_run:
                task.run()
        time.sleep(60)


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
        yield Task(name, command, parse_interval(interval))


def setup_logger(logfile, debug):
    level = logging.DEBUG if debug else logging.INFO
    logging.basicConfig(
        filename=logfile, level=level,
        format='%(asctime)s:%(levelname)s:%(funcName)s: %(message)s')


def main():
    config_path = os.path.expandvars('$HOME/.config/dsched/dsched.conf')

    parser = argparse.ArgumentParser()
    parser.description = 'Desktop task scheduler'
    parser.add_argument('-c', '--config', default=config_path)
    parser.add_argument('-l', '--list', action='store_true')
    parser.add_argument('--debug', action='store_true')
    parser.add_argument('--logfile')

    args = parser.parse_args()

    if not os.path.exists(args.config):
        parser.error('"{}" does not exist'.format(args.config))

    tasks = list(load_config(args.config))

    if args.list:
        for task in tasks:
            print(task)
        return 0

    setup_logger(logfile=args.logfile, debug=args.debug)
    run(tasks)
    return 0


if __name__ == '__main__':
    sys.exit(main())
# vi: ts=4 sw=4 et
