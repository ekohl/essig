#include <elf.h>
#include "vm.h"

#define GETPC(state) state->registers[PC]
#define OPCODE(state) (state->instructions + GETPC(state))

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
        vm_seterrno(errno); \
        goto error; \
    }

#define err_malloc(result) err(result, "malloc", VM_NO_MEMORY)

static bool _read_elf(VMState *state, char *program, size_t program_size);

static char *_vm_error_messages[] = { 
#   define __vm_errno__(a,b) b,
#   include "vmerrors.h"
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

void
vm_seterrno(int err)
{
    _vm_errno = err;
}

/* iterable functions */

static void
_vm_close_iterable(VMIterable *it)
{
    VMIterable *tmp;
    
    while (it) {
        tmp = it->next;
        free(it);
        it = tmp;
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
    VMBreakpoint *bp;
    
    bp = state->breakpoints;
    while (bp) {
        if (bp->offset == state->registers[PC])
            return true;
        
        bp = bp->next;
    }
    
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
    err_malloc(newstate->registers = calloc(nregisters, sizeof(OPCODE_TYPE)));
    err_malloc(newstate->pins = calloc(npins, sizeof(OPCODE_TYPE)));
    newstate->interrupt_policy = interrupt_policy;
    newstate->break_async = false;
    
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
        _vm_close_iterable((VMIterable *) state->interrupt_callables);
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
        tmp = tmp->next;
    }
    
    _vm_close_iterable((VMIterable *) diff);
}


/* control functions */

static bool 
_vm_step(VMState *state, int nsteps, VMStateDiff **diff, bool *hit_bp, bool first)
{
    Opcode *opcode;
    opcode_handler *handler;
    VMInterruptCallable *callable;
    VMInterruptItem *interrupt_item, *previous_interrupt_item = NULL;
    VMStateDiff *newdiff = NULL;
    
#define RETURN(x) do { RELEASE_STATE(state); return (x); } while(0)
    
    *hit_bp = false;
#if 0
    printf("%-20s %-20s %-20s\n", "Opcode", "Program Counter", "Instruction");
#endif
    while (nsteps > 0) {
        /* acquire and release for every step. This allows for some nice 
           contention! */
        ACQUIRE_STATE(state);
        if (state->break_async) {
            RETURN(true);
        }
        
        callable = state->interrupt_callables;
        while (callable) {
            if (!callable->func(state, callable->argument)) {
                vm_seterrno(VM_INTERRUPT_CALLABLE_ERROR);
                RETURN(false);
            }
            callable = callable->next;
        }
        
        if (false || state->interrupts != NULL) {
            // /* interrupt */
            // interrupt_item = state->interrupts;
            // while (interrupt_item) {
                // if (interrupt_item->cycles <= state->cycles) {
                    // bool result; 
                    // interrupt_handler *handler;
                    // 
                    // handler = state->interrupt_handlers[
                                            // interrupt_item->interrupt_type];
                    // result = handler(state, interrupt_item->interrupt_type);
                    // 
                    // /* delete item from the queue */
                    // if (previous_interrupt_item) {
                        // previous_interrupt_item->next = interrupt_item->next;
                    // } else {
                        // state->interrupts = interrupt_item->next;
                    // }
                    // free(interrupt_item);
                    // interrupt_item = previous_interrupt_item;
                    // 
                    // if (!result)
                        // return false;
                // }
                // interrupt_item = interrupt_item->next;
                // 
            // }
        } else if (!first && _hit_breakpoint(state)) {
            /* breakpoint */
            *hit_bp = true;
            RETURN(true);
        } else {
            
            /* PC error checking */
            {
                /* offsets in bytes */
                size_t pc;
                
                pc = GETPC(state);
                
                if (pc < state->executable_segment_offset || 
                    pc >= state->instructions_size) {
                    vm_seterrno(VM_PC_OUT_OF_BOUNDS);
#ifdef VM_DEBUG
                    printf(LOCATION " pc: %lu max pc: %lu\n", 
                           (unsigned long) pc, 
                           (unsigned long) state->instructions_size - 1);
#endif
                    RETURN(false);
                }
            }
            
            /* Save changes in diff */
            if (diff) {
                if (!(newdiff = vm_newdiff()))
                    RETURN(false);
                
                newdiff->next = *diff;
                newdiff->pc = state->registers[PC];
                newdiff->cycles = state->cycles;
                
                *diff = newdiff;
            }
            
            /* Execute instruction */
            opcode = (Opcode *) OPCODE(state);
            handler = opcode_handlers[opcode->opcode_index].handler;
#if 0
            printf("%-20s 0x%-18x 0x%-18x\n", 
                   opcode_handlers[opcode->opcode_index].opcode_name,
                   state->registers[PC],
                   opcode->instruction);
            /*printf("0x%x\r", GETPC(state));
            fflush(stdout);*/
#endif
            
            if (!handler(state, newdiff, opcode->instruction)) {
                RETURN(state->stopped_running);
            }
        }
        nsteps--;
        first = false;
        RELEASE_STATE(state);
    }
    
    return true;
#undef RETURN
}

bool 
vm_step(VMState *state, int nsteps, VMStateDiff **diff, bool *hit_bp)
{
    return _vm_step(state, nsteps, diff, hit_bp, true);
}

static OPCODE_TYPE *
_get_location(VMState *state, VMInfoType type, size_t addr)
{
    OPCODE_TYPE *location = NULL;

    switch (type) {
        case VM_INFO_REGISTER:
            location = state->registers;
            if (addr >= nregisters)
                goto error;
            break;
        case VM_INFO_RAM:
            location = state->ram;
            if (addr >= ramsize)
                goto error;
            break;
        case VM_INFO_PIN:
            location = state->pins + pinoffset;
            if (addr >= npins)
                goto error;
            break;
    }
    
    return location + addr;

error:
    vm_seterrno(VM_OUT_OF_BOUNDS_ERROR);
    return NULL;
}

#ifdef VM_DEBUG

static void
_print_diff(VMState *state, VMStateDiff *diff)
{
    char *fmt =        "    %-20s: %lu -> %lu\n";
    char *fmt_single = "    %-10s at 0x%-04x: %lu -> %lu\n";
    
    VMSingleStateDiff *singlediff;
    
    puts("VMStateDiff {");
    
    printf(fmt, "PC", (unsigned long) state->registers[PC],
                      (unsigned long) diff->pc);
    printf(fmt, "Cycles", (unsigned long) state->cycles,
                          (unsigned long) diff->cycles);
    
    singlediff = diff->singlediff;
    while (singlediff) {
        char *info_type;
        switch (singlediff->type) {
            case VM_INFO_REGISTER:
                info_type = "Register";
                break;
            case VM_INFO_RAM:
                info_type = "Ram";
                break;
            case VM_INFO_PIN:
                info_type = "Pin";
                break;
        }
        printf(fmt_single, info_type, singlediff->location,
               (unsigned long) singlediff->oldval, 
               (unsigned long) singlediff->newval);
        
        singlediff = singlediff->next;
    }
    
    puts("}");
}

#endif /* VM_DEBUG */

static void
_vm_rstep(VMState *state, int nsteps, VMStateDiff **diff, bool *hit_bp,
          bool first)
{
    VMSingleStateDiff *singlediff;
    VMStateDiff *next;
    
    *hit_bp = false;
    state->stopped_running = false;
    
    while (nsteps-- && (*diff)->next) {
        if (!first && _hit_breakpoint(state)) {
            *hit_bp = true;
            break; 
        }
        
        first = false;
        
#if 0
        printf("Applying diff: ");
        _print_diff(state, *diff);
#endif
        
        state->cycles = (*diff)->cycles;
        state->registers[PC] = (*diff)->pc;
        
        singlediff = (*diff)->singlediff;
        /* Apply singlediff changes */
        while (singlediff) {
            OPCODE_TYPE *location;
            
            location = _get_location(state, singlediff->type,
                                     singlediff->location);
            *location = singlediff->oldval;
            
            singlediff = singlediff->next;
        }
        
        /* Deallocate diff */
        _vm_close_iterable((VMIterable *) (*diff)->singlediff);
        next = (*diff)->next;
        free(*diff);
        *diff = next;
    }
}

void
vm_rstep(VMState *state, int nsteps, VMStateDiff **diff, bool *hit_bp)
{
    _vm_rstep(state, nsteps, diff, hit_bp, true);
}

static bool
_vm_cont(VMState *state, VMStateDiff **diff, bool *hit_bp, bool first)
{
    while (true) {
        if (!_vm_step(state, 1000, diff, hit_bp, first))
            return false;
        if (*hit_bp || state->stopped_running)
            break;
        
        first = false;
    }
    return true;
}


bool
vm_cont(VMState *state, VMStateDiff **diff, bool *hit_bp)
{
    return _vm_cont(state, diff, hit_bp, true);
}


bool
vm_run(VMState *state, VMStateDiff **diff, bool *hit_bp)
{
    return _vm_cont(state, diff, hit_bp, false);
}


void
vm_rcont(VMState *state, VMStateDiff **diff, bool *hit_bp)
{
    bool first = true;
    
    /* Unwind until the first diff is met (which is unused), or until a
       breakpoint is met */
    while ((*diff)->next) {
        _vm_rstep(state, 1000, diff, hit_bp, first);
        if (*hit_bp)
            break;
        
        first = false;
    }
}

bool 
vm_break(VMState *state, size_t code_offset) 
{
    VMBreakpoint *bp = NULL;
    err_malloc(bp = calloc(1, sizeof(VMBreakpoint)));
    
    /* prepend breakpoint */
    bp->next = state->breakpoints;
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
            vm_seterrno(VM_NO_SUCH_INTERRUPT_TYPE_SUPPORT_ERROR);
            return false;
    }
    if(!item)
        return false;
    
    item->next = state->interrupts;
    state->interrupts = item;
    va_end(args);
    return true;
}

