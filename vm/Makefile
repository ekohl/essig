CFLAGS=-g -Wall -fPIC -DVM_DEBUG=1 -DVM_WITH_THREADS=1
OBJECTS=vm.o generated_simulator.o
PYTHON_DEBUG=`cython 2>&1 | grep -q '\--debug' && echo --pyrex-debug`
PYTHON=python #`which python-dbg || which python`
PYTHONPATH=$${PYTHONPATH}:cython

TESTFILE1=../code-samples/factorial
TESTFILE2=../code-samples/empty
TESTFILE=$(TESTFILE1)

# make sure CFLAGS are also passed to python
export CFLAGS
export PYTHONPATH

run: cli
	$(PYTHON) simulator.py $(TESTFILE)

run-debug: cli
	cygdb . --args $(PYTHON) simulator.py $(TESTFILE)

run-pdb: cli
	$(PYTHON) -m pdb simulator.py $(TESTFILE)



# problem if the user has a 64-bit OS but a 32-bit Python
# We could have distutils do everything, if we care
cli: $(OBJECTS)
	$(PYTHON) setup.py build_ext --inplace $(PYTHON_DEBUG)

vm.o: vm.c vm.h simulator.h readelf.c vmerrno.h
generated_simulator.o: generated_simulator.c generated_simulator.h

doc:
	doxygen doxyconfig

clean:
	git clean -d -X -f
