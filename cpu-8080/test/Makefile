
SHELL=/bin/bash
all: test

test: $(CPULIB) .venv/lib/python3.11/site-packages/yaml
	bash -c 'source .venv/bin/activate && python ./test.py $(CPULIB)'
	touch test

.venv:
	python3 -m venv .venv

.venv/lib/python3.11/site-packages/yaml: .venv
	bash -c 'source .venv/bin/activate && pip install -r requirements.txt'

clean:
	rm -fr .venv test