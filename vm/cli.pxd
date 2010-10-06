
cdef extern from "vm.h":
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
    
    ctypedef struct VMInterruptItem:
        VMInterruptType interrupt_type
        void *extra_arg
        VMInterruptItem *next

    ctypedef struct VMState
    ctypedef struct VMSingleStateDiff:
        size_t oldval
        size_t newval
        VMInfoType type
        size_t location
        VMSingleStateDiff *next

    ctypedef struct VMStateDiff:
        VMSingleStateDiff *singlediff
        VMStateDiff *next
            
    VMState *vm_newstate(void *instructions, VMInterruptPolicy interrupt_policy)
    VMStateDiff *vm_newdiff()
    void vm_acquire_interrupt_queue(VMState *)
    void vm_release_interrupt_queue(VMState *)
    int vm_break(VMState *state, size_t code_offset)
    void vm_cont(VMState *state, VMStateDiff *diffs)
    void vm_rcont(VMState *state, VMStateDiff *diffs)
    void vm_step(VMState *state, int nsteps, VMStateDiff *diffs)
    void vm_rstep(VMState *state, int nsteps, VMStateDiff *diffs)
    void vm_break_async(VMState *state)
    void vm_interrupt(VMState *state, VMInterruptType type, ...)
    int vm_info(VMState *state, VMInfoType type, size_t vmaddr)
    int vm_errno()
    char *vm_strerror(int vm_errno)