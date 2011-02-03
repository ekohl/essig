Embedded System SImulator Generator
===================================

About
-----

The Embedded System SImulator Generator is a design project at the University
of Twente. The goal is to generate a simulator for microcontroller programs
based on the specification of such a microcontroller.
The bug tracker and forum are on Redmine (requires login) [#]_.

Installation
------------
To build the project::

    essig $ make
    essig $ cd vm
    essig/vm $ python simulator.py /path/to/atmega16-program

This requires that you have a working (GNU C compatible) C compiler, a make
program, Java, Maven and Python 2.
In order to build Atmega 16 programs (or programs for other microcontroller
architectures, you need to install a compatible compiler. For Atmega 16, the
avr-gcc, avr-binutils (objdump and such), and avr-libc form a good choice.
These tools can be used like this::

    $ avr-gcc -g -Wall -mmcu=atmega16 -o program program.c
    $ avr-objdump -d program | less

With the project come a few code sample, which can be build from the project's
root directory by typing::

    essig $ make code-samples

The factorial sample can then be run like this::

    essig/vm $ python simulator.py ../code-samples/factorial

or::

    essig/vm $ make run

To debug the simulator itself use (requires GDB)::

    essig/vm $ make run-debug

For further instructions and a general overview of the project, see the
'report' directory::

    essig $ cd report
    essig/report $ make

This should make a report.pdf in the current directory. For this you will
need Latex and TexLive.

References
----------
.. [#] http://fmt.cs.utwente.nl/redmine/projects/essig.
