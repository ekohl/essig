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

VMStateDiff *vm_newdiff(void){
	VMStateDiff *newdiff = (VMStateDiff *) malloc(sizeof(VMStateDiff));
	newdiff->singlediff = NULL;
	newdiff->next = NULL;
	return newdiff;
}

void vm_step(VMState *state, int nsteps, VMStateDiff *diffs){
	while(nsteps > 0){
		// Check for interrupt
		if(state->interrupt_queue != NULL){
			switch(state->interrupt_policy){
				case VM_POLICY_INTERRUPT_NEVER:
					// clean list
					break;
				case VM_POLICY_INTERRUPT_ALWAYS:
					interrupt_handler(state, diffs);
					break;
				default:
					//For now clean list
					break;
			}
		}else{
			// Execute instruction
			instruction_handler(state,diffs);
		}
		nsteps--;
	}
}
