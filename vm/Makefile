# inherit rules
dir := .
include Rules.mk

TESTFILE1=../code-samples/factorial
TESTFILE2=../code-samples/empty
TESTFILE=$(TESTFILE1)

PROFILE_N_STEPS = 10000000

run: cli.so
	@$(PYTHON) simulator.py $(TESTFILE)

run-debug: cli.so
	cygdb . --args $(PYTHON) simulator.py $(TESTFILE)

run-pdb: cli.so
	$(PYTHON) -m pdb simulator.py $(TESTFILE)

profile: clean
	ulimit -v $$((1024 * 1024)) && \
	CFLAGS="$$CFLAGS -pg -O3" LDFLAGS="-pg" $(MAKE) _profile

cachegrind: clean
	ulimit -v $$((1024 * 1024)) && \
	CFLAGS="$$CFLAGS -O3" $(MAKE) _cachegrind

_profile: runtest
	./runtest $(PROFILE_N_STEPS)
	gprof runtest gmon.out

_cachegrind: runtest
	valgrind --tool=callgrind --callgrind-out-file=callgrind.out \
		     ./runtest $(PROFILE_N_STEPS)
	kcachegrind callgrind.out

doc:
	doxygen doxyconfig

clean:
	git clean -d -X -f

.PHONY: doc clean run _profile _cachegrind cachegrind profile run-pdb run-debug run
