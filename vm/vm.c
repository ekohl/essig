#include "vm.h"

#define OPCODE_SIZE (nbits_cpu / 8)
#define OPCODE(state) (state->instructions + (state->pc / OPCODE_SIZE))

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
        goto error; \
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


/* iterable functions */

static void
_vm_close_iterable(VMIterable *it)
{
	VMIterable *tmp;
    
	while (it) {
		tmp = it;
		free(it);
		it = tmp->next;
	}
}

static VMIterable *
_vm_append_item(VMIterable *it, VMIterable *item)
{
	VMIterable *begin = it;
	if (!it) {
		return item;
	}
    
	while (it) {
		it = it->next;
	}
	it->next = item;
	return begin;
}

/* breakpoint checking */
bool
_hit_breakpoint(VMState *state)
{
	while (state->breakpoints)
		if (state->breakpoints->offset == state->pc)
			return true;
		
	return false;
}

/* allocation functions */
VMState *
vm_newstate(void *instructions, 
            size_t instructions_size, 
            VMInterruptPolicy interrupt_policy)
{
	VMState *newstate = NULL;
	err_malloc(newstate = calloc(1, sizeof(VMState)));
	newstate->instructions = instructions;
	newstate->pc = 0;
	
	// ramsize is in bytes
	err_malloc(newstate->ram = calloc(ramsize, 1));
	
#ifdef VM_WITH_THREADS
	err(pthread_mutex_init(&newstate->interrupt_queue_lock, NULL) == 0,
		"pthread_mutex_init",
		VM_OSERROR);
#endif
    
	// We make all registers ints
	err_malloc(newstate->registers = calloc(nregisters, sizeof(int)));
	newstate->interrupt_policy = interrupt_policy;
	newstate->interrupt_queue = NULL;
	newstate->break_async = false;
    newstate->breakpoints = NULL;
	return newstate;
error:
	vm_closestate(newstate);
	return NULL;
}

VMStateDiff *
vm_newdiff(void)
{
	VMStateDiff *newdiff = NULL;
	err_malloc(newdiff = calloc(1, sizeof(VMStateDiff)));
	return newdiff;
error:
	vm_closediff(newdiff);
	return NULL;
}

/* deallocation functions */
void 
vm_closestate(VMState *state) 
{
	if (state) {
		free(state->instructions);
		free(state->ram);
		free(state->registers);
#ifdef VM_WITH_THREADS
		/* reference counting? Oh well... */
		(void) pthread_mutex_destroy(&state->interrupt_queue_lock);
#endif
		_vm_close_iterable((VMIterable *) state->interrupt_queue);
		free(state);
	}
}

void 
vm_closediff(VMStateDiff *diff) 
{
	if (diff) {
		_vm_close_iterable((VMIterable *) diff);
		free(diff);
	}
}


/* control functions */

bool 
vm_step(VMState *state, int nsteps, VMStateDiff *diffs, bool *hit_bp)
{
	Opcode *opcode;
	opcode_handler *handler;
	
	*hit_bp = false;
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
		} else if (_hit_breakpoint(state)) {
			*hit_bp = true;
			break;
		} else {
			// Execute instruction
			opcode = OPCODE(state);
			handler = opcode_handlers[opcode->opcode_index].handler;
			if (!handler(state, diffs, opcode->opcode))
				return false;
		}
		nsteps--;
	}
    return true;
}

bool
vm_cont(VMState *state, VMStateDiff *diffs, bool *hit_bp)
{
    while (true) {
        if(!vm_step(state, 1000, diffs, hit_bp))
            return false;
        if (hit_bp)
            break;
    }
    return true;
}

bool 
vm_break(VMState *state, size_t code_offset) 
{
	VMBreakpoint *bp = NULL;
	err_malloc(bp = calloc(1, sizeof(VMBreakpoint)));
	/* prepend breakpoint */
	bp->next = (VMIterable *) state->breakpoints;
	bp->offset = code_offset;
	state->breakpoints = bp;
	return true;
error:
	free(bp);
	return false;
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
	if (err == -1)
		err = _vm_errno;
	
	if (err == VM_OSERROR)
		return strerror(errno);
	
	return _vm_error_messages[err];
}

Opcode *disassemble(unsigned int *assembly, size_t assembly_length) {
	Opcode *result = NULL;
	size_t i, j;

	err_malloc(result = malloc(sizeof(Opcode) * assembly_length));
	// Walk though assembly
	for (i = 0; i < assembly_length; ++i) {
		// Walk through opcode handlers
		for (j = 0; j < n_opcode_handlers; ++j) {
			// If the opcode matches an opcode handlers opcode, we found our handler
			if ((assembly[i] & opcode_handlers[j].mask) == opcode_handlers[j].opcode) {
				result->opcode_index = j;
				result->opcode = assembly[i];
				break;
			}
		}
	}

	return result;
error:
	free(result);
	return NULL;
}