bool
vm_register_interrupt_callable(VMState *state, interrupt_callable *func, 
                               void *argument)
{
    VMInterruptCallable *callable;
    
    err_malloc(callable = calloc(1, sizeof(VMInterruptCallable)));
    callable->func = func;
    callable->argument = argument;
    /* prepend our struct in the linked list */
    callable->next = state->interrupt_callables;
    state->interrupt_callables = callable;
    return true;
error:
    free(callable);
    return false;
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
    
    if (diff) {
        /* update our diffs */
        err_malloc((singlediff = malloc(sizeof(VMSingleStateDiff))));
        singlediff->oldval = *dest;
        
#ifdef VM_DEBUG
        singlediff->newval = value;
#endif
        
        singlediff->type = type;
        singlediff->location = destaddr;
        
        singlediff->next = diff->singlediff;
        diff->singlediff = singlediff;
    }
    
    /* finally, write the value */
    *dest = value;
    return true;
    
error:
    return false;
}

static void
swap_bytes(char *value, size_t length)
{
    int i;
    char temp;
    
    for (i = 0; i < length / 2; ++i) {
        temp = value[i];
        value[i] = value[length - 1 - i];
        value[length - 1 - i] = temp;
    }
}

void 
vm_convert_to_host_endianness(char *value, size_t length)
{
    int val;
    char *valp;
    
    val = 1;
    valp = (char *) &val;
    if (valp[0] == 0) {
        /* Host is big endian */
        if (!is_big_endian)
            swap_bytes(value, length);
    } else {
        /* Host is little endian */
        if (is_big_endian)
            swap_bytes(value, length);
    }
}

