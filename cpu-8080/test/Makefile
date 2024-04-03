
# SHELL=/bin/bash
all: test

test: $(CPULIB) venv
	bash -c 'source .venv/bin/activate && python ./test.py $(CPULIB)'
	touch test

venv:
	python3 -m venv .venv
	bash -c 'source .venv/bin/activate && pip install -r requirements.txt'
	touch venv
clean:
	rm -fr .venv test venv