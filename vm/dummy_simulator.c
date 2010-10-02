#include "simulator.h"

int nbits_cpu=8;
char *register_name[3] = {"F","r1","r2"};
int nregisters=3;
char *pin_names[0] = {};
int npins = 0;
size_t ramsize = 256;

void instruction_handler(VMState *state, VMStateDiff *diff){
	
}
