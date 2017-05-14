## Config file

~/.config/dsched/dsched.conf

    [task <name>]
    command=
    interval=

    # Optional: command which must succeed to run the task
    requires=

## Files

~/.cache/dsched/tasks/<name>.log

## Architecture

    class Task:
        command
        interval
        _last_run

        def next_run():
            return _last_run + interval

    def run(tasks):
        while True:
            now = now()
            for task in tasks:
                if now >= task.next_run()
                    run_task(task)
            sleep(60)

    load_config:
        read config => [Task]

    main():
        tasks = load_config()
        run(tasks)
