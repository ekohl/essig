#cython: nonecheck=True

"""
Command Line Interface for the simulator
"""

import os
import sys
import cmd
import glob
import functools
import traceback
import subprocess


class CLIError(Exception):
    
    def __init__(self, *args):
        if not args:
            args = vm_strerror(-1),
            
        super(CLIError, self).__init__(*args)

class VMError(CLIError):
    "raised for any error originating in the simulator"

class ErrorMessage(CLIError):
    "raised to print error messages"


class CreateClosure(object):
    """
    Helper class because Cython can't deal with closures. 
    It takes the original function and the wrapper function as arguments.
    
    Don't use functools.wraps() with this class, as it replaces __dict__.
    """
    
    def __init__(self, wrapper, func):
        self.wrapper = wrapper
        self.func = func
    
    def __call__(self, *args, **kwargs):
        "This is used as a function decorator"
        return self.wrapper(self.func, *args, **kwargs)
    
    def __getattribute__(self, attr):
        if attr in ('__doc__', '__module__', '__name__'):
            # work around a bug (or feature?) of older Cython versions where
            # getattr() needs three arguments
            if hasattr(self.func, attr):
                return getattr(self.func, attr, None)
            raise AttributeError(attr)
            
        return super(CreateClosure, self).__getattribute__(attr)
    
    def __get__(self, obj, type=None):
        "Implement a non-data/non-overriding descriptor to bind the method."
        return BoundMethodClosure(self.wrapper, self.func, obj, type)


class BoundMethodClosure(CreateClosure):
    """
    Simulate bound and unbound methods. We could implement __call__ on 
    CreateClosure, but it would mean e.g.
        
        obj = Class()
        obj.m()
        Class.m()
   
    would be valid, because 'm' would still be bound. Besides, it's not
    thread-safe (i.e., subject to race conditions).
    """
    
    def __init__(self, wrapper, func, obj, type):
        super(BoundMethodClosure, self).__init__(wrapper, func)
        self.obj = obj
        self.type = type
    
    def __call__(self, *args, **kwargs):
        "Used as a method decorator"
        if self.obj is None:
            if args and isinstance(args[0], self.type):
                return self.wrapper(self.func, self.obj, *args, **kwargs)
            
            raise TypeError("'self' not provided or not an instance of this "
                            "class")
        else:
            return self.wrapper(self.func, self.obj, *args, **kwargs)


def resume_execution_wrapper(wrapped_func, self, *args, **kwargs):
    cdef Simulator sim = self.simulator
    
    if not sim.running:
        raise ErrorMessage("The program is not running. Use the 'run' "
                            "command to start it.")
    
    self.check_hit_breakpoint(wrapped_func(self, *args, **kwargs))
    
    if sim.state.stopped_running:
        raise ErrorMessage('The program has stopped running.')

def resume_execution_decorator(func):
    return CreateClosure(resume_execution_wrapper, func)

def parse_nsteps_wrapper(wrapped_func, self, nsteps_str, *args, **kwargs):
    if nsteps_str:
        try:
            nsteps = int(nsteps_str)
            if nsteps <= 0:
                raise ValueError
        except ValueError:
            raise ErrorMessage('Invalid number of steps: %r' % nsteps_str)
    else:
        nsteps = 1
    
    return wrapped_func(self, nsteps, *args, **kwargs)

def parse_nsteps_decorator(func):
    return CreateClosure(parse_nsteps_wrapper, func)
    

