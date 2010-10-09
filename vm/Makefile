CFLAGS=-g -Wall -fPIC -DVM_DEBUG=1 -DVM_WITH_THREADS=1
OBJECTS=vm.o dummy_simulator.o

all: doc cli

# problem if the user has a 64-bit OS but a 32-bit Python
# We could have distutils do everything, if we care
cli: $(OBJECTS)
	python setup.py build_ext --inplace

doc:
	doxygen doxyconfig >/dev/null

clean:
	git clean -d -X -f
	rm *.o *.so cli.c
