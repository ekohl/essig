#include <stdio.h>
#include <stdlib.h>

#include "vm.h"

#define check_error(expr) \
    if (!(expr)) { \
        fprintf(stderr, "Error: "); \
        fputs(vm_strerror(-1), stderr); \
        exit(EXIT_FAILURE); \
    }

int
get_index(char *op_name)
{
    int index;
    
    for (index = 0; opcode_handlers[index].opcode_name; index++) {
        if (strcmp(opcode_handlers[index].opcode_name, op_name) == 0)
            return index;
    }
    
    return -1;
}

void
put_code(VMState *state)
{
    Opcode *opcodes;
    
    err_malloc(opcodes = malloc(sizeof(Opcode) * 2));
    opcodes[0].opcode_index = 0; /* noop */
    opcodes[0].instruction = 0;
    opcodes[1].opcode_index = get_index("rjmp");
    opcodes[1].instruction = 0b1100111111111110; /* rjmp -2 */
    
    state->instructions = opcodes;
    state->instructions_size = 2;
    
    return;
error:
    check_error(false);
}

int
main(int argc, char **argv)
{
    char *endptr;
    unsigned long nsteps = 1000000;
    VMState *state;
    bool hit_bp = false;
#ifdef WITH_DIFF
    VMStateDiff *diff;
#endif
    
    if (argc <= 1) {
        printf("Using a default of %lu steps\n", nsteps);
    } else {
        nsteps = strtoul(argv[1], &endptr, 0);
        
        if (errno) {
            perror("Invalid command line argument.");
            return 1;
        }
        
        if (*endptr)
            fprintf(stderr, "Warning: junk at end of number.\n");
    }
    

    check_error(state = vm_newstate_no_code(VM_POLICY_INTERRUPT_ALWAYS));
    
    put_code(state);
    
#ifdef WITH_DIFF
    check_error(diff = vm_newdiff());
    vm_step(state, nsteps, diff, &hit_bp);
#else
    vm_step(state, nsteps, NULL, &hit_bp);
#endif
    
    return 0;
}