long long
vm_convert_to_signed(unsigned long long value, int nbits)
{
    unsigned long long firstbit = 1 << (nbits - 1);
    
    if (value & firstbit) {
        /* first bit is set, convert to signed */
        /* remove the first bit */
        value = value & ~firstbit;
        /* compute: lowest_value + remaining_value */
        return -firstbit + value; 
    } else {
        /* no need to do conversion, the first bit is not set */
        return (long long) value;
    }
}

static Opcode *
disassemble(OPCODE_TYPE *assembly, size_t assembly_length) 
{
    Opcode *result = NULL;
    OpcodeHandler *op_handler;
    size_t i, j;
    bool some_error = false;
    
    err_malloc(result = malloc(sizeof(Opcode) * assembly_length));
    for (i = 0; i < assembly_length; ++i) {
        bool found = false, is_arg = false;
        char *name;
        OPCODE_TYPE instruction = assembly[i];
        
        vm_convert_to_host_endianness((char *) &instruction, sizeof(instruction));
        
        for (j = 0; opcode_handlers[j].opcode_name; ++j) {
            op_handler = &opcode_handlers[j];
            if ((assembly[i] & op_handler->mask) == op_handler->opcode) {
                result[i].opcode_index = j;
                result[i].instruction = assembly[i];
                found = true;
                name = op_handler->opcode_name;
                break;
            }
        }
        
        if (!found) {
            /* This is another (16-bits) argument to an instruction. Apparently
               no instruction handler could be found for the argument. We try
               to find a handler in any case to allow simulation of code that
               (incorrectly or maliciously) jumps to an argument. */
            result[i].opcode_index = 0; /* noop */
            result[i].instruction = assembly[i];
            name = "noop";
            
            if (!is_arg) {
                some_error = true;
#ifdef VM_DEBUG
                fprintf(
                    stderr,
                    LOCATION " Cannot handle instruction 0x%x at address "
                    "offset 0x%x.\n",
                    (unsigned int) assembly[i], 
                    (unsigned int) (i * sizeof(OPCODE_TYPE)));
#endif
                /* vm_seterrno(VM_ILLEGAL_INSTRUCTION); */
            }
        }
        
        is_arg = op_handler->next_is_arg;
    }

    if (some_error) {
        fprintf(stderr, "WARNING: Some opcodes are not implemented, "
                        "simulation might not work properly.\n");
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
        vm_seterrno(VM_NOT_ELF);
        return false;
    }
}
