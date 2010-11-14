#include <stdio.h>
#include "simulator.h"

// Spec for atmel

// Parameters
size_t ramsize = 0xFFFFFF;
// FIXME: clock = 1;
// FIXME: size = 1;
// End of parameters

int npins = 0;
size_t pinoffset = 0;
int nbits_cpu = 16;
// Registers
int nregisters = 43;
Register registers[] = {
	{ "R0", R0 },
	{ "R1", R1 },
	{ "R2", R2 },
	{ "R3", R3 },
	{ "R4", R4 },
	{ "R5", R5 },
	{ "R6", R6 },
	{ "R7", R7 },
	{ "R8", R8 },
	{ "R9", R9 },
	{ "R10", R10 },
	{ "R11", R11 },
	{ "R12", R12 },
	{ "R13", R13 },
	{ "R14", R14 },
	{ "R15", R15 },
	{ "R16", R16 },
	{ "R17", R17 },
	{ "R18", R18 },
	{ "R19", R19 },
	{ "R20", R20 },
	{ "R21", R21 },
	{ "R22", R22 },
	{ "R23", R23 },
	{ "R24", R24 },
	{ "R25", R25 },
	{ "R26", R26 },
	{ "R27", R27 },
	{ "R28", R28 },
	{ "R29", R29 },
	{ "R30", R30 },
	{ "R31", R31 },
	{ "SREG", SREG },
	{ "C", C },
	{ "Z", Z },
	{ "N", N },
	{ "V", V },
	{ "S", S },
	{ "H", H },
	{ "T", T },
	{ "I", I },
	{ "PC", PC },
	{ "SP", SP }

 };
// End of registers

// helper functions (should be in VM I Think)
bool GetBit(int b, int pos)
{
	return ((b & (1 << pos)) != 0);
}

void AddBit(int *val, int source, int pos)
{
	*val <<=1;
	*val |= GetBit(source,pos);
} 

