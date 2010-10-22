#!/usr/bin/env python

"""
Main script that starts up the simulator's interface
"""

import sys

import cli

banner = "Welcome to the simulator's debugging interface. Type help for help."

if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.stderr.write("Usage: %s program\n" % sys.argv[0])
    else:
        cli.SimulatorCLI(sys.argv[1]).cmdloop(banner)
