#cython: nonecheck=True

"""
Command Line Interface for the simulator
"""

import os
import sys
import cmd
import glob
import traceback
import subprocess


class VMError(Exception):
    "raised for any error originating in the simulator"
    def __init__(self, *args):
        if not args:
            args = vm_strerror(-1),
            
        super(VMError, self).__init__(args)
        
    
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
    
    def load_plugins(self):
        # Don't use __import__, but exec. This way every simulator can have
        # it's separate plugins! We could also use __import__ + a metaclass
        # to register subclasses and instantiate them with the simulator as
        # argument.
        path = os.path.join(os.path.dirname('__file__'), 'plugins', '*.py')
        for modulename in glob.glob(path):
            d = {'simulator': self}
            exec open(modulename).read() in d, d
    
    def __dealloc__(self):
        vm_closestate(self.state)
        vm_closediff(self.diff)

    property cycles:
        def __get__(self):
            return self.state.cycles
    
            
class SimulatorCLI(cmd.Cmd, object):
    
    def __init__(self, program):
        super(SimulatorCLI, self).__init__()
        self.program = program
        self.simulator = Simulator(open(program).read())
        self.symtab = self.read_symtab()
        self.symtab_offset_to_func = {v : k for k, v in self.symtab.items()}
        self.prompt = '(sim) '
    
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
        cdef VMState *state = (<Simulator> self.simulator).state
        
        try:
            # parse the int according to its base
            addr = int(funcname_or_addr, 0)
        except ValueError:
            addr = self.symtab.get(funcname_or_addr)
            if addr is None:
                self.print_err("No such function: %s" % funcname_or_addr)
                return
        
        if not vm_break(state, addr):
            self.print_err()
        
    def complete_break(self, text, line, beginidx, endidx):
        return self.complete_from_it(text, self.symtab)
    
    def do_cont(self, args):
        "continue or run the program"
        cdef Simulator sim
        cdef bint hit_bp
        
        sim = self.simulator
        
        if not vm_cont(sim.state, NULL, &hit_bp):
            self.print_err()

        if sim.state.stopped_running:
            print 'Stopped running.'
        elif hit_bp:
            pc = (<Simulator> self.simulator).registers[PC]
            print 'Hit breakpoint at %x' % pc
    
    def info_breakpoints(self, Simulator sim, about):
        cdef VMBreakpoint *bp = sim.state.breakpoints
        
        index = 0
        while bp:
            if bp.offset in self.symtab_offset_to_func:
                t = self.symtab_offset_to_func[bp.offset], bp.offset
                breakpoint = '%s at 0x%016x' % t
            else:
                breakpoint = '0x%016x' % bp.offset
                
            print '%2d   %s' % (index, breakpoint)
            
            bp = bp.next
            index += 1
    
    def info_cycles(self, Simulator sim, about):
        print sim.state.cycles, 'cycles have passed.'
    
    def info_registers(self, Simulator sim, about, register=None):
        for i in range(nregisters):
            if register is None or registers[i].name == register:
                val = sim.state.registers[registers[i].offset]
                print '%-15s 0x%016x' % (registers[i].name, val)
                if register is not None:
                    break
        else:
            if register is not None:
                print 'No such register: %r' % register
    
    def info_symbols(self, Simulator sim, about):
        for symname, offset in sorted(self.symtab.iteritems()):
            print '%-30s 0x%016x' % (symname, offset)
    
    def info_ram(self, Simulator sim, about):
        cdef bint error = False
        cdef OPCODE_TYPE value
        
        if not about:
            print 'Ramsize: 0x%x' % ramsize
        else:
            try:
                address = int(about, 0)
                if address < 0:
                    raise ValueError("Address must be positive.")
            except ValueError, e:
                print e
            else:
                value = vm_info(sim.state, VM_INFO_RAM, address, <bool *> &error)
                if error:
                    self.print_err()
                else:
                    print value
    
    def info_register(self, Simulator sim, about):
        if about:
            self.info_registers(sim, about, register=about)
        else:
            print 'Provide the name of a register.'
    
    def do_info(self, about):
        """
        Show information about stuff:
            breakpoints
            registers
            ram
            symbols
            cycles
            
            ram address
            register name
            pin name
        """
        info_type, _, about = about.expandtabs(1).partition(' ')
        info_func = getattr(self, 'info_' + info_type, None)
        
        if info_func is not None:
            info_func(self.simulator, about)
        else:
            sys.stderr.write("Invalid info command: %r\n" % info_type)
        
    def complete_info(self, text, line, beginidx, endidx):
        options = ('breakpoints', 'registers', 'symbols', 'cycles',
                   'ram', 'register', 'pin')
        return self.complete_from_it(text, options)
    
    def do_disassemble(self, args):
        """
        Disassemble the program that's to be simulated.
        """
        cdef:
            Simulator sim
            Opcode *op
            OpcodeHandler *handler
            size_t i, address
            
        sim = self.simulator
        print '%-10s     %-5s %-15s %-15s' % ('Address', 'PC', 'Opcode', 'Instruction')
        offset = sim.state.executable_segment_offset
        for i from 0 <= i < sim.state.instructions_size:
            op = sim.state.instructions + i
            handler = opcode_handlers + op.opcode_index
            address =  offset + i * sizeof(OPCODE_TYPE)
            print '%6x     %6x     %-15s 0x%0*x' % (
                address,
                address / sizeof(OPCODE_TYPE),
                handler.opcode_name, 
                sizeof(OPCODE_TYPE) * 2,
                op.instruction)
            
    def complete_from_it(self, text, it):
        "complete command beginning with text from iterable"
        return [s for s in it if s.startswith(text)]
    
    def do_EOF(self, _):
        "Exit the simulator"
        print "Bye"
        sys.exit()
    
    do_exit = do_quit = do_EOF
    
    def print_err(self, msg=None):
        if msg is None:
            msg = vm_strerror(-1)
        sys.stderr.write(msg + "\n")


cdef bool python_callback(VMState *state, void *argument):
    cdef object python_callable = <object> argument
    
    try:
        python_callable()
        return true
    except:
        traceback.print_exc()
        sys.exc_clear()
        return false

def register_callback(Simulator sim, callback):
    Py_INCREF(callback)
    if not vm_register_interrupt_callable(sim.state, <void *> &python_callback, 
                                          <void *> callback):
        raise VMError()


INTERRUPT_TIMER = VM_INTERRUPT_TIMER

def specify_vm_interrupt(Simulator sim, interrupt_type, argument):
    vm_interrupt(sim.state, interrupt_type, <unsigned int> argument)