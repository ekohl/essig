#include <elf.h>
#include "vm.h"

#define OPCODE_SIZE (nbits_cpu / 8)
#define OPCODE(state) (state->instructions + (state->pc / OPCODE_SIZE))

#define STRINGIFY(msg) #msg
#define TOSTRING(msg) STRINGIFY(msg)
#define LOCATION __FILE__ ":" TOSTRING(__LINE__)
#if VM_DEBUG
#   define print_err(x) puts(x)
#else
#   define print_err(x)
#endif

#define err(result, msg, errno) \
    if (!(bool) (result)) { \
        print_err(LOCATION " " msg); \
        _vm_errno = errno; \
        goto error; \
    }

#define err_malloc(result) err(result, "malloc", VM_NO_MEMORY)

static bool _read_elf(VMState *state, char *program, size_t program_size);

enum _vmerrno {
#   define __vm_errno__(a,b) a,
#   include "vmerrno.h"
#   undef __vm_errno__
    VM_ERROR_NUM
};

static char *_vm_error_messages[] = { 
#   define __vm_errno__(a,b) b,
#   include "vmerrno.h"
#   undef __vm_errno__
};

__thread int _vm_errno = 0;

/* error functions */
int 
vm_errno(void)
{
    int tmp = _vm_errno;
    _vm_errno = VM_NO_ERROR;
    return tmp;
}

char *
vm_strerror(int err) 
{
    if (err == -1)
        err = _vm_errno;
    
    if (err == VM_OSERROR)
        return strerror(errno);
    
    return _vm_error_messages[err];
}

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

