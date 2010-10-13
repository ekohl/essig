#cython: nonecheck=True

"""
Command Line Interface for the simulator
"""

import sys
import cmd
import subprocess

class VMError(Exception):
    "raised for any error originating in the simulator"
    def __init__(self, *args):
        if not args:
            args = vm_strerror(-1),
            
        super(VMError, self).__init__(args)
        

cdef class VMPythonItem(object):
    cdef VMIterable *item

cdef create_VMPythonItem(VMIterable *item):
        cdef VMPythonItem result = VMPythonItem()
        result.item = item
        return result


cdef class VMPythonIterator(object):
    cdef VMIterable *it
    
    def __iter__(self):
        if not self.it:
            raise StopIteration()
            
        result = create_VMPythonItem(self.it)
        self.it = self.it.next
        return result

cdef create_VMPythonIterator(void *it):
    cdef VMPythonIterator result 
    if not it:
        return iter(())
        
    result = VMPythonIterator()
    result.it = <VMIterable *> it
    return result
    
    
cdef class Simulator(object):
    cdef VMState *state
    cdef VMStateDiff *diff
    cdef instructions
    
    def __init__(self, instructions):
        """
        \param program Path to the executable that should be simulated
        """
        # ensure a reference to self.instructions
        self.instructions = instructions
        
        self.state = vm_newstate(<char *> self.instructions, 
                                 len(self.instructions), 
                                 VM_POLICY_INTERRUPT_NEVER)
        if not self.state:
            raise VMError()
        
        self.diff = vm_newdiff()
        if not self.diff:
            raise VMError()
    
    def __dealloc__(self):
        vm_closestate(self.state)
        vm_closediff(self.diff)
    
            
class SimulatorCLI(cmd.Cmd, object):
    
    def __init__(self, program):
        super(SimulatorCLI, self).__init__()
        self.program = program
        self.simulator = Simulator(self.read_instructions())
        self.symtab = self.read_symtab()
        self.prompt = '(sim) '
    
    def read_instructions(self):
        """
        Read instructions from the executable
        """
        return "foo"
        
    def read_symtab(self):
        p = subprocess.Popen(['nm', self.program], stdout=subprocess.PIPE)
        symtab = {}
        for line in p.stdout:
            try:
                addr, type, funcname = line.split()
            except ValueError:
                # Symbol did not have an address (external symbol)
                pass
            else:
                symtab[funcname] = int(addr, 16)
        
        p.stdout.close()
        p.wait()
        return symtab
        
    def do_break(self, funcname_or_addr):
        "Set a breakpoint for an address or a function"
        try:
            # parse the int according to its base
            addr = int(funcname_or_addr, 0)
        except ValueError:
            addr = self.symtab.get(funcname_or_addr)
            if addr is None:
                self.print_err("No such function: %s" % funcname_or_addr)
                return
       
        if vm_break((<Simulator> self.simulator).state, addr) != 0:
            self.print_err()
        
    def do_cont(self):
        "continue or run the program"
        cdef Simulator sim
        cdef bool hit_bp
        
        sim = self.simulator
        
        if not vm_cont(sim.state, NULL, &hit_bp):
            raise VMError()
            
        if hit_bp:
            pc = (<Simulator> self.simulator).state.pc
            print 'Hit breakpoint at %x' % pc
    
    def complete_break(self, text, line, beginidx, endidx):
        return self.complete_from_it(text, self.symtab)
    
    def do_info(self, about):
        """
        Show information about stuff:
            breakpoints
            ram address
            register name
            pin name
        """
        cdef Simulator sim = self.simulator
        if about == 'breakpoints':
            it = create_VMPythonIterator(sim.state.breakpoints)
            for i, bp in enumerate(it):
                print '%2d   %x' % (i + 1, (<VMBreakpoint *> bp.item).offset)
    
    def complete_info(self, text, line, beginidx, endidx):
        options = 'breakpoints', 'ram', 'register', 'pin'
        return self.complete_from_it(text, options)
    
    def complete_from_it(self, text, it):
        "complete command beginning with text from iterable"
        return [s for s in it if s.startswith(text)]
    
    def do_EOF(self, _):
        "Exit the simulator"
        sys.exit("Bye")
    
    do_exit = do_quit = do_EOF
    
    def print_err(self, msg=None):
        if msg is None:
            msg = vm_strerror(-1)
        sys.stderr.write(msg + "\n")