// Instructions
bool adc (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {

	// error
	bool error = false;

	// result
	int result = 0;

	//Arguments Rd,Rr
	state->cycles += 1;
	// FIXME size = 1;

	// Decode the opcode
	int d = 0; AddBit(&d,opcode,8); AddBit(&d,opcode,7); AddBit(&d,opcode,6); AddBit(&d,opcode,5); AddBit(&d,opcode,4); 
	int r = 0; AddBit(&r,opcode,9); AddBit(&r,opcode,3); AddBit(&r,opcode,2); AddBit(&r,opcode,1); AddBit(&r,opcode,0); 


	// Execute expressions
	// Rd = Rd + Rr + C  
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,d,&error) +  vm_info(state,VM_INFO_REGISTER,r,&error) +  vm_info(state,VM_INFO_REGISTER,C,&error)  ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, d, result))
		return false;


	// H = Rd(3) & Rr(3) + Rr(3) & !R3 + !R3 & Rd(3)     
	// Calculate expressions for the result var
	result =  GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 3) &  GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 3) +  GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 3) & ! vm_info(state,VM_INFO_REGISTER,R3,&error) + ! vm_info(state,VM_INFO_REGISTER,R3,&error) &  GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 3)     ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, H, result))
		return false;


	// S = N ^ V 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,N,&error) ^  vm_info(state,VM_INFO_REGISTER,V,&error) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, S, result))
		return false;


	// V = Rd(7) & Rr(7) & !R7 + !Rd(7) & !Rr(7) & R7     
	// Calculate expressions for the result var
	result =  GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7) &  GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7) & ! vm_info(state,VM_INFO_REGISTER,R7,&error) + ! GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7) & ! GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7) &  vm_info(state,VM_INFO_REGISTER,R7,&error)     ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, V, result))
		return false;


	// N = R7
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,R7,&error);
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, N, result))
		return false;


	// Z = !R7 & !R6 & !R5 & !R4 & !R3 & !R2 & !R1 & !R0       
	// Calculate expressions for the result var
	result = ! vm_info(state,VM_INFO_REGISTER,R7,&error) & ! vm_info(state,VM_INFO_REGISTER,R6,&error) & ! vm_info(state,VM_INFO_REGISTER,R5,&error) & ! vm_info(state,VM_INFO_REGISTER,R4,&error) & ! vm_info(state,VM_INFO_REGISTER,R3,&error) & ! vm_info(state,VM_INFO_REGISTER,R2,&error) & ! vm_info(state,VM_INFO_REGISTER,R1,&error) & ! vm_info(state,VM_INFO_REGISTER,R0,&error)       ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, Z, result))
		return false;


	// C = Rd(7) & Rr(7) + Rr(7) & !R7 + !R7 & Rd(7)     
	// Calculate expressions for the result var
	result =  GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7) &  GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7) +  GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7) & ! vm_info(state,VM_INFO_REGISTER,R7,&error) + ! vm_info(state,VM_INFO_REGISTER,R7,&error) &  GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7)     ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, C, result))
		return false;


	// PC = PC + 1 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,PC,&error) + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;



	//Update instruction pointer
	state->pc = vm_info(state,VM_INFO_REGISTER,PC,&error);

	return !error;
}
bool add (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {

	// error
	bool error = false;

	// result
	int result = 0;

	//Arguments Rd,Rr

	// Decode the opcode
	int d = 0; AddBit(&d,opcode,8); AddBit(&d,opcode,7); AddBit(&d,opcode,6); AddBit(&d,opcode,5); AddBit(&d,opcode,4); 
	int r = 0; AddBit(&r,opcode,9); AddBit(&r,opcode,3); AddBit(&r,opcode,2); AddBit(&r,opcode,1); AddBit(&r,opcode,0); 


	// Execute expressions
	// Rd = Rd + Rr 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,d,&error) +  vm_info(state,VM_INFO_REGISTER,r,&error) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, d, result))
		return false;


	// H = Rd(3) & Rr(3) + Rr(3) & !R3 + !R3 & Rd(3)     
	// Calculate expressions for the result var
	result =  GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 3) &  GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 3) +  GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 3) & ! vm_info(state,VM_INFO_REGISTER,R3,&error) + ! vm_info(state,VM_INFO_REGISTER,R3,&error) &  GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 3)     ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, H, result))
		return false;


	// S = N ^ V 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,N,&error) ^  vm_info(state,VM_INFO_REGISTER,V,&error) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, S, result))
		return false;


	// V = Rd(7) & Rr(7) & !R7 + !Rd(7) & !Rr(7) & R7     
	// Calculate expressions for the result var
	result =  GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7) &  GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7) & ! vm_info(state,VM_INFO_REGISTER,R7,&error) + ! GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7) & ! GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7) &  vm_info(state,VM_INFO_REGISTER,R7,&error)     ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, V, result))
		return false;


	// N = R7
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,R7,&error);
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, N, result))
		return false;


	// Z = !R7 & !R6 & !R5 & !R4 & !R3 & !R2 & !R1 & !R0       
	// Calculate expressions for the result var
	result = ! vm_info(state,VM_INFO_REGISTER,R7,&error) & ! vm_info(state,VM_INFO_REGISTER,R6,&error) & ! vm_info(state,VM_INFO_REGISTER,R5,&error) & ! vm_info(state,VM_INFO_REGISTER,R4,&error) & ! vm_info(state,VM_INFO_REGISTER,R3,&error) & ! vm_info(state,VM_INFO_REGISTER,R2,&error) & ! vm_info(state,VM_INFO_REGISTER,R1,&error) & ! vm_info(state,VM_INFO_REGISTER,R0,&error)       ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, Z, result))
		return false;


	// C = Rd(7) & Rr(7) + Rr(7) & !R7 + !R7 & Rd(7)     
	// Calculate expressions for the result var
	result =  GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7) &  GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7) +  GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7) & ! vm_info(state,VM_INFO_REGISTER,R7,&error) + ! vm_info(state,VM_INFO_REGISTER,R7,&error) &  GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7)     ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, C, result))
		return false;


	// PC = PC + 1 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,PC,&error) + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;



	//Update instruction pointer
	state->pc = vm_info(state,VM_INFO_REGISTER,PC,&error);

	return !error;
}
bool adiw (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {

	// error
	bool error = false;

	// result
	int result = 0;

	//Arguments Rd,K

	// Decode the opcode
	int d = 0; AddBit(&d,opcode,5); AddBit(&d,opcode,4); 
	int K = 0; AddBit(&K,opcode,7); AddBit(&K,opcode,6); AddBit(&K,opcode,3); AddBit(&K,opcode,2); AddBit(&K,opcode,1); AddBit(&K,opcode,0); 


	// Execute expressions
	// S = N ^ V 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,N,&error) ^  vm_info(state,VM_INFO_REGISTER,V,&error) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, S, result))
		return false;


	// PC = PC + 1 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,PC,&error) + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;


	// V = !Rd(h)(7) & R15 
	// Calculate expressions for the result var
	result = ! GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7) &  vm_info(state,VM_INFO_REGISTER,R15,&error) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, V, result))
		return false;


	// N = R15
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,R15,&error);
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, N, result))
		return false;


	// Z = !R15 & !R14 & !R13 & !R12 & !R11 & !R10 & !R9 & !R8 & !R7 & !R6 & !R5 & !R4 & !R3 & !R2 & !R1 & !R0               
	// Calculate expressions for the result var
	result = ! vm_info(state,VM_INFO_REGISTER,R15,&error) & ! vm_info(state,VM_INFO_REGISTER,R14,&error) & ! vm_info(state,VM_INFO_REGISTER,R13,&error) & ! vm_info(state,VM_INFO_REGISTER,R12,&error) & ! vm_info(state,VM_INFO_REGISTER,R11,&error) & ! vm_info(state,VM_INFO_REGISTER,R10,&error) & ! vm_info(state,VM_INFO_REGISTER,R9,&error) & ! vm_info(state,VM_INFO_REGISTER,R8,&error) & ! vm_info(state,VM_INFO_REGISTER,R7,&error) & ! vm_info(state,VM_INFO_REGISTER,R6,&error) & ! vm_info(state,VM_INFO_REGISTER,R5,&error) & ! vm_info(state,VM_INFO_REGISTER,R4,&error) & ! vm_info(state,VM_INFO_REGISTER,R3,&error) & ! vm_info(state,VM_INFO_REGISTER,R2,&error) & ! vm_info(state,VM_INFO_REGISTER,R1,&error) & ! vm_info(state,VM_INFO_REGISTER,R0,&error)               ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, Z, result))
		return false;


	// C = !R15 & Rd(h)(7) 
	// Calculate expressions for the result var
	result = ! vm_info(state,VM_INFO_REGISTER,R15,&error) &  GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, C, result))
		return false;



	//Update instruction pointer
	state->pc = vm_info(state,VM_INFO_REGISTER,PC,&error);

	return !error;
}
bool and (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {

	// error
	bool error = false;

	// result
	int result = 0;

	//Arguments Rd,Rr

	// Decode the opcode
	int d = 0; AddBit(&d,opcode,8); AddBit(&d,opcode,7); AddBit(&d,opcode,6); AddBit(&d,opcode,5); AddBit(&d,opcode,4); 
	int r = 0; AddBit(&r,opcode,9); AddBit(&r,opcode,3); AddBit(&r,opcode,2); AddBit(&r,opcode,1); AddBit(&r,opcode,0); 


	// Execute expressions
	// Rd = Rd & Rr 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,d,&error) &  vm_info(state,VM_INFO_REGISTER,r,&error) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, d, result))
		return false;


	// PC = PC + 1 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,PC,&error) + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;


	// S = N ^ V 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,N,&error) ^  vm_info(state,VM_INFO_REGISTER,V,&error) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, S, result))
		return false;


	// V = 0
	// Calculate expressions for the result var
	result = 0;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, V, result))
		return false;


	// N = R7
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,R7,&error);
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, N, result))
		return false;


	// Z = !R7 & !R6 & !R5 & !R4 & !R3 & !R2 & !R1 & !R0       
	// Calculate expressions for the result var
	result = ! vm_info(state,VM_INFO_REGISTER,R7,&error) & ! vm_info(state,VM_INFO_REGISTER,R6,&error) & ! vm_info(state,VM_INFO_REGISTER,R5,&error) & ! vm_info(state,VM_INFO_REGISTER,R4,&error) & ! vm_info(state,VM_INFO_REGISTER,R3,&error) & ! vm_info(state,VM_INFO_REGISTER,R2,&error) & ! vm_info(state,VM_INFO_REGISTER,R1,&error) & ! vm_info(state,VM_INFO_REGISTER,R0,&error)       ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, Z, result))
		return false;



	//Update instruction pointer
	state->pc = vm_info(state,VM_INFO_REGISTER,PC,&error);

	return !error;
}
bool breq (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {

	// error
	bool error = false;

	// result
	int result = 0;

	//Arguments k

	// Decode the opcode
	int k = 0; AddBit(&k,opcode,9); AddBit(&k,opcode,8); AddBit(&k,opcode,7); AddBit(&k,opcode,6); AddBit(&k,opcode,5); AddBit(&k,opcode,4); AddBit(&k,opcode,3); 


	// Execute expressions
	if ( vm_info(state,VM_INFO_REGISTER,Z,&error) == 1) {
		// PC = PC + k + 1  
		// Calculate expressions for the result var
		result =  vm_info(state,VM_INFO_REGISTER,PC,&error) +  vm_info(state,VM_INFO_REGISTER,k,&error) + 1  ;
		// Check if there was an error in the calculation of the result
		if (error)
			return false;

		if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
			return false;


	}
	else {
		// PC = PC + 1 
		// Calculate expressions for the result var
		result =  vm_info(state,VM_INFO_REGISTER,PC,&error) + 1 ;
		// Check if there was an error in the calculation of the result
		if (error)
			return false;

		if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
			return false;


	}

	//Update instruction pointer
	state->pc = vm_info(state,VM_INFO_REGISTER,PC,&error);

	return !error;
}
bool brge (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {

	// error
	bool error = false;

	// result
	int result = 0;

	//Arguments k

	// Decode the opcode
	int k = 0; AddBit(&k,opcode,8); AddBit(&k,opcode,7); AddBit(&k,opcode,6); AddBit(&k,opcode,5); AddBit(&k,opcode,4); AddBit(&k,opcode,3); 


	// Execute expressions
	if ( vm_info(state,VM_INFO_REGISTER,N,&error) ^  vm_info(state,VM_INFO_REGISTER,V,&error)  == 0) {
		// PC = PC + k + 1  
		// Calculate expressions for the result var
		result =  vm_info(state,VM_INFO_REGISTER,PC,&error) +  vm_info(state,VM_INFO_REGISTER,k,&error) + 1  ;
		// Check if there was an error in the calculation of the result
		if (error)
			return false;

		if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
			return false;


	}
	else {
		// PC = PC + 1 
		// Calculate expressions for the result var
		result =  vm_info(state,VM_INFO_REGISTER,PC,&error) + 1 ;
		// Check if there was an error in the calculation of the result
		if (error)
			return false;

		if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
			return false;


	}

	//Update instruction pointer
	state->pc = vm_info(state,VM_INFO_REGISTER,PC,&error);

	return !error;
}
bool brne (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {

	// error
	bool error = false;

	// result
	int result = 0;

	//Arguments k

	// Decode the opcode
	int k = 0; AddBit(&k,opcode,8); AddBit(&k,opcode,7); AddBit(&k,opcode,6); AddBit(&k,opcode,5); AddBit(&k,opcode,4); AddBit(&k,opcode,3); 


	// Execute expressions
	if ( vm_info(state,VM_INFO_REGISTER,Z,&error) == 0) {
		// PC = PC + k + 1  
		// Calculate expressions for the result var
		result =  vm_info(state,VM_INFO_REGISTER,PC,&error) +  vm_info(state,VM_INFO_REGISTER,k,&error) + 1  ;
		// Check if there was an error in the calculation of the result
		if (error)
			return false;

		if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
			return false;


	}
	else {
		// PC = PC + 1 
		// Calculate expressions for the result var
		result =  vm_info(state,VM_INFO_REGISTER,PC,&error) + 1 ;
		// Check if there was an error in the calculation of the result
		if (error)
			return false;

		if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
			return false;


	}

	//Update instruction pointer
	state->pc = vm_info(state,VM_INFO_REGISTER,PC,&error);

	return !error;
}
bool cli (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {

	// error
	bool error = false;

	// result
	int result = 0;

	//Arguments 

	// Decode the opcode

	// Execute expressions
	// I = 0
	// Calculate expressions for the result var
	result = 0;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, I, result))
		return false;


	// PC = PC + 1 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,PC,&error) + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;



	//Update instruction pointer
	state->pc = vm_info(state,VM_INFO_REGISTER,PC,&error);

	return !error;
}
bool cp (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {

	// error
	bool error = false;

	// result
	int result = 0;

	//Arguments Rd,Rr

	// Decode the opcode
	int d = 0; AddBit(&d,opcode,8); AddBit(&d,opcode,7); AddBit(&d,opcode,6); AddBit(&d,opcode,5); AddBit(&d,opcode,4); 
	int r = 0; AddBit(&r,opcode,9); AddBit(&r,opcode,3); AddBit(&r,opcode,2); AddBit(&r,opcode,1); AddBit(&r,opcode,0); 


	// Execute expressions
	// H = !Rd(3) & Rr(3) + Rr(3) & R3 + R3 & !Rd(3)     
	// Calculate expressions for the result var
	result = ! GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 3) &  GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 3) +  GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 3) &  vm_info(state,VM_INFO_REGISTER,R3,&error) +  vm_info(state,VM_INFO_REGISTER,R3,&error) & ! GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 3)     ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, H, result))
		return false;


	// S = N ^ V 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,N,&error) ^  vm_info(state,VM_INFO_REGISTER,V,&error) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, S, result))
		return false;


	// V = Rd(7) & !Rr(7) & !R7 + !Rd(7) & Rr(7) & R7     
	// Calculate expressions for the result var
	result =  GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7) & ! GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7) & ! vm_info(state,VM_INFO_REGISTER,R7,&error) + ! GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7) &  GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7) &  vm_info(state,VM_INFO_REGISTER,R7,&error)     ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, V, result))
		return false;


	// N = R7
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,R7,&error);
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, N, result))
		return false;


	// Z = !R7 & !R6 & !R5 & !R4 & !R3 & !R2 & !R1 & !R0 & Z        
	// Calculate expressions for the result var
	result = ! vm_info(state,VM_INFO_REGISTER,R7,&error) & ! vm_info(state,VM_INFO_REGISTER,R6,&error) & ! vm_info(state,VM_INFO_REGISTER,R5,&error) & ! vm_info(state,VM_INFO_REGISTER,R4,&error) & ! vm_info(state,VM_INFO_REGISTER,R3,&error) & ! vm_info(state,VM_INFO_REGISTER,R2,&error) & ! vm_info(state,VM_INFO_REGISTER,R1,&error) & ! vm_info(state,VM_INFO_REGISTER,R0,&error) &  vm_info(state,VM_INFO_REGISTER,Z,&error)        ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, Z, result))
		return false;


	// C = !Rd(7) & Rr(7) + Rr(7) & R7 + R7 & !Rd(7)     
	// Calculate expressions for the result var
	result = ! GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7) &  GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7) +  GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7) &  vm_info(state,VM_INFO_REGISTER,R7,&error) +  vm_info(state,VM_INFO_REGISTER,R7,&error) & ! GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7)     ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, C, result))
		return false;


	// PC = PC + 1 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,PC,&error) + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;



	//Update instruction pointer
	state->pc = vm_info(state,VM_INFO_REGISTER,PC,&error);

	return !error;
}
bool cpc (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {

	// error
	bool error = false;

	// result
	int result = 0;

	//Arguments Rd,Rr

	// Decode the opcode
	int d = 0; AddBit(&d,opcode,8); AddBit(&d,opcode,7); AddBit(&d,opcode,6); AddBit(&d,opcode,5); AddBit(&d,opcode,4); 
	int r = 0; AddBit(&r,opcode,9); AddBit(&r,opcode,3); AddBit(&r,opcode,2); AddBit(&r,opcode,1); AddBit(&r,opcode,0); 


	// Execute expressions
	// H = !Rd(3) & Rr(3) + Rr(3) & R3 + R3 & !Rd(3)     
	// Calculate expressions for the result var
	result = ! GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 3) &  GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 3) +  GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 3) &  vm_info(state,VM_INFO_REGISTER,R3,&error) +  vm_info(state,VM_INFO_REGISTER,R3,&error) & ! GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 3)     ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, H, result))
		return false;


	// S = N ^ V 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,N,&error) ^  vm_info(state,VM_INFO_REGISTER,V,&error) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, S, result))
		return false;


	// V = Rd(7) & !Rr(7) & !R7 + !Rd(7) & Rr(7) & R7     
	// Calculate expressions for the result var
	result =  GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7) & ! GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7) & ! vm_info(state,VM_INFO_REGISTER,R7,&error) + ! GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7) &  GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7) &  vm_info(state,VM_INFO_REGISTER,R7,&error)     ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, V, result))
		return false;


	// N = R7
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,R7,&error);
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, N, result))
		return false;


	// Z = !R7 & !R6 & !R5 & !R4 & !R3 & !R2 & !R1 & !R0 & Z        
	// Calculate expressions for the result var
	result = ! vm_info(state,VM_INFO_REGISTER,R7,&error) & ! vm_info(state,VM_INFO_REGISTER,R6,&error) & ! vm_info(state,VM_INFO_REGISTER,R5,&error) & ! vm_info(state,VM_INFO_REGISTER,R4,&error) & ! vm_info(state,VM_INFO_REGISTER,R3,&error) & ! vm_info(state,VM_INFO_REGISTER,R2,&error) & ! vm_info(state,VM_INFO_REGISTER,R1,&error) & ! vm_info(state,VM_INFO_REGISTER,R0,&error) &  vm_info(state,VM_INFO_REGISTER,Z,&error)        ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, Z, result))
		return false;


	// C = !Rd(7) & Rr(7) + Rr(7) & R7 + R7 & !Rd(7)     
	// Calculate expressions for the result var
	result = ! GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7) &  GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7) +  GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7) &  vm_info(state,VM_INFO_REGISTER,R7,&error) +  vm_info(state,VM_INFO_REGISTER,R7,&error) & ! GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7)     ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, C, result))
		return false;


	// PC = PC + 1 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,PC,&error) + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;



	//Update instruction pointer
	state->pc = vm_info(state,VM_INFO_REGISTER,PC,&error);

	return !error;
}
bool cpi (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {

	// error
	bool error = false;

	// result
	int result = 0;

	//Arguments Rd,K

	// Decode the opcode
	int d = 0; AddBit(&d,opcode,7); AddBit(&d,opcode,6); AddBit(&d,opcode,5); AddBit(&d,opcode,4); 
	int K = 0; AddBit(&K,opcode,11); AddBit(&K,opcode,10); AddBit(&K,opcode,9); AddBit(&K,opcode,8); AddBit(&K,opcode,3); AddBit(&K,opcode,2); AddBit(&K,opcode,1); AddBit(&K,opcode,0); 


	// Execute expressions
	// H = !Rd(3) & K(3) + K(3) & R3 + R3 & Rd(3)     
	// Calculate expressions for the result var
	result = ! GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 3) &  GetBit(vm_info(state,VM_INFO_REGISTER,K,&error), 3) +  GetBit(vm_info(state,VM_INFO_REGISTER,K,&error), 3) &  vm_info(state,VM_INFO_REGISTER,R3,&error) +  vm_info(state,VM_INFO_REGISTER,R3,&error) &  GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 3)     ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, H, result))
		return false;


	// S = N ^ V 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,N,&error) ^  vm_info(state,VM_INFO_REGISTER,V,&error) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, S, result))
		return false;


	// V = Rd(7) & !K(7) & !R7 + !Rd(7) & K(7) & R7     
	// Calculate expressions for the result var
	result =  GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7) & ! GetBit(vm_info(state,VM_INFO_REGISTER,K,&error), 7) & ! vm_info(state,VM_INFO_REGISTER,R7,&error) + ! GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7) &  GetBit(vm_info(state,VM_INFO_REGISTER,K,&error), 7) &  vm_info(state,VM_INFO_REGISTER,R7,&error)     ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, V, result))
		return false;


	// N = R7
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,R7,&error);
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, N, result))
		return false;


	// Z = !R7 & !R6 & !R5 & !R4 & !R3 & !R2 & !R1 & !R0       
	// Calculate expressions for the result var
	result = ! vm_info(state,VM_INFO_REGISTER,R7,&error) & ! vm_info(state,VM_INFO_REGISTER,R6,&error) & ! vm_info(state,VM_INFO_REGISTER,R5,&error) & ! vm_info(state,VM_INFO_REGISTER,R4,&error) & ! vm_info(state,VM_INFO_REGISTER,R3,&error) & ! vm_info(state,VM_INFO_REGISTER,R2,&error) & ! vm_info(state,VM_INFO_REGISTER,R1,&error) & ! vm_info(state,VM_INFO_REGISTER,R0,&error)       ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, Z, result))
		return false;


	// C = !Rd(7) & K(7) + K(7) & R7 + R7 & !Rd(7)     
	// Calculate expressions for the result var
	result = ! GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7) &  GetBit(vm_info(state,VM_INFO_REGISTER,K,&error), 7) +  GetBit(vm_info(state,VM_INFO_REGISTER,K,&error), 7) &  vm_info(state,VM_INFO_REGISTER,R7,&error) +  vm_info(state,VM_INFO_REGISTER,R7,&error) & ! GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7)     ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, C, result))
		return false;


	// PC = PC + 1 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,PC,&error) + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;



	//Update instruction pointer
	state->pc = vm_info(state,VM_INFO_REGISTER,PC,&error);

	return !error;
}
bool eor (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {

	// error
	bool error = false;

	// result
	int result = 0;

	//Arguments Rd,Rr

	// Decode the opcode
	int d = 0; AddBit(&d,opcode,8); AddBit(&d,opcode,7); AddBit(&d,opcode,6); AddBit(&d,opcode,5); AddBit(&d,opcode,4); 
	int r = 0; AddBit(&r,opcode,9); AddBit(&r,opcode,3); AddBit(&r,opcode,2); AddBit(&r,opcode,1); AddBit(&r,opcode,0); 


	// Execute expressions
	// Rd = Rd ^ Rr 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,d,&error) ^  vm_info(state,VM_INFO_REGISTER,r,&error) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, d, result))
		return false;


	// S = N ^ V 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,N,&error) ^  vm_info(state,VM_INFO_REGISTER,V,&error) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, S, result))
		return false;


	// V = 0
	// Calculate expressions for the result var
	result = 0;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, V, result))
		return false;


	// N = R7
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,R7,&error);
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, N, result))
		return false;


	// Z = !R7 & !R6 & !R5 & !R4 & !R3 & !R2 & !R1 & !R0       
	// Calculate expressions for the result var
	result = ! vm_info(state,VM_INFO_REGISTER,R7,&error) & ! vm_info(state,VM_INFO_REGISTER,R6,&error) & ! vm_info(state,VM_INFO_REGISTER,R5,&error) & ! vm_info(state,VM_INFO_REGISTER,R4,&error) & ! vm_info(state,VM_INFO_REGISTER,R3,&error) & ! vm_info(state,VM_INFO_REGISTER,R2,&error) & ! vm_info(state,VM_INFO_REGISTER,R1,&error) & ! vm_info(state,VM_INFO_REGISTER,R0,&error)       ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, Z, result))
		return false;


	// PC = PC + 1 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,PC,&error) + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;



	//Update instruction pointer
	state->pc = vm_info(state,VM_INFO_REGISTER,PC,&error);

	return !error;
}
bool in (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {

	// error
	bool error = false;

	// result
	int result = 0;

	//Arguments Rd,A

	// Decode the opcode
	int d = 0; AddBit(&d,opcode,8); AddBit(&d,opcode,7); AddBit(&d,opcode,6); AddBit(&d,opcode,5); AddBit(&d,opcode,4); 
	int A = 0; AddBit(&A,opcode,10); AddBit(&A,opcode,9); AddBit(&A,opcode,3); AddBit(&A,opcode,2); AddBit(&A,opcode,1); AddBit(&A,opcode,0); 


	// Execute expressions
	// A = Rd
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,d,&error);
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, A, result))
		return false;


	// PC = PC + 1 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,PC,&error) + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;



	//Update instruction pointer
	state->pc = vm_info(state,VM_INFO_REGISTER,PC,&error);

	return !error;
}
bool ldi (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {

	// error
	bool error = false;

	// result
	int result = 0;

	//Arguments Rd,K

	// Decode the opcode
	int d = 0; AddBit(&d,opcode,7); AddBit(&d,opcode,6); AddBit(&d,opcode,5); AddBit(&d,opcode,4); 
	int K = 0; AddBit(&K,opcode,11); AddBit(&K,opcode,10); AddBit(&K,opcode,9); AddBit(&K,opcode,8); AddBit(&K,opcode,3); AddBit(&K,opcode,2); AddBit(&K,opcode,1); AddBit(&K,opcode,0); 


	// Execute expressions
	// Rd = K
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,K,&error);
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, d, result))
		return false;


	// PC = PC + 1 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,PC,&error) + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;



	//Update instruction pointer
	state->pc = vm_info(state,VM_INFO_REGISTER,PC,&error);

	return !error;
}
bool lddyplus (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {

	// error
	bool error = false;

	// result
	int result = 0;

	//Arguments 
	
	// Decode the opcode
	int d = 0; AddBit(&d,opcode,8); AddBit(&d,opcode,7); AddBit(&d,opcode,6); AddBit(&d,opcode,5); AddBit(&d,opcode,4); 
	int q = 0; AddBit(&q,opcode,13); AddBit(&q,opcode,11); AddBit(&q,opcode,10); AddBit(&q,opcode,2); AddBit(&q,opcode,1); AddBit(&q,opcode,0); 


	// Execute expressions
	//Rd = RAM(Y+q);
	result = vm_info(state,VM_INFO_REGISTER,RY+q,&error);
	if(!vm_write(state, diff, VM_INFO_REGISTER, d, result))
		return false;
	
	// PC = PC + 1 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,PC,&error) + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;



	//Update instruction pointer
	state->pc = vm_info(state,VM_INFO_REGISTER,PC,&error);

	return !error;
}
bool lsr (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {

	// error
	bool error = false;

	// result
	int result = 0;

	//Arguments Rd

	// Decode the opcode
	int d = 0; AddBit(&d,opcode,8); AddBit(&d,opcode,7); AddBit(&d,opcode,6); AddBit(&d,opcode,5); AddBit(&d,opcode,4); 


	// Execute expressions
	// S = N ^ V 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,N,&error) ^  vm_info(state,VM_INFO_REGISTER,V,&error) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, S, result))
		return false;


	// V = N ^ C 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,N,&error) ^  vm_info(state,VM_INFO_REGISTER,C,&error) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, V, result))
		return false;


	// N = 0
	// Calculate expressions for the result var
	result = 0;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, N, result))
		return false;


	// Z = !R7 & !R6 & !R5 & !R4 & !R3 & !R2 & !R1 & !R0       
	// Calculate expressions for the result var
	result = ! vm_info(state,VM_INFO_REGISTER,R7,&error) & ! vm_info(state,VM_INFO_REGISTER,R6,&error) & ! vm_info(state,VM_INFO_REGISTER,R5,&error) & ! vm_info(state,VM_INFO_REGISTER,R4,&error) & ! vm_info(state,VM_INFO_REGISTER,R3,&error) & ! vm_info(state,VM_INFO_REGISTER,R2,&error) & ! vm_info(state,VM_INFO_REGISTER,R1,&error) & ! vm_info(state,VM_INFO_REGISTER,R0,&error)       ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, Z, result))
		return false;


	// C = Rd(0)
	// Calculate expressions for the result var
	result =  GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 0);
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, C, result))
		return false;


	// PC = PC + 1 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,PC,&error) + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;



	//Update instruction pointer
	state->pc = vm_info(state,VM_INFO_REGISTER,PC,&error);

	return !error;
}
bool mov (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {

	// error
	bool error = false;

	// result
	int result = 0;

	//Arguments Rd,Rr

	// Decode the opcode
	int d = 0; AddBit(&d,opcode,8); AddBit(&d,opcode,7); AddBit(&d,opcode,6); AddBit(&d,opcode,5); AddBit(&d,opcode,4); 
	int r = 0; AddBit(&r,opcode,9); AddBit(&r,opcode,3); AddBit(&r,opcode,2); AddBit(&r,opcode,1); AddBit(&r,opcode,0); 


	// Execute expressions
	// Rd = Rr
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,r,&error);
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, d, result))
		return false;


	// PC = PC + 1 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,PC,&error) + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;



	//Update instruction pointer
	state->pc = vm_info(state,VM_INFO_REGISTER,PC,&error);

	return !error;
}
bool out (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {

	// error
	bool error = false;

	// result
	int result = 0;

	//Arguments A,Rr

	// Decode the opcode
	int r = 0; AddBit(&r,opcode,8); AddBit(&r,opcode,7); AddBit(&r,opcode,6); AddBit(&r,opcode,5); AddBit(&r,opcode,4); 
	int A = 0; AddBit(&A,opcode,10); AddBit(&A,opcode,9); AddBit(&A,opcode,3); AddBit(&A,opcode,2); AddBit(&A,opcode,1); AddBit(&A,opcode,0); 


	// Execute expressions
	// A = Rr
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,r,&error);
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, A, result))
		return false;


	// PC = PC + 1 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,PC,&error) + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;



	//Update instruction pointer
	state->pc = vm_info(state,VM_INFO_REGISTER,PC,&error);

	return !error;
}
bool push (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {

	// error
	bool error = false;

	// result
	int result = 0;

	//Arguments Rr

	// Decode the opcode
	int d = 0; AddBit(&d,opcode,8); AddBit(&d,opcode,7); AddBit(&d,opcode,6); AddBit(&d,opcode,5); AddBit(&d,opcode,4); 


	// Execute expressions
	// PC = PC + 1 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,PC,&error) + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;



	//Update instruction pointer
	state->pc = vm_info(state,VM_INFO_REGISTER,PC,&error);

	return !error;
}
bool ret (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {

	// error
	bool error = false;

	// result
	int result = 0;

	//Arguments 

	// Decode the opcode

	// Execute expressions
	// PC = PC + 1 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,PC,&error) + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;



	//Update instruction pointer
	state->pc = vm_info(state,VM_INFO_REGISTER,PC,&error);

	return !error;
}
bool rcall (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {

	// error
	bool error = false;

	// result
	int result = 0;

	//Arguments k

	// Decode the opcode
	int k = 0; AddBit(&k,opcode,11); AddBit(&k,opcode,10); AddBit(&k,opcode,9); AddBit(&k,opcode,8); AddBit(&k,opcode,7); AddBit(&k,opcode,6); AddBit(&k,opcode,5); AddBit(&k,opcode,4); AddBit(&k,opcode,3); AddBit(&k,opcode,2); AddBit(&k,opcode,1); AddBit(&k,opcode,0); 


	// Execute expressions
	// PC = PC + k + 1  
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,PC,&error) +  vm_info(state,VM_INFO_REGISTER,k,&error) + 1  ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;



	//Update instruction pointer
	state->pc = vm_info(state,VM_INFO_REGISTER,PC,&error);

	return !error;
}
bool rjmp (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {

	// error
	bool error = false;

	// result
	int result = 0;

	//Arguments k

	// Decode the opcode
	int k = 0; AddBit(&k,opcode,11); AddBit(&k,opcode,10); AddBit(&k,opcode,9); AddBit(&k,opcode,8); AddBit(&k,opcode,7); AddBit(&k,opcode,6); AddBit(&k,opcode,5); AddBit(&k,opcode,4); AddBit(&k,opcode,3); AddBit(&k,opcode,2); AddBit(&k,opcode,1); AddBit(&k,opcode,0); 


	// Execute expressions
	// PC = PC + k + 1  
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,PC,&error) +  vm_info(state,VM_INFO_REGISTER,k,&error) + 1  ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;



	//Update instruction pointer
	state->pc = vm_info(state,VM_INFO_REGISTER,PC,&error);

	return !error;
}
bool ror (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {

	// error
	bool error = false;

	// result
	int result = 0;

	//Arguments Rd

	// Decode the opcode
	int d = 0; AddBit(&d,opcode,8); AddBit(&d,opcode,7); AddBit(&d,opcode,6); AddBit(&d,opcode,5); AddBit(&d,opcode,4); 


	// Execute expressions
	// C = Rd(0)
	// Calculate expressions for the result var
	result =  GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 0);
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, C, result))
		return false;



	//Update instruction pointer
	state->pc = vm_info(state,VM_INFO_REGISTER,PC,&error);

	return !error;
}
bool sbiw (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {

	// error
	bool error = false;

	// result
	int result = 0;

	//Arguments Rd,K

	// Decode the opcode
	int d = 0; AddBit(&d,opcode,5); AddBit(&d,opcode,4); 
	int K = 0; AddBit(&K,opcode,7); AddBit(&K,opcode,6); AddBit(&K,opcode,3); AddBit(&K,opcode,2); AddBit(&K,opcode,1); AddBit(&K,opcode,0); 


	// Execute expressions
	// S = N ^ V 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,N,&error) ^  vm_info(state,VM_INFO_REGISTER,V,&error) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, S, result))
		return false;


	// PC = PC + 1 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,PC,&error) + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;


	// V = Rd(h)(7) & !R15 
	// Calculate expressions for the result var
	result =  GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7) & ! vm_info(state,VM_INFO_REGISTER,R15,&error) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, V, result))
		return false;


	// N = R15
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,R15,&error);
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, N, result))
		return false;


	// Z = !R15 & !R14 & !R13 & !R12 & !R11 & !R10 & !R9 & !R8 & !R7 & !R6 & !R5 & !R4 & !R3 & !R2 & !R1 & !R0               
	// Calculate expressions for the result var
	result = ! vm_info(state,VM_INFO_REGISTER,R15,&error) & ! vm_info(state,VM_INFO_REGISTER,R14,&error) & ! vm_info(state,VM_INFO_REGISTER,R13,&error) & ! vm_info(state,VM_INFO_REGISTER,R12,&error) & ! vm_info(state,VM_INFO_REGISTER,R11,&error) & ! vm_info(state,VM_INFO_REGISTER,R10,&error) & ! vm_info(state,VM_INFO_REGISTER,R9,&error) & ! vm_info(state,VM_INFO_REGISTER,R8,&error) & ! vm_info(state,VM_INFO_REGISTER,R7,&error) & ! vm_info(state,VM_INFO_REGISTER,R6,&error) & ! vm_info(state,VM_INFO_REGISTER,R5,&error) & ! vm_info(state,VM_INFO_REGISTER,R4,&error) & ! vm_info(state,VM_INFO_REGISTER,R3,&error) & ! vm_info(state,VM_INFO_REGISTER,R2,&error) & ! vm_info(state,VM_INFO_REGISTER,R1,&error) & ! vm_info(state,VM_INFO_REGISTER,R0,&error)               ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, Z, result))
		return false;


	// C = R15 & !Rd(h)(7) 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,R15,&error) & ! GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, C, result))
		return false;



	//Update instruction pointer
	state->pc = vm_info(state,VM_INFO_REGISTER,PC,&error);

	return !error;
}
bool sbrs (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {

	// error
	bool error = false;

	// result
	int result = 0;

	//Arguments Rr,b

	// Decode the opcode
	int b = 0; AddBit(&b,opcode,2); AddBit(&b,opcode,1); AddBit(&b,opcode,0); 
	int r = 0; AddBit(&r,opcode,8); AddBit(&r,opcode,7); AddBit(&r,opcode,6); AddBit(&r,opcode,5); AddBit(&r,opcode,4); 


	// Execute expressions
	if ( vm_info(state,VM_INFO_REGISTER,r,&error) == 1) {
		// PC = PC + 2 
		// Calculate expressions for the result var
		result =  vm_info(state,VM_INFO_REGISTER,PC,&error) + 2 ;
		// Check if there was an error in the calculation of the result
		if (error)
			return false;

		if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
			return false;


	}
	else {
		// PC = PC + 1 
		// Calculate expressions for the result var
		result =  vm_info(state,VM_INFO_REGISTER,PC,&error) + 1 ;
		// Check if there was an error in the calculation of the result
		if (error)
			return false;

		if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
			return false;


	}

	//Update instruction pointer
	state->pc = vm_info(state,VM_INFO_REGISTER,PC,&error);

	return !error;
}
bool stxplus (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {

	// error
	bool error = false;

	// result
	int result = 0;

	//Arguments X,Rr

	// Decode the opcode
	int r = 0; AddBit(&r,opcode,8); AddBit(&r,opcode,7); AddBit(&r,opcode,6); AddBit(&r,opcode,5); AddBit(&r,opcode,4); 


	// Execute expressions
	
	//RAM(X) = Rr;
	result = vm_info(state,VM_INFO_REGISTER,r,&error);
	if(!vm_write(state, diff, VM_INFO_RAM, RX, result))
		return false;

	// X = X + 1 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,RX,&error) + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, RX, result))
		return false;


	// PC = PC + 1 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,PC,&error) + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;



	//Update instruction pointer
	state->pc = vm_info(state,VM_INFO_REGISTER,PC,&error);

	return !error;
}
bool stdyplus (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {

	// error
	bool error = false;

	// result
	int result = 0;

	//Arguments Y,Rr

	// Decode the opcode
	int r = 0; AddBit(&r,opcode,8); AddBit(&r,opcode,7); AddBit(&r,opcode,6); AddBit(&r,opcode,5); AddBit(&r,opcode,4); 


	// Execute expressions
	
	//RAM(Y) = Rr;
	result = vm_info(state,VM_INFO_REGISTER,r,&error);
	if(!vm_write(state, diff, VM_INFO_RAM, RY, result))
		return false;

	// Y = Y + 1 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,RY,&error) + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, RY, result))
		return false;


	// PC = PC + 1 
	// Calculate expressions for the result var
	result =  vm_info(state,VM_INFO_REGISTER,PC,&error) + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;



	//Update instruction pointer
	state->pc = vm_info(state,VM_INFO_REGISTER,PC,&error);

	return !error;
}

bool nop(VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {}

int n_opcode_handlers = 29;
OpcodeHandler opcode_handlers[] = {
    { "nop",      0b0000000000000,    0b1111111111111111, (opcode_handler *) nop },
	{ "adc",      0b1110000000000,    0b1111110000000000, (opcode_handler *) adc },
	{ "add",      0b110000000000,     0b1111110000000000, (opcode_handler *) add },
	{ "adiw",     0b1001011000000000, 0b1111111100000000, (opcode_handler *) adiw },
	{ "and",      0b10000000000000,   0b1111110000000000, (opcode_handler *) and },
	{ "breq",     0b1111000000000001, 0b1111110000000111, (opcode_handler *) breq },
	{ "brge",     0b111101000000100,  0b111111000000111, (opcode_handler *) brge },
	{ "brne",     0b1111010000000001, 0b1111110000000111, (opcode_handler *) brne },
	{ "cli",      0b1001010011111000, 0b1111111111111111, (opcode_handler *) cli },
	{ "cp",       0b1010000000000,    0b1111110000000000, (opcode_handler *) cp },
	{ "cpc",      0b10000000000,      0b1111110000000000, (opcode_handler *) cpc },
	{ "cpi",      0b11000000000000,   0b1111000000000000, (opcode_handler *) cpi },
	{ "eor",      0b10010000000000,   0b1111110000000000, (opcode_handler *) eor },
	{ "in",       0b1011000000000000, 0b1111100000000000, (opcode_handler *) in },
	{ "ldi",      0b1110000000000000, 0b1111000000000000, (opcode_handler *) ldi },
	{ "lddyplus", 0b1000000000001000, 0b1101001000001000, (opcode_handler *) lddyplus },
	{ "lsr",      0b1001010000000110, 0b1111111000001111, (opcode_handler *) lsr },
	{ "mov",      0b10110000000000,   0b1111110000000000, (opcode_handler *) mov },
	{ "out",      0b1011100000000000, 0b1111100000000000, (opcode_handler *) out },
	{ "push",     0b1001001000001111, 0b1111111000001111, (opcode_handler *) push },
	{ "ret",      0b1001010100001000, 0b1111111111111111, (opcode_handler *) ret },
	{ "rcall",    0b1101000000000000, 0b1111000000000000, (opcode_handler *) rcall },
	{ "rjmp",     0b1100000000000000, 0b1111000000000000, (opcode_handler *) rjmp },
	{ "ror",      0b1001010000000111, 0b1111111000001111, (opcode_handler *) ror },
	{ "sbiw",     0b1001011100000000, 0b1111111100000000, (opcode_handler *) sbiw },
	{ "sbrs",     0b1111111000000000, 0b1111111000001000, (opcode_handler *) sbrs },
	{ "stxplus",  0b1001001000001101, 0b1111111000001111, (opcode_handler *) stxplus },
	{ "stdyplus", 0b1000001000001000, 0b1111111000001111, (opcode_handler *) stdyplus },
    { "lpm",      0b1001010111001000, 0xFFFF, (opcode_handler *) nop },
};
// End of instructions
