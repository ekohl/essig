#!/usr/bin/env python

"""
Main script that starts up the simulator's interface
"""

import sys

import cli

def empty_callback():
    """
    Needed so Python can call PyErr_CheckSignals in case the user hits ^C.
    We could also set a signal handler for SIGINT and call 
    vm_break_async_signal, which would be a lot faster, but we wouldn't be
    able to interrupt any Python code (not from the C signal handler at least).
    """


banner = "Welcome to the simulator's debugging interface. Type help for help."

if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.stderr.write("Usage: %s program\n" % sys.argv[0])
    else:
        simulator_cli = cli.SimulatorCLI(sys.argv[1])
        cli.register_callback(simulator_cli.simulator, empty_callback)
        simulator_cli.simulator.load_plugins()
        simulator_cli.cmdloop(banner)
