
cdef extern from "vm.h":
    ctypedef enum bool:
        true
        false

    ctypedef int OPCODE_TYPE
    cdef extern OPCODE_TYPE PC
    cdef extern size_t ramsize
    
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
        VM_INFO_PIN
    
    ctypedef struct VMIterable:
        VMIterable *next
    
    ctypedef struct VMInterruptItem:
        VMInterruptItem *next
        VMInterruptType interrupt_type
        void *extra_arg
    
    ctypedef struct VMSingleStateDiff:
        VMSingleStateDiff *next
        size_t oldval
        size_t newval
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

    ctypedef struct VMState:
        Opcode *instructions
        size_t instructions_size
        size_t executable_segment_offset
        VMBreakpoint *breakpoints
        unsigned int cycles
        OPCODE_TYPE *ram
        OPCODE_TYPE *registers
        OPCODE_TYPE *pins
    
    cdef extern OpcodeHandler opcode_handlers[]
    cdef extern Register registers[]
    cdef extern int nregisters
    
    VMState *vm_newstate(void *instructions, 
                         size_t instructions_size, 
                         VMInterruptPolicy interrupt_policy)
    VMStateDiff *vm_newdiff()
    void vm_closestate(VMState *)
    void vm_closediff(VMStateDiff *)
    bint vm_break(VMState *state, size_t code_offset)
    bint vm_cont(VMState *state, VMStateDiff *diffs, bint *hit_bp)
    VMStateDiff *vm_rcont(VMState *state, VMStateDiff *diffs, bint *hit_bp)
    bint vm_step(VMState *state, int nsteps, VMStateDiff *diffs, bint *hit_bp)
    VMStateDiff *vm_rstep(VMState *state, int nsteps, VMStateDiff *diffs, 
                          bint *hit_bp)
    void vm_break_async_from_signal(VMState *state)
    void vm_break_async_from_thread(VMState *state)
    bint vm_interrupt(VMState *state, VMInterruptType type, ...)
    OPCODE_TYPE vm_info(VMState *state, VMInfoType type, size_t vmaddr, 
                        bool *result)
    
    void Py_INCREF(object obj)
    bool vm_register_interrupt_callable(VMState *state, void *func, 
                                        void *argument)
    
    
    int vm_errno()
    char *vm_strerror(int vm_errno)