VMIterable *
vm_reversed_it(VMIterable *it)
{
    VMIterable *previous = NULL;
    VMIterable *next;
    
    while (it) {
        next = it->next;
        it->next = previous;
        previous = it;
        it = next;
    }
    
    return it;
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
vm_newstate(void *program, 
            size_t program_size,
            VMInterruptPolicy interrupt_policy)
{
    VMState *newstate = NULL;
    err_malloc(newstate = calloc(1, sizeof(VMState)));
    
    // ramsize is in bytes
    err_malloc(newstate->ram = calloc(ramsize, 1));
    
#ifdef VM_WITH_THREADS
    err(pthread_mutex_init(&newstate->lock, NULL) == 0,
        "pthread_mutex_init",
        VM_OSERROR);
#endif
    
    // We make all registers ints
    err_malloc(newstate->registers = calloc(nregisters, sizeof(int)));
    newstate->interrupt_policy = interrupt_policy;
    newstate->interrupts = NULL;
    newstate->break_async = false;
    newstate->breakpoints = NULL;
    
    if (!_read_elf(newstate, program, program_size))
        goto error;
    
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

/*! Create a new interrupt item */
static VMInterruptItem *
vm_new_interrupt_item(VMInterruptType interrupt_type, unsigned int cycles)
{
    VMInterruptItem *result = NULL;
    
    err_malloc(result = calloc(1, sizeof(VMInterruptItem)));
    result->interrupt_type = interrupt_type;
    result->cycles = cycles;
    return result;
error:
    free(result);
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
        (void) pthread_mutex_destroy(&state->lock);
#endif
        _vm_close_iterable((VMIterable *) state->breakpoints);
        _vm_close_iterable((VMIterable *) state->interrupts);
        free(state);
    }
}

void 
vm_closediff(VMStateDiff *diff) 
{
    VMStateDiff *tmp;
    
    tmp = diff;
    while (tmp) {
        _vm_close_iterable((VMIterable *) tmp->singlediff);
        tmp = (VMStateDiff *) tmp->next;
    }
    
    _vm_close_iterable((VMIterable *) diff);
}


/* control functions */

bool 
vm_step(VMState *state, int nsteps, VMStateDiff *diff, bool *hit_bp)
{
    Opcode *opcode;
    opcode_handler *handler;
    VMStateDiff *newdiff;
    VMIterable *interrupt_item, *previous_interrupt_item = NULL;
    
    *hit_bp = false;
    while (nsteps > 0) {
        /* acquire and release for every step. This allows for some nice 
           contention! */
        ACQUIRE_STATE(state);
        if (state->break_async) {
            *hit_bp = true;
            RELEASE_STATE(state); 
            break;
        }
        
        if (state->interrupts != NULL) {
            /* interrupt */
            interrupt_item = (VMIterable *) state->interrupts;
            while (interrupt_item) {
                VMInterruptItem *item; 
                
                item = (VMInterruptItem *) interrupt_item; 
                if (item->cycles == state->cycles) {
                    bool result; 
                    interrupt_handler *handler;
                    
                    handler = state->interrupt_handlers[item->interrupt_type];
                    result = handler(state, item->interrupt_type);
                    
                    /* delete item from the queue */
                    if (previous_interrupt_item) {
                        previous_interrupt_item->next = interrupt_item->next;
                    } else {
                        state->interrupts = 
                            (VMInterruptItem *) interrupt_item->next;
                    }
                    free(interrupt_item);
                    interrupt_item = previous_interrupt_item;
                    
                    if (!result)
                        return false;
                }
                interrupt_item = interrupt_item->next;
            }
        } else if (_hit_breakpoint(state)) {
            /* breakpoint */
            *hit_bp = true;
             RELEASE_STATE(state);
            break;
        } else {
            /* Save PC */
            newdiff = vm_newdiff();
            if (!newdiff)
                return false;
            newdiff->pc = state->pc;
            newdiff->cycles = state->cycles;
            diff->next = (VMIterable *) newdiff;
            diff = newdiff;
            
            /* Execute instruction */
            opcode = OPCODE(state);
            handler = opcode_handlers[opcode->opcode_index].handler;
            if (!handler(state, diff, opcode->instruction))
                return false;
        }
        nsteps--;
        
        RELEASE_STATE(state);
    }
    return true;
}

static OPCODE_TYPE *
_get_location(VMState *state, VMInfoType type, size_t addr)
{
    OPCODE_TYPE *location = NULL;

    switch (type) {
        case VM_INFO_REGISTER:
            location = state->registers;
            if (addr > nregisters)
                goto error;
            break;
        case VM_INFO_RAM:
            location = state->ram;
            if (addr > ramsize)
                goto error;
            break;
        case VM_INFO_PIN:
            location = state->ram + pinoffset;
            if (addr > npins)
                goto error;
            break;
    }
    
    return location + addr;

error:
    _vm_errno = VM_OUT_OF_BOUNDS_ERROR;
    return NULL;
}

VMStateDiff *
vm_rstep(VMState *state, int nsteps, VMStateDiff *diff, bool *hit_bp)
{
    VMSingleStateDiff *singlediff;
    VMStateDiff *next;
    
    *hit_bp = false;
    
    while (nsteps--) {
        state->cycles = diff->cycles;
        state->pc = diff->pc;
        
        singlediff = diff->singlediff;
        
        while (singlediff) {
            OPCODE_TYPE *location;
            
            location = _get_location(state, singlediff->type, 
                                     singlediff->location);
            *location = singlediff->oldval;
            
            singlediff = (VMSingleStateDiff *) singlediff->next;
        }
        /* unwind the diff list */
        _vm_close_iterable((VMIterable *) diff->singlediff);
        next = (VMStateDiff *) diff->next;
        free(diff);
        diff = next;
    }
    return diff;
}

bool
vm_cont(VMState *state, VMStateDiff *diff, bool *hit_bp)
{
    while (true) {
        if (!vm_step(state, 1000, diff, hit_bp))
            return false;
        if (*hit_bp)
            break;
    }
    return true;
}

VMStateDiff *
vm_rcont(VMState *state, VMStateDiff *diff, bool *hit_bp)
{
    diff = (VMStateDiff *) vm_reversed_it((VMIterable *) diff);
    
    while (true) {
        diff = vm_rstep(state, 1000, diff, hit_bp);
        if (*hit_bp)
            break;
    }
    
    (void) vm_reversed_it((VMIterable *) diff);
    return diff;
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

void
vm_break_async_from_signal(VMState *state)
{
    state->break_async = true;
}

void
vm_break_async_from_thread(VMState *state)
{
    ACQUIRE_STATE(state);
    state->break_async = true;
    RELEASE_STATE(state);
}


bool 
vm_interrupt(VMState *state, VMInterruptType type, ...)
{
    va_list args;
    VMInterruptItem *item;
    unsigned int cycles;
    
    if (state->interrupt_policy == VM_POLICY_INTERRUPT_NEVER)
            return true;
    
    va_start(args, type);
    item = NULL;
    
    switch (type) {
        case VM_INTERRUPT_TIMER:
            cycles = va_arg(args, unsigned int);
            item = vm_new_interrupt_item(VM_INTERRUPT_TIMER, 
                                         state->cycles + cycles);
            break;
        default:
            _vm_errno = VM_NO_SUCH_INTERRUPT_TYPE_SUPPORT_ERROR;
            return false;
    }
    if(!item)
        return false;
    
    item->next = (VMIterable *) state->interrupts;
    state->interrupts = item;
    va_end(args);
    return true;
}

void
vm_register_handler(VMState *state, 
                    VMInterruptType type, 
                    interrupt_handler *handler)
{
    state->interrupt_handlers[type] = handler;
}

OPCODE_TYPE
vm_info(VMState *state, VMInfoType type, size_t vmaddr, bool *errorp)
{
    OPCODE_TYPE *dest;
    
    if (!(dest = _get_location(state, type, vmaddr))) {
        if (errorp)
            *errorp = true;
        
        return 0;
    }
    
    return *dest;
}

bool
vm_write(VMState *state, VMStateDiff *diff, VMInfoType type, 
         size_t destaddr, OPCODE_TYPE value)
{
    OPCODE_TYPE *dest; 
    VMSingleStateDiff *singlediff;
    
    if (!(dest =_get_location(state, type, destaddr)))
        goto error;
    
    /* update our diffs */
    err_malloc((singlediff = malloc(sizeof(VMSingleStateDiff))));
    singlediff->oldval = *dest;
    singlediff->newval = value;
    singlediff->type = type;
    singlediff->location = destaddr;
    
    singlediff->next = (VMIterable *) diff->singlediff;
    diff->singlediff = singlediff;
    
    /* finally, write the value */
    *dest = value;
    return true;
    
error:
    return false;
}

static Opcode *
disassemble(OPCODE_TYPE *assembly, size_t assembly_length) 
{
    Opcode *result = NULL;
    OpcodeHandler *op_handler;
    size_t i, j;
    
    
    err_malloc(result = malloc(sizeof(Opcode) * assembly_length));
    for (i = 0; i < assembly_length; ++i) {
        bool found = false, is_arg = false;
        char *name;
        
        if (is_arg) {
            /* Not an actual opcode, but an argument to another opcode 
               (e.g. ld, st) */
            result[i].opcode_index = 0; /* nop */
            result[i].instruction = assembly[i];
            name = "nop";
        } else {
            /* Find the right opcode handler for the current instruction and */
            for (j = 0; j < n_opcode_handlers; ++j) {
                op_handler = &opcode_handlers[j];
                if ((assembly[i] & op_handler->mask) == op_handler->opcode) {
                    result[i].opcode_index = j;
                    result[i].instruction = assembly[i];
                    found = true;
                    name = op_handler->opcode_name;
                    break;
                }
            }
        }
        
        if (!found && !is_arg) {
#ifdef VM_DEBUG
            printf(
                LOCATION " Cannot handle instruction 0x%x at address "
                "offset 0x%x.\n",
                (unsigned int) assembly[i], i * sizeof(OPCODE_TYPE));
#endif
            _vm_errno = VM_ILLEGAL_INSTRUCTION;
            return false;
        }
        
        /* For now, use this. Later, rely on opcode_handler->next_is_arg. */
        is_arg = (strcmp(name, "ld")   == 0 ||
                  strcmp(name, "st")   == 0 ||
                  strcmp(name, "lpm")  == 0 ||
                  strcmp(name, "elpm") == 0);
        // is_arg = op_handler->next_is_arg;                  
    }

    return result;
error:
    free(result);
    return NULL;
}

#include "readelf.c"

#define _elf32_read _elf64_read
#define Elf32_Ehdr Elf64_Ehdr
#define Elf32_Phdr Elf64_Phdr
#   include "readelf.c"
#undef _elf32_read
#undef Elf32_Ehdr
#undef Elf32_Phdr

/* Parse an ELF file.
See http://www.skyfree.org/linux/references/ELF_Format.pdf for a description
of the ELF format. */
static bool
_read_elf(VMState *state, char *program, size_t program_size)
{
    Elf32_Ehdr *ehdr;
    char elfclass;

    ehdr = (Elf32_Ehdr *) program;
    if (ehdr->e_ident[EI_MAG0] == 0x7f &&
        ehdr->e_ident[EI_MAG1] == 'E' &&
        ehdr->e_ident[EI_MAG2] == 'L' &&
        ehdr->e_ident[EI_MAG3] == 'F' &&
        (elfclass = ehdr->e_ident[EI_CLASS]) != ELFCLASSNONE) 
    {
        /* valid ELF file */
        if (elfclass == ELFCLASS32)
            return _elf32_read(state, program, program_size);
        else
            return _elf64_read(state, program, program_size);
    } else {
        _vm_errno = VM_NOT_ELF;
        return false;
    }
}
