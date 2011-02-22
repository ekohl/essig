"""
This is an example plugin that defines a callback which is called at every
step. 'simulator' is a global variable automatically available.
A plugin may inspect and alter the state of the simulation and may specify
new interrupts that should be triggered by using 'cli.specify_vm_interrupt'.
"""

import sys

import cli

def cycle_progress_bar():
    import time; time.sleep(0.005)
    sys.stdout.write('%s\r' % simulator.cycles)
    sys.stdout.flush()

def specify_interrupt():
    # Specify an interrupt that should be triggered after 1 cycle has passed
    cli.specify_vm_interrupt(simulator, cli.INTERRUPT_TIMER, 1)


cli.register_callback(simulator, cycle_progress_bar)
cli.register_callback(simulator, specify_interrupt)
