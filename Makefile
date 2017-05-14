all: venv

clean:
	-rm -rf venv

venv:
	virtualenv --python python3 venv
	. venv/bin/activate && pip install -r requirements.txt

shell: venv
	. venv/bin/activate && exec $$SHELL
