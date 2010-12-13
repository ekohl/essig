
cdef extern from "vm.h":
    ctypedef enum bool:
        true
        false

    ctypedef long BIGTYPE
    ctypedef int OPCODE_TYPE
    ctypedef OPCODE_TYPE PC_TYPE
    
    ctypedef enum VMInterruptPolicy:
        VM_POLICY_INTERRUPT_NEVER
        VM_POLICY_INTERRUPT_AVOID
        VM_POLICY_INTERRUPT_ALWAYS
        VM_POLICY_INTERRUPT_RANDOM
        VM_POLICY_INTERRUPT_INTERACTIVE
        VM_POLICY_INTERRUPT_SCRIPTED
    
    ctypedef enum VMInterruptType:
        VM_INTERRUPT_TIMER

    ctypedef enum VMInfoType:
        VM_INFO_REGISTER
        VM_INFO_RAM
        VM_INFO_IO
        VM_INFO_CHUNK
    
    ctypedef struct VMIterable:
        VMIterable *next
    
    ctypedef struct VMInterruptItem:
        VMInterruptItem *next
        VMInterruptType interrupt_type
        void *extra_arg
    
    ctypedef struct VMSingleStateDiff:
        VMSingleStateDiff *next
        size_t oldval
        VMInfoType type
        size_t location

    ctypedef struct VMStateDiff:
        VMStateDiff *next
        VMSingleStateDiff *singlediff
        
    ctypedef struct VMBreakpoint:
        VMBreakpoint *next
        size_t offset
    
    ctypedef struct OpcodeHandler:
        char *opcode_name
        OPCODE_TYPE opcode
    
    ctypedef struct Opcode:
        int opcode_index
        unsigned int instruction

    ctypedef struct Register:
        char *name
        size_t offset

    ctypedef struct VMInterruptCallable:
        pass

    ctypedef struct VMState:
        Opcode *instructions
        size_t instructions_size
        size_t executable_segment_offset
        VMBreakpoint *breakpoints
        unsigned int cycles
        unsigned char *chunk
        # OPCODE_TYPE *ram
        # OPCODE_TYPE *registers
        # OPCODE_TYPE *pins
        bint stopped_running
        VMInterruptCallable *interrupt_callables
    
    BIGTYPE GETPC(VMState *state)
    void SETPC(VMState *state, BIGTYPE pc)
    Opcode *get_opcode(VMState *state, PC_TYPE pc)
    
    cdef extern OpcodeHandler opcode_handlers[]
    cdef extern Register registers[]
    cdef extern int nregisters
    cdef extern unsigned long RAM_OFFSET
    cdef extern unsigned long RAM_END
    
    VMState *vm_newstate(void *instructions, 
                         size_t instructions_size, 
                         VMInterruptPolicy interrupt_policy)
    VMStateDiff *vm_newdiff()
    void vm_closestate(VMState *)
    void vm_closediff(VMStateDiff *)
    bint vm_break(VMState *state, size_t code_offset)
    bint vm_cont(VMState *state, VMStateDiff **diff, bint *hit_bp)
    void vm_rcont(VMState *state, VMStateDiff **diff, bint *hit_bp)
    bint vm_step(VMState *state, unsigned long nsteps, VMStateDiff **diff,
                 bint *hit_bp)
    bint vm_run(VMState *state, VMStateDiff **diff, bint *hit_bp)
    void vm_rstep(VMState *state, unsigned long nsteps, VMStateDiff **diff,
                  bint *hit_bp)
    void vm_break_async_from_signal(VMState *state)
    void vm_break_async_from_thread(VMState *state)
    bint vm_interrupt(VMState *state, VMInterruptType type, ...)
    BIGTYPE vm_info(VMState *state, VMInfoType type, size_t vmaddr, 
                    bool *errorp)
    
    void _print_diff(VMState *state, VMStateDiff *diff)

    bool vm_write(VMState *state, VMStateDiff *diff, VMInfoType type, 
                  size_t destaddr, OPCODE_TYPE value)
    
    void Py_INCREF(object obj)
    bool vm_register_interrupt_callable(VMState *state, void *func, 
                                        void *argument)
    
    
    int vm_errno()
    char *vm_strerror(int vm_errno)
