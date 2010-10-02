CFLAGS=-g -Wall

all: doc code

code: vm.o dummy_simulator.o

doc:
	doxygen doxyconfig

clean:
	git clean -d -X -f
