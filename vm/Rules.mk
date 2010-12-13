# Standard things
sp 		:= $(sp).x
dirstack_$(sp)	:= $(d)
d		:= $(dir)

# Local variables
OBJS_$(d):=$(d)/vm.o $(d)/generated_simulator.o $(d)/interrupt_handler.o
PROFILE_OBJECTS=$(OBJS_$(d)) $(d)/runtest.o
CFLAGS += -O2 -g -Wall -Wno-unused-variable -lm -fPIC -DVM_DEBUG=1

# Set interrupt handler
INTERRUPT_HANDLER=$(d)/interrupt_handler.c

# Use bundled cython
PATH := $(d)/cython/bin:${PATH}
PYTHON_DEBUG=`cython 2>&1 | grep -q '\--debug' && echo --pyrex-debug`
PYTHON=`which python-dbg 2>/dev/null || which python`
PYTHONPATH := $(d)/cython:${PYTHONPATH}

# make sure CFLAGS are also passed to python
export PATH
export PYTHONPATH

# problem if the user has a 64-bit OS but a 32-bit Python
# We could have distutils do everything, if we care
$(d)/cli.so: $(OBJS_$(d))
	cd $(dir) && CFLAGS="$(CFLAGS)" $(PYTHON) setup.py build_ext $(PYTHON_DEBUG)

# Local rules
$(d)/vm.o: $(d)/vm.c $(d)/vm.h $(d)/simulator.h $(d)/readelf.c $(d)/vmerrno.h $(d)/generated_simulator.h
$(d)/generated_simulator.o: $(d)/generated_simulator.c $(d)/generated_simulator.h

$(d)/runtest: $(PROFILE_OBJECTS)
	$(CC) $(CFLAGS) $(PROFILE_OBJECTS) -o $@

# Standard things

-include	$(DEPS_$(d))

d		:= $(dirstack_$(sp))
sp		:= $(basename $(sp))
