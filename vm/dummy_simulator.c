#include "simulator.h"
#include "string.h"
#include "stdio.h"

#define NUM_INSTR 4

int nbits_cpu=8;
char *register_name[3] = {"r1","r2","F"};
int nregisters=3;
char *pin_names[0] = {};
int npins = 0;
size_t ramsize = 256;

char *instruction_name[NUM_INSTR] = {"ld","str","add","sub"};
typedef void (*instruction_func)(VMState *state, VMStateDiff *diff, char* args);
// LD
void ld(VMState * state, VMStateDiff *diff, char *args){
	unsigned int rdi; // destination
	unsigned int s; // source address
	sscanf(args, "r%u, %x\n",&rdi,&s);
	// Do logic
	int *rd = (((int *)state->registers) + rdi - 1);
	unsigned char *sa = (((unsigned char *)state->ram)+s);
	//Do something with diff before actual change
	if (diff != NULL) {
		
	}
	*rd = *sa;
	//Update instruction pointer
	state->current_instruction = strchr(state->current_instruction, '\n') + 1;
}
void str(VMState * state, VMStateDiff *diff, char *args){

}
void add(VMState * state, VMStateDiff *diff, char *args){

}
// SUB
void sub(VMState * state, VMStateDiff *diff, char *args){
	
}

instruction_func instruction_funcs[NUM_INSTR] = {
	ld,
	str,
	add,
	sub
};

void instruction_handler(VMState *state, VMStateDiff *diff){
	// FIXME: Are wide chars better?
	char* instruction = (char *) state->current_instruction;
	// FIXME: This may be difficult to generate?
	char* space = (char *) strchr(instruction, ' ');
	char* args = space+1;
	// TODO: We need to find a piece of code to fit 
	// with instruction This works but is slow
	int i = 0;
	while(i<NUM_INSTR){
		if(strncmp(instruction, instruction_name[i], strlen(instruction_name[i])) == 0){
			instruction_funcs[i](state,diff,args);
			i=NUM_INSTR;
		}
		i++;
	}
}
