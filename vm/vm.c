#include "vm.h"

#define STRINGIFY(X) #X
#define LOCATION __FILE__ ":" STRINGIFY(__LINE__)
#define err(result, msg) _err((bool) (result), (msg))

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

static void
_err(bool result, char *msg)
{
	if (!result) {
		perror(msg);
		exit(EXIT_FAILURE);
	}
}

VMState *vm_newstate(void *instructions, VMInterruptPolicy interrupt_policy){
	VMState *newstate;
	err(newstate = malloc(sizeof(VMState)), "malloc " LOCATION);
	newstate->instructions = instructions;
	newstate->current_instruction = instructions;
	
	// ramsize is in bytes
	err(newstate->ram = malloc(ramsize), "malloc " LOCATION);
	
	// We make all registers ints
	err(newstate->registers = malloc(nregisters * sizeof(int)), 
		"malloc " LOCATION);
	newstate->interrupt_policy = interrupt_policy;
	newstate->interrupt_queue = NULL;
	
	#ifdef VM_WITH_THREADS
	// TODO: on windows link with http://sourceware.org/pthreads-win32/
	err(pthread_mutex_init(&newstate->interrupt_queue_lock) != 0, 
		"pthread_mutex_init " LOCATION);
	#endif
	
	newstate->break_async = false;
	return newstate;
}

VMStateDiff *vm_newdiff(void){
	VMStateDiff *newdiff;
	err(newdiff = malloc(sizeof(VMStateDiff)), "malloc " LOCATION);
	newdiff->singlediff = NULL;
	newdiff->next = NULL;
	return newdiff;
}

void vm_step(VMState *state, int nsteps, VMStateDiff *diffs){
	while (nsteps > 0) {
		// Check for interrupt
		if (state->interrupt_queue != NULL) {
			switch (state->interrupt_policy) {
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
		} else {
			// Execute instruction
			instruction_handler(state,diffs);
		}
		nsteps--;
	}
}

int vm_info(VMState *state, VMInfoType type, size_t vmaddr){
	int result = 0;
	switch (type) {
		case VM_INFO_REGISTER:
			result = *(((int *) (state->registers)) + vmaddr);
			break;
		case VM_INFO_RAM:
			result =  *(((unsigned char *) (state->ram)) + vmaddr);
			break;
		case VM_INFO_PIN:
			// FIXME: No way to access pin values in VMState
			result = 0;
			break;
	}
	return result;
}
