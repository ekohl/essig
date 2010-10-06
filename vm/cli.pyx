"""
Command Line Interface for the simulator
"""

import readline

class VMError(Exception):
    "raised for any error originating in the simulator"
    def __init__(self, *args):
        if not args:
            args = vm_strerror(-1),
            
        super(VMError, self).__init__(args)
        
        
cdef class Simulator(object):
    cdef VMState *state
    cdef VMStateDiff *diffs
    
    cdef public program
    cdef public instructions
    
    
    def __init__(self, program):
        """
        \param program Path to the executable that should be simulated
        """
        
        self.program = program
        self.instructions = self.read_instructions()
        
        self.state = vm_newstate(<char *> self.instructions, VM_POLICY_INTERRUPT_NEVER)
        if not self.state:
            raise VMError()
        
        self.diffs = vm_newdiff()
        if not self.diffs:
            raise VMError()
    
    def read_instructions(self):
        """
        Read instructions from the executable
        """
        