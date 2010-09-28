CFLAGS=-g -Wall

all: doc code

code: vm.o

doc:
	doxygen doxyconfig

clean:
	git clean -d -X -f
