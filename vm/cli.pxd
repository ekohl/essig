
cdef extern from "vm.h":
    ctypedef enum bool:
        true
        false

    ctypedef int OPCODE_TYPE
    
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
        VMIterable *next
        VMInterruptType interrupt_type
        void *extra_arg
    
    ctypedef struct VMSingleStateDiff:
        VMIterable *next
        size_t oldval
        size_t newval
        VMInfoType type
        size_t location

    ctypedef struct VMStateDiff:
        VMIterable *next
        VMSingleStateDiff *singlediff
        
    ctypedef struct VMBreakpoint:
        VMIterable *next
        size_t offset
    
    ctypedef struct VMState:
        VMBreakpoint *breakpoints
        size_t pc
    
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
    bint vm_info(VMState *state, VMInfoType type, size_t vmaddr, 
                 OPCODE_TYPE *result)
    int vm_errno()
    char *vm_strerror(int vm_errno)
