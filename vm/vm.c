#include "vm.h"

#define STRINGIFY(msg) #msg
#define TOSTRING(msg) STRINGIFY(msg)
#define LOCATION __FILE__ ":" TOSTRING(__LINE__)
#ifdef VM_DEBUG
#	define print_err(x) perror(x)
#else
#	define print_err(x)
#endif

#define err(result, msg, errno) \
    if (!(bool) (result)) { \
        print_err(LOCATION " " msg); \
        _vm_errno = errno; \
        return NULL; \
    }

#define err_malloc(result) err(result, "malloc", VM_NO_MEMORY)

enum _vmerrno {
#	define __vm_errno__(a,b) a,
#	include "vmerrno.h"
#	undef __vm_errno__
	VM_ERROR_NUM
};

static char *_vm_error_messages[] = { 
#	define __vm_errno__(a,b) b,
#	include "vmerrno.h"
#	undef __vm_errno__
};

__thread int _vm_errno = 0;

void interrupt_handler(VMState *state, VMStateDiff *diffs) {

}

VMState *
vm_newstate(void *instructions, 
            size_t instructions_size, 
            VMInterruptPolicy interrupt_policy)
{
	VMState *newstate;
	err_malloc(newstate = malloc(sizeof(VMState)));
	newstate->instructions = instructions;
	newstate->current_instruction = instructions;
	
	// ramsize is in bytes
	err_malloc(newstate->ram = malloc(ramsize));
	
	// We make all registers ints
	err_malloc(newstate->registers = malloc(nregisters * sizeof(int)));
	newstate->interrupt_policy = interrupt_policy;
	newstate->interrupt_queue = NULL;
	
#	ifdef VM_WITH_THREADS
	// TODO: on windows link with http://sourceware.org/pthreads-win32/
	err(pthread_mutex_init(&newstate->interrupt_queue_lock, NULL) == 0, 
		"pthread_mutex_init",
        VM_OSERROR);
#	endif
	
	newstate->break_async = false;
    newstate->breakpoints = NULL;
	return newstate;
}

VMStateDiff *vm_newdiff(void){
	VMStateDiff *newdiff;
	err_malloc(newdiff = malloc(sizeof(VMStateDiff)));
	newdiff->singlediff = NULL;
	newdiff->next = NULL;
	return newdiff;
}

void vm_closestate(VMState *state) {
    
}

void vm_closediff(VMStateDiff *diff) {
    
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

int vm_break(VMState *state, size_t code_offset) {
	
	return 0;
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

int vm_errno(void) {
	int tmp = _vm_errno;
	_vm_errno = VM_NO_ERROR;
	return tmp;
}

char *vm_strerror(int err) {
	if (err == VM_OSERROR) {
		return strerror(errno);
	} else {
		if (err == -1)
			err = _vm_errno;
		return _vm_error_messages[err];
	}
}
