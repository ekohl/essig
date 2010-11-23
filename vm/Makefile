CFLAGS += -g -Wall -fPIC -DVM_DEBUG=1
OBJECTS=vm.o generated_simulator.o
PROFILE_OBJECTS=$(OBJECTS) runtest.o
PYTHON_DEBUG=`cython 2>&1 | grep -q '\--debug' && echo --pyrex-debug`
PYTHON=python #`which python-dbg || which python`
PYTHONPATH=$${PYTHONPATH}:cython

TESTFILE1=../code-samples/factorial
TESTFILE2=../code-samples/empty
TESTFILE=$(TESTFILE1)

PROFILE_N_STEPS = 10000000

# make sure CFLAGS are also passed to python
export PYTHONPATH

run: cli
	$(PYTHON) simulator.py $(TESTFILE)

run-debug: cli
	cygdb . --args $(PYTHON) simulator.py $(TESTFILE)

run-pdb: cli
	$(PYTHON) -m pdb simulator.py $(TESTFILE)

profile: 
	make clean
	CFLAGS="$$CFLAGS -pg -O3" LDFLAGS="-pg" $(MAKE) _profile

cachegrind:
	make clean
	CFLAGS="$$CFLAGS-O3" $(MAKE) _cachegrind

runtest: $(PROFILE_OBJECTS)
	$(CC) $(CFLAGS) $(PROFILE_OBJECTS) -o runtest

_profile: runtest
	./runtest $(PROFILE_N_STEPS)
	gprof runtest gmon.out

_cachegrind: runtest
	valgrind --tool=callgrind --callgrind-out-file=callgrind.out \
		     ./runtest $(PROFILE_N_STEPS)
	kcachegrind callgrind.out

# problem if the user has a 64-bit OS but a 32-bit Python
# We could have distutils do everything, if we care
cli: $(OBJECTS)
	CFLAGS=$(CFLAGS) $(PYTHON) setup.py build_ext --inplace $(PYTHON_DEBUG)

vm.o: vm.c vm.h simulator.h readelf.c vmerrno.h
generated_simulator.o: generated_simulator.c generated_simulator.h

doc:
	doxygen doxyconfig

clean:
	git clean -d -X -f