cdef class Simulator(object):
    cdef VMState *state
    cdef VMStateDiff *diff, *firstdiff
    cdef public bint running
    cdef instructions
    
    def __init__(self, instructions):
        """
        \param program Path to the executable that should be simulated
        """
        # ensure a reference to self.instructions
        self.instructions = instructions
        self.state = NULL
        self.load_state()
        
    
    def load_state(self):
        """
        Load the underlying VMState. If a previous state was set, deallocate 
        it. We need this to implement the 'run' command.
        """
        cdef VMBreakpoint *breakpoints = NULL
        cdef VMInterruptCallable *interrupt_callables = NULL
        
        if self.state:
            # copy breakpoints and interrupt_callables (i.e., ensure they don't
            # get deallocated and save a pointer
            breakpoints = self.state.breakpoints
            interrupt_callables = self.state.interrupt_callables
            
            self.state.breakpoints = NULL
            self.state.interrupt_callables = NULL
            
            vm_closestate(self.state)
            vm_closediff(self.diff)
        
        self.running = False
        
        self.state = vm_newstate(<char *> self.instructions, 
                                 len(self.instructions),
                                 VM_POLICY_INTERRUPT_NEVER)
        if not self.state:
            raise VMError()
        
        self.state.breakpoints = breakpoints
        self.state.interrupt_callables = interrupt_callables
        
        self.firstdiff = self.diff = vm_newdiff()
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
        cdef Register *register
        
        super(SimulatorCLI, self).__init__()
        self.program = program
        self.simulator = Simulator(open(program).read())
        self.symtab = self.read_symtab()
        self.symtab_offset_to_func = {v : k for k, v in self.symtab.items()}
        self.prompt = '(sim) '
        
        self.registers = {}
        for i from 0 <= i < nregisters:
            self.registers[registers[i].name.lower()] = registers[i].offset
        
        self.register_fmt = '%-15s 0x%-*x'
        
        self.vm_info_type = {
            'register': VM_INFO_REGISTER,
            'ram'     : VM_INFO_RAM,
            'pin'     : VM_INFO_PIN,
        }
        
        # indicates whether the 'run' command has been called. If it has been
        # called before, have Simulator allocate a new VMState
        self.firstrun = True
    
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
    
    def do_run(self, args):
        "Start the program."
        cdef Simulator sim = self.simulator
        cdef bint hit_bp
        
        if not self.firstrun:
            self.simulator.load_state()
        
        self.firstrun = False
    
        self.simulator.running = True
        if not vm_run(sim.state, &sim.diff, &hit_bp):
            raise ErrorMessage()

        self.check_hit_breakpoint(hit_bp)
        
        if sim.state.stopped_running:
            raise ErrorMessage('The program has stopped running.')
        
    @resume_execution_decorator
    def do_cont(self, args):
        "continue or run the program"
        cdef Simulator sim = self.simulator
        cdef bint hit_bp
        
        if not vm_cont(sim.state, &sim.diff, &hit_bp):
            raise ErrorMessage()

        return hit_bp
    
    @resume_execution_decorator
    def do_rcont(self, args):
        "continue or run the program"
        cdef Simulator sim = self.simulator
        cdef bint hit_bp
        
        # if not sim.running:
            # raise ErrorMessage("The program is not running. Use the 'run' "
                               # "command to start it.")
        
        vm_rcont(sim.state, &sim.diff, &hit_bp)
        return hit_bp
    
    @resume_execution_decorator
    @parse_nsteps_decorator
    def do_step(self, nsteps):
        "Make a single step to the next instruction."
        cdef Simulator sim = self.simulator
        cdef bint hit_bp
        
        if not vm_step(sim.state, nsteps, &sim.diff, &hit_bp):
            raise VMError()
        
        if not hit_bp:
            self.info_instruction(self.simulator, '')        

        return hit_bp
    
    @resume_execution_decorator
    @parse_nsteps_decorator
    def do_rstep(self, nsteps):
        "Make a single reversed step to the previous instruction."
        cdef Simulator sim = self.simulator
        cdef bint hit_bp
        
        if sim.diff == sim.firstdiff:
            raise ErrorMessage("We're at the beginning of the program.")
        
        vm_rstep(sim.state, nsteps, &sim.diff, &hit_bp)
        
        if not hit_bp:
            self.info_instruction(self.simulator, '')

        return hit_bp
    
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
    
    def info_registers(self, Simulator sim, about):
        for i in range(nregisters):
            val = sim.state.registers[registers[i].offset]
            print self.register_fmt % (registers[i].name, 
                                       sizeof(OPCODE_TYPE) * 2,
                                       val)
            
    
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
                value = vm_info(sim.state, VM_INFO_RAM, address, 
                                <bool *> &error)
                if error:
                    self.print_err()
                else:
                    print value
    
    def info_register(self, Simulator sim, about):
        offset = self.registers.get(about.lower())
        if offset is None:
            raise ErrorMessage('No such register: %r' % about)
        
        print self.register_fmt % (about, 
                                   sizeof(OPCODE_TYPE) * 2,
                                   sim.state.registers[offset])
        
    def info_instruction(self, Simulator sim, about, pc=None):
        cdef Opcode *op
        
        if pc is None:
            pc = sim.state.registers[PC]
        
        if not 0 <= pc < sim.state.instructions_size:
            raise ErrorMessage('PC out of bounds: %r' % (pc,))
        
        op = &sim.state.instructions[pc]
        print '%10s 0x%-*x' % (opcode_handlers[op.opcode_index].opcode_name,
                               sizeof(OPCODE_TYPE) * 2,
                               op.instruction)
    
    def do_info(self, about):
        """
        Show information about stuff:
            breakpoints
            registers
            ram
            instruction
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
                   'ram', 'register', 'pin', 'instruction')
        return self.complete_from_it(text, options)
    
    def do_set(self, args):
        '''
        set register name value
        set ram address value
        set pin address value
        '''
        cdef Simulator sim = self.simulator
        
        arglist = args.split()
        if len(arglist) != 3:
            raise ErrorMessage('Usage: ' + self.do_set.__doc__)
        
        type, addr, value = arglist
        vm_info_type = self.vm_info_type.get(type)
        if vm_info_type is None:
            raise ErrorMessage('Invalid argument: %r' % args)
        
        if vm_info_type == VM_INFO_REGISTER:
            addr = self.registers[addr]
        
        value = int(value, 0)
        
        if not vm_write(sim.state, NULL, vm_info_type, addr, value):
            raise VMError()
        
    def complete_set(self, text, line, beginidx, endidx):
        return self.complete_from_it(text, ('register', 'ram', 'pin'))
    
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
    
    def cmdloop(self, banner):
        while True:
            try:
                super(SimulatorCLI, self).cmdloop(banner)
            except (KeyboardInterrupt, ErrorMessage), e:
                sys.stderr.write(str(e) + '\n')
            
            banner = ''
    
    def check_hit_breakpoint(self, hit_bp):
        if hit_bp:
            print 'We hit a breakpoint:',
            self.info_instruction(self.simulator, '')
            # self.info_register(self.simulator, 'PC')
    
    def print_err(self, msg=None):
        if msg is None:
            msg = vm_strerror(-1)
        raise ErrorMessage(msg + "\n")


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
    if not <int> vm_register_interrupt_callable(
                    sim.state, <void *> &python_callback, <void *> callback):
        raise VMError()


INTERRUPT_TIMER = VM_INTERRUPT_TIMER

def specify_vm_interrupt(Simulator sim, interrupt_type, argument):
    vm_interrupt(sim.state, interrupt_type, <unsigned int> argument)