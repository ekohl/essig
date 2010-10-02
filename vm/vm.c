#include "vm.h"

enum _vmerrno {
    #define __vm_errno__(a,b) a,
    #include "vmerrno.h"
    #undef __vm_errno__
    VM_ERROR_NUM
};

static char *vm_error_messages[] = { 
    #define __vm_errno__(a,b) b,
    #include "vmerrno.h"
    #undef __vm_errno__
};

VMState *vm_newstate(void *instructions, VMInterruptPolicy interrupt_policy){
	VMState *newstate = (VMState *) malloc(sizeof(VMState));
	newstate->instructions = instructions;
	newstate->current_instruction = instructions;
	newstate->ram = (void *) malloc(ramsize);
	newstate->registers = (void *) malloc(nregisters);
	newstate->interrupt_policy = interrupt_policy;
	newstate->interrupt_queue = NULL;
	#ifdef VM_WITH_THREADS
	// TODO: initialize queue lock here
	#endif
	newstate->break_async = false;
	return newstate;
}
