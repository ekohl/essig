#include <stdio.h>
#include "simulator.h"

// Spec for atmel

// Parameters
size_t ramsize = 65536;
// FIXME: clock = 1;
// FIXME: size = 1;
// End of parameters

// FIXME: Hardcoded parameters
int npins = 0;
size_t pinoffset = 0;
int nbits_cpu = 16;
bool is_big_endian = false;

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
bool noop (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode

	//Arguments (cast if signed)

	// Execute expressions
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;
	int R = result;

	return true;
}
bool adc (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	
	state->cycles += 1;
	// FIXME size = 1;

	// Decode the opcode
	int d = 0;
	int d_bits = 5;
	AddBit(&d,opcode,8);
	AddBit(&d,opcode,7);
	AddBit(&d,opcode,6);
	AddBit(&d,opcode,5);
	AddBit(&d,opcode,4);

	int r = 0;
	int r_bits = 5;
	AddBit(&r,opcode,9);
	AddBit(&r,opcode,3);
	AddBit(&r,opcode,2);
	AddBit(&r,opcode,1);
	AddBit(&r,opcode,0);



	//Arguments (cast if signed)



	// Execute expressions
	// d = Rd + Rr + C  
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,d,&error)  + vm_info(state,VM_INFO_REGISTER,r,&error)  + vm_info(state,VM_INFO_REGISTER,C,&error)   ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, d, result))
		return false;
	int R = result;
	// H = Rd3 & Rr3 + Rr3 & !R3 + !R3 & Rd3     
	// Calculate expressions for the result var
	result = GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 3)  & GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 3)  + GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 3)  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 3))  + (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 3))  & GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 3)      ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, H, result))
		return false;
	// S = N ^ V 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,N,&error)  ^ vm_info(state,VM_INFO_REGISTER,V,&error)  ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, S, result))
		return false;
	// V = Rd7 & Rr7 & !R7 + !Rd7 & !Rr7 & R7     
	// Calculate expressions for the result var
	result = GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7)  & GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7)  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7))  + (!GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7))  & GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7)      ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, V, result))
		return false;
	// N = R7
	// Calculate expressions for the result var
	result = GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, N, result))
		return false;
	// Z = !R7 & !R6 & !R5 & !R4 & !R3 & !R2 & !R1 & !R0       
	// Calculate expressions for the result var
	result = (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 6))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 5))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 4))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 3))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 2))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 1))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 0))        ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, Z, result))
		return false;
	// C = Rd7 & Rr7 + Rr7 & !R7 + !R7 & Rd7     
	// Calculate expressions for the result var
	result = GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7)  & GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7)  + GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7)  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7))  + (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7))  & GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7)      ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, C, result))
		return false;
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;

	return true;
}
bool add (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int d = 0;
	int d_bits = 5;
	AddBit(&d,opcode,8);
	AddBit(&d,opcode,7);
	AddBit(&d,opcode,6);
	AddBit(&d,opcode,5);
	AddBit(&d,opcode,4);

	int r = 0;
	int r_bits = 5;
	AddBit(&r,opcode,9);
	AddBit(&r,opcode,3);
	AddBit(&r,opcode,2);
	AddBit(&r,opcode,1);
	AddBit(&r,opcode,0);



	//Arguments (cast if signed)



	// Execute expressions
	// d = Rd + Rr 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,d,&error)  + vm_info(state,VM_INFO_REGISTER,r,&error)  ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, d, result))
		return false;
	int R = result;
	// H = Rd3 & Rr3 + Rr3 & !R3 + !R3 & Rd3     
	// Calculate expressions for the result var
	result = GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 3)  & GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 3)  + GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 3)  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 3))  + (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 3))  & GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 3)      ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, H, result))
		return false;
	// S = N ^ V 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,N,&error)  ^ vm_info(state,VM_INFO_REGISTER,V,&error)  ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, S, result))
		return false;
	// V = Rd7 & Rr7 & !R7 + !Rd7 & !Rr7 & R7     
	// Calculate expressions for the result var
	result = GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7)  & GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7)  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7))  + (!GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7))  & GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7)      ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, V, result))
		return false;
	// N = R7
	// Calculate expressions for the result var
	result = GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, N, result))
		return false;
	// Z = !R7 & !R6 & !R5 & !R4 & !R3 & !R2 & !R1 & !R0       
	// Calculate expressions for the result var
	result = (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 6))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 5))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 4))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 3))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 2))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 1))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 0))        ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, Z, result))
		return false;
	// C = Rd7 & Rr7 + Rr7 & !R7 + !R7 & Rd7     
	// Calculate expressions for the result var
	result = GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7)  & GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7)  + GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7)  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7))  + (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7))  & GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7)      ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, C, result))
		return false;
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;

	return true;
}
bool adiw (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int d = 0;
	int d_bits = 2;
	AddBit(&d,opcode,5);
	AddBit(&d,opcode,4);

	int K = 0;
	int K_bits = 6;
	AddBit(&K,opcode,7);
	AddBit(&K,opcode,6);
	AddBit(&K,opcode,3);
	AddBit(&K,opcode,2);
	AddBit(&K,opcode,1);
	AddBit(&K,opcode,0);



	//Arguments (cast if signed)



	// Execute expressions
	// S = N ^ V 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,N,&error)  ^ vm_info(state,VM_INFO_REGISTER,V,&error)  ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, S, result))
		return false;
	int R = result;
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;
	// V = !Rd7 & R15 
	// Calculate expressions for the result var
	result = (!GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7))  & GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 15)  ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, V, result))
		return false;
	// N = R15
	// Calculate expressions for the result var
	result = GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 15) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, N, result))
		return false;
	// Z = !R15 & !R14 & !R13 & !R12 & !R11 & !R10 & !R9 & !R8 & !R7 & !R6 & !R5 & !R4 & !R3 & !R2 & !R1 & !R0               
	// Calculate expressions for the result var
	result = (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 15))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 14))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 13))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 12))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 11))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 10))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 9))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 8))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 6))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 5))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 4))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 3))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 2))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 1))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 0))                ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, Z, result))
		return false;
	// C = !R15 & Rd7 
	// Calculate expressions for the result var
	result = (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 15))  & GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7)  ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, C, result))
		return false;

	return true;
}
bool and (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int d = 0;
	int d_bits = 5;
	AddBit(&d,opcode,8);
	AddBit(&d,opcode,7);
	AddBit(&d,opcode,6);
	AddBit(&d,opcode,5);
	AddBit(&d,opcode,4);

	int r = 0;
	int r_bits = 5;
	AddBit(&r,opcode,9);
	AddBit(&r,opcode,3);
	AddBit(&r,opcode,2);
	AddBit(&r,opcode,1);
	AddBit(&r,opcode,0);



	//Arguments (cast if signed)



	// Execute expressions
	// d = Rd & Rr 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,d,&error)  & vm_info(state,VM_INFO_REGISTER,r,&error)  ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, d, result))
		return false;
	int R = result;
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;
	// S = N ^ V 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,N,&error)  ^ vm_info(state,VM_INFO_REGISTER,V,&error)  ;
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
	result = GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, N, result))
		return false;
	// Z = !R7 & !R6 & !R5 & !R4 & !R3 & !R2 & !R1 & !R0       
	// Calculate expressions for the result var
	result = (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 6))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 5))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 4))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 3))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 2))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 1))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 0))        ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, Z, result))
		return false;

	return true;
}
bool andi (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int d = 0;
	int d_bits = 4;
	AddBit(&d,opcode,7);
	AddBit(&d,opcode,6);
	AddBit(&d,opcode,5);
	AddBit(&d,opcode,4);

	int K = 0;
	int K_bits = 8;
	AddBit(&K,opcode,11);
	AddBit(&K,opcode,10);
	AddBit(&K,opcode,9);
	AddBit(&K,opcode,8);
	AddBit(&K,opcode,3);
	AddBit(&K,opcode,2);
	AddBit(&K,opcode,1);
	AddBit(&K,opcode,0);



	//Arguments (cast if signed)



	// Execute expressions
	// d = Rd & K 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,d,&error)  & K  ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, d, result))
		return false;
	int R = result;
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;
	// S = N ^ V 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,N,&error)  ^ vm_info(state,VM_INFO_REGISTER,V,&error)  ;
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
	result = GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, N, result))
		return false;
	// Z = !R7 & !R6 & !R5 & !R4 & !R3 & !R2 & !R1 & !R0       
	// Calculate expressions for the result var
	result = (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 6))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 5))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 4))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 3))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 2))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 1))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 0))        ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, Z, result))
		return false;

	return true;
}
bool breq (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int k = 0;
	int k_bits = 7;
	AddBit(&k,opcode,9);
	AddBit(&k,opcode,8);
	AddBit(&k,opcode,7);
	AddBit(&k,opcode,6);
	AddBit(&k,opcode,5);
	AddBit(&k,opcode,4);
	AddBit(&k,opcode,3);



	//Arguments (cast if signed)
	 k = (int) vm_convert_to_signed(k,k_bits); 

	// Execute expressions
	if (vm_info(state,VM_INFO_REGISTER,Z,&error)  == 1) {
		// PC = PC + k + 1  
		// Calculate expressions for the result var
		result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + k  + 1  ;
		// Check if there was an error in the calculation of the result
		if (error)
			return false;

		if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
			return false;
	}
	else {
		// PC = PC + 1 
		// Calculate expressions for the result var
		result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
		// Check if there was an error in the calculation of the result
		if (error)
			return false;

		if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
			return false;
	}
	int R = result;

	return true;
}
bool brge (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int k = 0;
	int k_bits = 7;
	AddBit(&k,opcode,9);
	AddBit(&k,opcode,8);
	AddBit(&k,opcode,7);
	AddBit(&k,opcode,6);
	AddBit(&k,opcode,5);
	AddBit(&k,opcode,4);
	AddBit(&k,opcode,3);



	//Arguments (cast if signed)
	 k = (int) vm_convert_to_signed(k,k_bits); 

	// Execute expressions
	if (vm_info(state,VM_INFO_REGISTER,N,&error)  ^ vm_info(state,VM_INFO_REGISTER,V,&error)   == 0) {
		// PC = PC + k + 1  
		// Calculate expressions for the result var
		result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + k  + 1  ;
		// Check if there was an error in the calculation of the result
		if (error)
			return false;

		if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
			return false;
	}
	else {
		// PC = PC + 1 
		// Calculate expressions for the result var
		result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
		// Check if there was an error in the calculation of the result
		if (error)
			return false;

		if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
			return false;
	}
	int R = result;

	return true;
}
bool brne (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int k = 0;
	int k_bits = 7;
	AddBit(&k,opcode,9);
	AddBit(&k,opcode,8);
	AddBit(&k,opcode,7);
	AddBit(&k,opcode,6);
	AddBit(&k,opcode,5);
	AddBit(&k,opcode,4);
	AddBit(&k,opcode,3);



	//Arguments (cast if signed)
	 k = (int) vm_convert_to_signed(k,k_bits); 

	// Execute expressions
	if (vm_info(state,VM_INFO_REGISTER,Z,&error)  == 0) {
		// PC = PC + k + 1  
		// Calculate expressions for the result var
		result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + k  + 1  ;
		// Check if there was an error in the calculation of the result
		if (error)
			return false;

		if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
			return false;
	}
	else {
		// PC = PC + 1 
		// Calculate expressions for the result var
		result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
		// Check if there was an error in the calculation of the result
		if (error)
			return false;

		if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
			return false;
	}
	int R = result;

	return true;
}
bool cli (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode

	//Arguments (cast if signed)

	// Execute expressions
	// I = 0
	// Calculate expressions for the result var
	result = 0;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, I, result))
		return false;
	int R = result;
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;

	return true;
}
bool cp (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int d = 0;
	int d_bits = 5;
	AddBit(&d,opcode,8);
	AddBit(&d,opcode,7);
	AddBit(&d,opcode,6);
	AddBit(&d,opcode,5);
	AddBit(&d,opcode,4);

	int r = 0;
	int r_bits = 5;
	AddBit(&r,opcode,9);
	AddBit(&r,opcode,3);
	AddBit(&r,opcode,2);
	AddBit(&r,opcode,1);
	AddBit(&r,opcode,0);



	//Arguments (cast if signed)



	// Execute expressions
	// R = Rd - Rr 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,d,&error)  - vm_info(state,VM_INFO_REGISTER,r,&error)  ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;


	int R = result;
	// H = !Rd3 & Rr3 + Rr3 & R3 + R3 & !Rd3     
	// Calculate expressions for the result var
	result = (!GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 3))  & GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 3)  + GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 3)  & GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 3)  + GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 3)  & (!GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 3))      ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, H, result))
		return false;
	// S = N ^ V 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,N,&error)  ^ vm_info(state,VM_INFO_REGISTER,V,&error)  ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, S, result))
		return false;
	// V = Rd7 & !Rr7 & !R7 + !Rd7 & Rr7 & R7     
	// Calculate expressions for the result var
	result = GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7)  & (!GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7))  + (!GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7))  & GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7)  & GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7)      ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, V, result))
		return false;
	// N = R7
	// Calculate expressions for the result var
	result = GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, N, result))
		return false;
	// Z = !R7 & !R6 & !R5 & !R4 & !R3 & !R2 & !R1 & !R0 & Z        
	// Calculate expressions for the result var
	result = (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 6))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 5))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 4))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 3))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 2))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 1))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 0))  & vm_info(state,VM_INFO_REGISTER,Z,&error)         ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, Z, result))
		return false;
	// C = !Rd7 & Rr7 + Rr7 & R7 + R7 & !Rd7     
	// Calculate expressions for the result var
	result = (!GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7))  & GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7)  + GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7)  & GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7)  + GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7)  & (!GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7))      ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, C, result))
		return false;
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;

	return true;
}
bool cpc (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int d = 0;
	int d_bits = 5;
	AddBit(&d,opcode,8);
	AddBit(&d,opcode,7);
	AddBit(&d,opcode,6);
	AddBit(&d,opcode,5);
	AddBit(&d,opcode,4);

	int r = 0;
	int r_bits = 5;
	AddBit(&r,opcode,9);
	AddBit(&r,opcode,3);
	AddBit(&r,opcode,2);
	AddBit(&r,opcode,1);
	AddBit(&r,opcode,0);



	//Arguments (cast if signed)



	// Execute expressions
	// R = Rd - Rr - C  
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,d,&error)  - vm_info(state,VM_INFO_REGISTER,r,&error)  - vm_info(state,VM_INFO_REGISTER,C,&error)   ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;


	int R = result;
	// H = !Rd3 & Rr3 + Rr3 & R3 + R3 & !Rd3     
	// Calculate expressions for the result var
	result = (!GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 3))  & GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 3)  + GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 3)  & GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 3)  + GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 3)  & (!GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 3))      ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, H, result))
		return false;
	// S = N ^ V 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,N,&error)  ^ vm_info(state,VM_INFO_REGISTER,V,&error)  ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, S, result))
		return false;
	// V = Rd7 & !Rr7 & !R7 + !Rd7 & Rr7 & R7     
	// Calculate expressions for the result var
	result = GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7)  & (!GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7))  + (!GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7))  & GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7)  & GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7)      ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, V, result))
		return false;
	// N = R7
	// Calculate expressions for the result var
	result = GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, N, result))
		return false;
	// Z = !R7 & !R6 & !R5 & !R4 & !R3 & !R2 & !R1 & !R0 & Z        
	// Calculate expressions for the result var
	result = (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 6))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 5))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 4))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 3))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 2))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 1))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 0))  & vm_info(state,VM_INFO_REGISTER,Z,&error)         ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, Z, result))
		return false;
	// C = !Rd7 & Rr7 + Rr7 & R7 + R7 & !Rd7     
	// Calculate expressions for the result var
	result = (!GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7))  & GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7)  + GetBit(vm_info(state,VM_INFO_REGISTER,r,&error), 7)  & GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7)  + GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7)  & (!GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7))      ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, C, result))
		return false;
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;

	return true;
}
bool cpi (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int d = 0;
	int d_bits = 4;
	AddBit(&d,opcode,7);
	AddBit(&d,opcode,6);
	AddBit(&d,opcode,5);
	AddBit(&d,opcode,4);

	int K = 0;
	int K_bits = 8;
	AddBit(&K,opcode,11);
	AddBit(&K,opcode,10);
	AddBit(&K,opcode,9);
	AddBit(&K,opcode,8);
	AddBit(&K,opcode,3);
	AddBit(&K,opcode,2);
	AddBit(&K,opcode,1);
	AddBit(&K,opcode,0);



	//Arguments (cast if signed)



	// Execute expressions
	// R = Rd - K 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,d,&error)  - K  ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;


	int R = result;
	// H = !Rd3 & !K3 + K3 & R3 + R3 & Rd3     
	// Calculate expressions for the result var
	result = (!GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 3))  & (!GetBit(K, 3))  + GetBit(K, 3)  & GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 3)  + GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 3)  & GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 3)      ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, H, result))
		return false;
	// S = N ^ V 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,N,&error)  ^ vm_info(state,VM_INFO_REGISTER,V,&error)  ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, S, result))
		return false;
	// V = Rd7 & !K7 & !R7 + !Rd7 & K7 & R7     
	// Calculate expressions for the result var
	result = GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7)  & (!GetBit(K, 7))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7))  + (!GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7))  & GetBit(K, 7)  & GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7)      ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, V, result))
		return false;
	// N = R7
	// Calculate expressions for the result var
	result = GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, N, result))
		return false;
	// Z = !R7 & !R6 & !R5 & !R4 & !R3 & !R2 & !R1 & !R0       
	// Calculate expressions for the result var
	result = (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 6))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 5))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 4))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 3))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 2))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 1))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 0))        ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, Z, result))
		return false;
	// C = !Rd7 & K7 + K7 & R7 + R7 & !Rd7     
	// Calculate expressions for the result var
	result = (!GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7))  & GetBit(vm_info(state,VM_INFO_REGISTER,K,&error), 7)  + GetBit(vm_info(state,VM_INFO_REGISTER,K,&error), 7)  & GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7)  + GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7)  & (!GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7))      ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, C, result))
		return false;
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;

	return true;
}
bool com (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int d = 0;
	int d_bits = 5;
	AddBit(&d,opcode,8);
	AddBit(&d,opcode,7);
	AddBit(&d,opcode,6);
	AddBit(&d,opcode,5);
	AddBit(&d,opcode,4);



	//Arguments (cast if signed)



	// Execute expressions
	// d = 255 - Rd 
	// Calculate expressions for the result var
	result = 255 - vm_info(state,VM_INFO_REGISTER,d,&error)  ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, d, result))
		return false;
	int R = result;
	// S = N ^ V 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,N,&error)  ^ vm_info(state,VM_INFO_REGISTER,V,&error)  ;
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
	result = GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, N, result))
		return false;
	// Z = !R7 & !R6 & !R5 & !R4 & !R3 & !R2 & !R1 & !R0       
	// Calculate expressions for the result var
	result = (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 6))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 5))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 4))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 3))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 2))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 1))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 0))        ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, Z, result))
		return false;
	// C = 1
	// Calculate expressions for the result var
	result = 1;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, C, result))
		return false;
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;

	return true;
}
bool eor (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int d = 0;
	int d_bits = 5;
	AddBit(&d,opcode,8);
	AddBit(&d,opcode,7);
	AddBit(&d,opcode,6);
	AddBit(&d,opcode,5);
	AddBit(&d,opcode,4);

	int r = 0;
	int r_bits = 5;
	AddBit(&r,opcode,9);
	AddBit(&r,opcode,3);
	AddBit(&r,opcode,2);
	AddBit(&r,opcode,1);
	AddBit(&r,opcode,0);



	//Arguments (cast if signed)



	// Execute expressions
	// d = Rd ^ Rr 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,d,&error)  ^ vm_info(state,VM_INFO_REGISTER,r,&error)  ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, d, result))
		return false;
	int R = result;
	// S = N ^ V 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,N,&error)  ^ vm_info(state,VM_INFO_REGISTER,V,&error)  ;
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
	result = GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, N, result))
		return false;
	// Z = !R7 & !R6 & !R5 & !R4 & !R3 & !R2 & !R1 & !R0       
	// Calculate expressions for the result var
	result = (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 6))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 5))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 4))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 3))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 2))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 1))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 0))        ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, Z, result))
		return false;
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;

	return true;
}
bool in (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int d = 0;
	int d_bits = 5;
	AddBit(&d,opcode,8);
	AddBit(&d,opcode,7);
	AddBit(&d,opcode,6);
	AddBit(&d,opcode,5);
	AddBit(&d,opcode,4);

	int A = 0;
	int A_bits = 6;
	AddBit(&A,opcode,10);
	AddBit(&A,opcode,9);
	AddBit(&A,opcode,3);
	AddBit(&A,opcode,2);
	AddBit(&A,opcode,1);
	AddBit(&A,opcode,0);



	//Arguments (cast if signed)



	// Execute expressions
	// A = Rd
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,d,&error) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_RAM, A, result))
		return false;
	int R = result;
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;

	return true;
}
bool ldi (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int d = 0;
	int d_bits = 4;
	AddBit(&d,opcode,7);
	AddBit(&d,opcode,6);
	AddBit(&d,opcode,5);
	AddBit(&d,opcode,4);

	int K = 0;
	int K_bits = 8;
	AddBit(&K,opcode,11);
	AddBit(&K,opcode,10);
	AddBit(&K,opcode,9);
	AddBit(&K,opcode,8);
	AddBit(&K,opcode,3);
	AddBit(&K,opcode,2);
	AddBit(&K,opcode,1);
	AddBit(&K,opcode,0);



	//Arguments (cast if signed)



	// Execute expressions
	// d = K
	// Calculate expressions for the result var
	result = K ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, d, result))
		return false;
	int R = result;
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;

	return true;
}
bool lddyplus (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int d = 0;
	int d_bits = 5;
	AddBit(&d,opcode,8);
	AddBit(&d,opcode,7);
	AddBit(&d,opcode,6);
	AddBit(&d,opcode,5);
	AddBit(&d,opcode,4);

	int q = 0;
	int q_bits = 6;
	AddBit(&q,opcode,13);
	AddBit(&q,opcode,11);
	AddBit(&q,opcode,10);
	AddBit(&q,opcode,2);
	AddBit(&q,opcode,1);
	AddBit(&q,opcode,0);



	//Arguments (cast if signed)



	// Execute expressions
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;
	int R = result;

	return true;
}
bool lddzmin (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int d = 0;
	int d_bits = 5;
	AddBit(&d,opcode,8);
	AddBit(&d,opcode,7);
	AddBit(&d,opcode,6);
	AddBit(&d,opcode,5);
	AddBit(&d,opcode,4);



	//Arguments (cast if signed)

	// Execute expressions
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;
	int R = result;

	return true;
}
bool lddzplus (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int d = 0;
	int d_bits = 5;
	AddBit(&d,opcode,8);
	AddBit(&d,opcode,7);
	AddBit(&d,opcode,6);
	AddBit(&d,opcode,5);
	AddBit(&d,opcode,4);



	//Arguments (cast if signed)

	// Execute expressions
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;
	int R = result;

	return true;
}
bool lddzq (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int d = 0;
	int d_bits = 5;
	AddBit(&d,opcode,8);
	AddBit(&d,opcode,7);
	AddBit(&d,opcode,6);
	AddBit(&d,opcode,5);
	AddBit(&d,opcode,4);

	int q = 0;
	int q_bits = 6;
	AddBit(&q,opcode,13);
	AddBit(&q,opcode,11);
	AddBit(&q,opcode,10);
	AddBit(&q,opcode,2);
	AddBit(&q,opcode,1);
	AddBit(&q,opcode,0);



	//Arguments (cast if signed)



	// Execute expressions
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;
	int R = result;

	return true;
}
bool lds (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int d = 0;
	int d_bits = 5;
	AddBit(&d,opcode,8);
	AddBit(&d,opcode,7);
	AddBit(&d,opcode,6);
	AddBit(&d,opcode,5);
	AddBit(&d,opcode,4);



	//Arguments (cast if signed)



	// Execute expressions
	// PC = PC + 2 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 2 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;
	int R = result;

	return true;
}
bool lsr (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int d = 0;
	int d_bits = 5;
	AddBit(&d,opcode,8);
	AddBit(&d,opcode,7);
	AddBit(&d,opcode,6);
	AddBit(&d,opcode,5);
	AddBit(&d,opcode,4);



	//Arguments (cast if signed)

	// Execute expressions
	// S = N ^ V 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,N,&error)  ^ vm_info(state,VM_INFO_REGISTER,V,&error)  ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, S, result))
		return false;
	int R = result;
	// V = N ^ C 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,N,&error)  ^ vm_info(state,VM_INFO_REGISTER,C,&error)  ;
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
	result = (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 6))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 5))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 4))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 3))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 2))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 1))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 0))        ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, Z, result))
		return false;
	// C = Rd0
	// Calculate expressions for the result var
	result = GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 0) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, C, result))
		return false;
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;

	return true;
}
bool lpm (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode

	//Arguments (cast if signed)

	// Execute expressions
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;
	int R = result;

	return true;
}
bool mov (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int d = 0;
	int d_bits = 5;
	AddBit(&d,opcode,8);
	AddBit(&d,opcode,7);
	AddBit(&d,opcode,6);
	AddBit(&d,opcode,5);
	AddBit(&d,opcode,4);

	int r = 0;
	int r_bits = 5;
	AddBit(&r,opcode,9);
	AddBit(&r,opcode,3);
	AddBit(&r,opcode,2);
	AddBit(&r,opcode,1);
	AddBit(&r,opcode,0);



	//Arguments (cast if signed)



	// Execute expressions
	// d = Rr
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,r,&error) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, d, result))
		return false;
	int R = result;
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;

	return true;
}
bool out (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int r = 0;
	int r_bits = 5;
	AddBit(&r,opcode,8);
	AddBit(&r,opcode,7);
	AddBit(&r,opcode,6);
	AddBit(&r,opcode,5);
	AddBit(&r,opcode,4);

	int A = 0;
	int A_bits = 6;
	AddBit(&A,opcode,10);
	AddBit(&A,opcode,9);
	AddBit(&A,opcode,3);
	AddBit(&A,opcode,2);
	AddBit(&A,opcode,1);
	AddBit(&A,opcode,0);



	//Arguments (cast if signed)



	// Execute expressions
	// A = Rr
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,r,&error) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_RAM, A, result))
		return false;
	int R = result;
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;

	return true;
}
bool push (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int r = 0;
	int r_bits = 5;
	AddBit(&r,opcode,8);
	AddBit(&r,opcode,7);
	AddBit(&r,opcode,6);
	AddBit(&r,opcode,5);
	AddBit(&r,opcode,4);



	//Arguments (cast if signed)

	// Execute expressions
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;
	int R = result;

	return true;
}
bool pop (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int d = 0;
	int d_bits = 5;
	AddBit(&d,opcode,8);
	AddBit(&d,opcode,7);
	AddBit(&d,opcode,6);
	AddBit(&d,opcode,5);
	AddBit(&d,opcode,4);



	//Arguments (cast if signed)

	// Execute expressions
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;
	int R = result;

	return true;
}
bool ret (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode

	//Arguments (cast if signed)

	// Execute expressions
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;
	int R = result;

	return true;
}
bool rcall (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int k = 0;
	int k_bits = 12;
	AddBit(&k,opcode,11);
	AddBit(&k,opcode,10);
	AddBit(&k,opcode,9);
	AddBit(&k,opcode,8);
	AddBit(&k,opcode,7);
	AddBit(&k,opcode,6);
	AddBit(&k,opcode,5);
	AddBit(&k,opcode,4);
	AddBit(&k,opcode,3);
	AddBit(&k,opcode,2);
	AddBit(&k,opcode,1);
	AddBit(&k,opcode,0);



	//Arguments (cast if signed)
	 k = (int) vm_convert_to_signed(k,k_bits); 

	// Execute expressions
	// PC = PC + k + 1  
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + k  + 1  ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;
	int R = result;

	return true;
}
bool rjmp (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int k = 0;
	int k_bits = 12;
	AddBit(&k,opcode,11);
	AddBit(&k,opcode,10);
	AddBit(&k,opcode,9);
	AddBit(&k,opcode,8);
	AddBit(&k,opcode,7);
	AddBit(&k,opcode,6);
	AddBit(&k,opcode,5);
	AddBit(&k,opcode,4);
	AddBit(&k,opcode,3);
	AddBit(&k,opcode,2);
	AddBit(&k,opcode,1);
	AddBit(&k,opcode,0);



	//Arguments (cast if signed)
	 k = (int) vm_convert_to_signed(k,k_bits); 

	// Execute expressions
	if (k  == -1) {
			state->stopped_running = true;
		        return false;
	}
	else {
		// PC = PC + k + 1  
		// Calculate expressions for the result var
		result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + k  + 1  ;
		// Check if there was an error in the calculation of the result
		if (error)
			return false;

		if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
			return false;
	}
	int R = result;

	return true;
}
bool ror (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int d = 0;
	int d_bits = 5;
	AddBit(&d,opcode,8);
	AddBit(&d,opcode,7);
	AddBit(&d,opcode,6);
	AddBit(&d,opcode,5);
	AddBit(&d,opcode,4);



	//Arguments (cast if signed)

	// Execute expressions
	// C = Rd0
	// Calculate expressions for the result var
	result = GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 0) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, C, result))
		return false;
	int R = result;

	return true;
}
bool sbiw (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int d = 0;
	int d_bits = 2;
	AddBit(&d,opcode,5);
	AddBit(&d,opcode,4);

	int K = 0;
	int K_bits = 6;
	AddBit(&K,opcode,7);
	AddBit(&K,opcode,6);
	AddBit(&K,opcode,3);
	AddBit(&K,opcode,2);
	AddBit(&K,opcode,1);
	AddBit(&K,opcode,0);



	//Arguments (cast if signed)



	// Execute expressions
	// S = N ^ V 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,N,&error)  ^ vm_info(state,VM_INFO_REGISTER,V,&error)  ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, S, result))
		return false;
	int R = result;
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;
	// V = Rd7 & !R15 
	// Calculate expressions for the result var
	result = GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7)  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 15))  ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, V, result))
		return false;
	// N = R15
	// Calculate expressions for the result var
	result = GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 15) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, N, result))
		return false;
	// Z = !R15 & !R14 & !R13 & !R12 & !R11 & !R10 & !R9 & !R8 & !R7 & !R6 & !R5 & !R4 & !R3 & !R2 & !R1 & !R0               
	// Calculate expressions for the result var
	result = (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 15))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 14))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 13))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 12))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 11))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 10))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 9))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 8))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 6))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 5))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 4))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 3))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 2))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 1))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 0))                ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, Z, result))
		return false;
	// C = R15 & !Rd7 
	// Calculate expressions for the result var
	result = GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 15)  & (!GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7))  ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, C, result))
		return false;

	return true;
}
bool sbrs (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int b = 0;
	int b_bits = 3;
	AddBit(&b,opcode,2);
	AddBit(&b,opcode,1);
	AddBit(&b,opcode,0);

	int r = 0;
	int r_bits = 5;
	AddBit(&r,opcode,8);
	AddBit(&r,opcode,7);
	AddBit(&r,opcode,6);
	AddBit(&r,opcode,5);
	AddBit(&r,opcode,4);



	//Arguments (cast if signed)



	// Execute expressions
	if (vm_info(state,VM_INFO_REGISTER,r,&error)  == 1) {
		// PC = PC + 2 
		// Calculate expressions for the result var
		result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 2 ;
		// Check if there was an error in the calculation of the result
		if (error)
			return false;

		if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
			return false;
	}
	else {
		// PC = PC + 1 
		// Calculate expressions for the result var
		result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
		// Check if there was an error in the calculation of the result
		if (error)
			return false;

		if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
			return false;
	}
	int R = result;

	return true;
}
bool stxplus (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int r = 0;
	int r_bits = 5;
	AddBit(&r,opcode,8);
	AddBit(&r,opcode,7);
	AddBit(&r,opcode,6);
	AddBit(&r,opcode,5);
	AddBit(&r,opcode,4);



	//Arguments (cast if signed)



	// Execute expressions
	// vm_info(state,VM_INFO_REGISTER,R26,&error)  = Rr
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,r,&error) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_RAM, vm_info(state,VM_INFO_REGISTER,R26,&error) , result))
		return false;
	int R = result;
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;

	return true;
}
bool stdyplus (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int r = 0;
	int r_bits = 5;
	AddBit(&r,opcode,8);
	AddBit(&r,opcode,7);
	AddBit(&r,opcode,6);
	AddBit(&r,opcode,5);
	AddBit(&r,opcode,4);



	//Arguments (cast if signed)

	// Execute expressions
	// vm_info(state,VM_INFO_REGISTER,R28,&error)  = Rr
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,r,&error) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_RAM, vm_info(state,VM_INFO_REGISTER,R28,&error) , result))
		return false;
	int R = result;
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;

	return true;
}
bool stdyplusq (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int r = 0;
	int r_bits = 5;
	AddBit(&r,opcode,8);
	AddBit(&r,opcode,7);
	AddBit(&r,opcode,6);
	AddBit(&r,opcode,5);
	AddBit(&r,opcode,4);

	int q = 0;
	int q_bits = 6;
	AddBit(&q,opcode,13);
	AddBit(&q,opcode,11);
	AddBit(&q,opcode,10);
	AddBit(&q,opcode,2);
	AddBit(&q,opcode,1);
	AddBit(&q,opcode,0);



	//Arguments (cast if signed)



	// Execute expressions
	// vm_info(state,VM_INFO_REGISTER,R28,&error)  + vm_info(state,VM_INFO_REGISTER,q,&error)   = Rr
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,r,&error) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_RAM, vm_info(state,VM_INFO_REGISTER,R28,&error)  + vm_info(state,VM_INFO_REGISTER,q,&error)  , result))
		return false;
	int R = result;
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;

	return true;
}
bool stdzplus (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int r = 0;
	int r_bits = 5;
	AddBit(&r,opcode,8);
	AddBit(&r,opcode,7);
	AddBit(&r,opcode,6);
	AddBit(&r,opcode,5);
	AddBit(&r,opcode,4);

	int q = 0;
	int q_bits = 6;
	AddBit(&q,opcode,13);
	AddBit(&q,opcode,11);
	AddBit(&q,opcode,10);
	AddBit(&q,opcode,2);
	AddBit(&q,opcode,1);
	AddBit(&q,opcode,0);



	//Arguments (cast if signed)



	// Execute expressions
	// vm_info(state,VM_INFO_REGISTER,R30,&error)  + vm_info(state,VM_INFO_REGISTER,q,&error)   = Rr
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,r,&error) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_RAM, vm_info(state,VM_INFO_REGISTER,R30,&error)  + vm_info(state,VM_INFO_REGISTER,q,&error)  , result))
		return false;
	int R = result;
	// PC = PC + 1 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
		return false;

	return true;
}
bool subi (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int d = 0;
	int d_bits = 4;
	AddBit(&d,opcode,7);
	AddBit(&d,opcode,6);
	AddBit(&d,opcode,5);
	AddBit(&d,opcode,4);

	int K = 0;
	int K_bits = 8;
	AddBit(&K,opcode,11);
	AddBit(&K,opcode,10);
	AddBit(&K,opcode,9);
	AddBit(&K,opcode,8);
	AddBit(&K,opcode,3);
	AddBit(&K,opcode,2);
	AddBit(&K,opcode,1);
	AddBit(&K,opcode,0);



	//Arguments (cast if signed)



	// Execute expressions
	// d = Rd - K 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,d,&error)  - vm_info(state,VM_INFO_REGISTER,K,&error)  ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, d, result))
		return false;
	int R = result;
	// H = !Rd3 & K3 + K3 & R3 + R3 & Rd3     
	// Calculate expressions for the result var
	result = (!GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 3))  & GetBit(K, 3)  + GetBit(K, 3)  & GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 3)  + GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 3)  & GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 3)      ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, H, result))
		return false;
	// S = N ^ V 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,N,&error)  ^ vm_info(state,VM_INFO_REGISTER,V,&error)  ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, S, result))
		return false;
	// V = Rd7 & !K7 & R7 + Rd7 & K7 & R7     
	// Calculate expressions for the result var
	result = GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7)  & (!GetBit(K, 7))  & GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7)  + GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7)  & GetBit(K, 7)  & GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7)      ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, V, result))
		return false;
	// N = R7
	// Calculate expressions for the result var
	result = GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, N, result))
		return false;
	// Z = !R7 & !R6 & !R5 & !R4 & !R3 & !R2 & !R1 & !R0       
	// Calculate expressions for the result var
	result = (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 6))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 5))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 4))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 3))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 2))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 1))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 0))        ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, Z, result))
		return false;
	// C = !Rd7 & K7 + K7 & R7 + R7 & !Rd7     
	// Calculate expressions for the result var
	result = (!GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7))  & GetBit(K, 7)  + GetBit(K, 7)  & GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7)  + GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7)  & (!GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7))      ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, C, result))
		return false;

	return true;
}
bool sbci (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int d = 0;
	int d_bits = 4;
	AddBit(&d,opcode,7);
	AddBit(&d,opcode,6);
	AddBit(&d,opcode,5);
	AddBit(&d,opcode,4);

	int K = 0;
	int K_bits = 8;
	AddBit(&K,opcode,11);
	AddBit(&K,opcode,10);
	AddBit(&K,opcode,9);
	AddBit(&K,opcode,8);
	AddBit(&K,opcode,3);
	AddBit(&K,opcode,2);
	AddBit(&K,opcode,1);
	AddBit(&K,opcode,0);



	//Arguments (cast if signed)



	// Execute expressions
	// d = Rd - K - C  
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,d,&error)  - K  - vm_info(state,VM_INFO_REGISTER,C,&error)   ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, d, result))
		return false;
	int R = result;
	// H = !Rd3 & K3 + K3 & R3 + R3 & Rd3     
	// Calculate expressions for the result var
	result = (!GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 3))  & GetBit(vm_info(state,VM_INFO_REGISTER,K,&error), 3)  + GetBit(vm_info(state,VM_INFO_REGISTER,K,&error), 3)  & GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 3)  + GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 3)  & GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 3)      ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, H, result))
		return false;
	// S = N ^ V 
	// Calculate expressions for the result var
	result = vm_info(state,VM_INFO_REGISTER,N,&error)  ^ vm_info(state,VM_INFO_REGISTER,V,&error)  ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, S, result))
		return false;
	// V = Rd7 & !K7 & R7 + Rd7 & K7 & R7     
	// Calculate expressions for the result var
	result = GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7)  & (!GetBit(K, 7))  & GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7)  + GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7)  & GetBit(K, 7)  & GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7)      ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, V, result))
		return false;
	// N = R7
	// Calculate expressions for the result var
	result = GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7) ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, N, result))
		return false;
	// Z = !R7 & !R6 & !R5 & !R4 & !R3 & !R2 & !R1 & !R0 & Z        
	// Calculate expressions for the result var
	result = (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 6))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 5))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 4))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 3))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 2))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 1))  & (!GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 0))  & vm_info(state,VM_INFO_REGISTER,Z,&error)         ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, Z, result))
		return false;
	// C = !Rd7 & K7 + K7 & R7 + R7 & !Rd7     
	// Calculate expressions for the result var
	result = (!GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7))  & GetBit(vm_info(state,VM_INFO_REGISTER,K,&error), 7)  + GetBit(vm_info(state,VM_INFO_REGISTER,K,&error), 7)  & GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7)  + GetBit(vm_info(state,VM_INFO_REGISTER,R,&error), 7)  & (!GetBit(vm_info(state,VM_INFO_REGISTER,d,&error), 7))      ;
	// Check if there was an error in the calculation of the result
	if (error)
		return false;

	if(!vm_write(state, diff, VM_INFO_REGISTER, C, result))
		return false;

	return true;
}
bool sbrc (VMState * state, VMStateDiff *diff, OPCODE_TYPE opcode) {
	// error
	bool error = false;

	// result
	int result = 0;

	

	// Decode the opcode
	int b = 0;
	int b_bits = 3;
	AddBit(&b,opcode,2);
	AddBit(&b,opcode,1);
	AddBit(&b,opcode,0);

	int r = 0;
	int r_bits = 5;
	AddBit(&r,opcode,8);
	AddBit(&r,opcode,7);
	AddBit(&r,opcode,6);
	AddBit(&r,opcode,5);
	AddBit(&r,opcode,4);



	//Arguments (cast if signed)



	// Execute expressions
	if (vm_info(state,VM_INFO_REGISTER,r,&error)  == 0) {
		// PC = PC + 2 
		// Calculate expressions for the result var
		result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 2 ;
		// Check if there was an error in the calculation of the result
		if (error)
			return false;

		if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
			return false;
	}
	else {
		// PC = PC + 1 
		// Calculate expressions for the result var
		result = vm_info(state,VM_INFO_REGISTER,PC,&error)  + 1 ;
		// Check if there was an error in the calculation of the result
		if (error)
			return false;

		if(!vm_write(state, diff, VM_INFO_REGISTER, PC, result))
			return false;
	}
	int R = result;

	return true;
}

OpcodeHandler opcode_handlers[] = {
	{ "noop", 0b0, 0b1, (opcode_handler *) noop,false },
	{ "adc", 0b1110000000000, 0b1111110000000000, (opcode_handler *) adc,false },
	{ "add", 0b110000000000, 0b1111110000000000, (opcode_handler *) add,false },
	{ "adiw", 0b1001011000000000, 0b1111111100000000, (opcode_handler *) adiw,false },
	{ "and", 0b10000000000000, 0b1111110000000000, (opcode_handler *) and,false },
	{ "andi", 0b111000000000000, 0b1111000000000000, (opcode_handler *) andi,false },
	{ "breq", 0b1111000000000001, 0b1111110000000111, (opcode_handler *) breq,false },
	{ "brge", 0b1111010000000100, 0b1111110000000111, (opcode_handler *) brge,false },
	{ "brne", 0b1111010000000001, 0b1111110000000111, (opcode_handler *) brne,false },
	{ "cli", 0b1001010011111000, 0b1111111111111111, (opcode_handler *) cli,false },
	{ "cp", 0b1010000000000, 0b1111110000000000, (opcode_handler *) cp,false },
	{ "cpc", 0b10000000000, 0b1111110000000000, (opcode_handler *) cpc,false },
	{ "cpi", 0b11000000000000, 0b1111000000000000, (opcode_handler *) cpi,false },
	{ "com", 0b1001010000000000, 0b1111111000001111, (opcode_handler *) com,false },
	{ "eor", 0b10010000000000, 0b1111110000000000, (opcode_handler *) eor,false },
	{ "in", 0b1011000000000000, 0b1111100000000000, (opcode_handler *) in,false },
	{ "ldi", 0b1110000000000000, 0b1111000000000000, (opcode_handler *) ldi,false },
	{ "lddyplus", 0b1000000000001000, 0b1101001000001000, (opcode_handler *) lddyplus,false },
	{ "lddzmin", 0b1001000000000010, 0b1111111000001111, (opcode_handler *) lddzmin,false },
	{ "lddzplus", 0b1001000000000001, 0b1111111000001111, (opcode_handler *) lddzplus,false },
	{ "lddzq", 0b1000000000000000, 0b1101001000001000, (opcode_handler *) lddzq,false },
	{ "lds", 0b1001000000000000, 0b1111111000001111, (opcode_handler *) lds,true },
	{ "lsr", 0b1001010000000110, 0b1111111000001111, (opcode_handler *) lsr,false },
	{ "lpm", 0b1001010111001000, 0b1111111111111111, (opcode_handler *) lpm,false },
	{ "mov", 0b10110000000000, 0b1111110000000000, (opcode_handler *) mov,false },
	{ "out", 0b1011100000000000, 0b1111100000000000, (opcode_handler *) out,false },
	{ "push", 0b1001001000001111, 0b1111111000001111, (opcode_handler *) push,false },
	{ "pop", 0b1001000000001111, 0b1111111000001111, (opcode_handler *) pop,false },
	{ "ret", 0b1001010100001000, 0b1111111111111111, (opcode_handler *) ret,false },
	{ "rcall", 0b1101000000000000, 0b1111000000000000, (opcode_handler *) rcall,false },
	{ "rjmp", 0b1100000000000000, 0b1111000000000000, (opcode_handler *) rjmp,false },
	{ "ror", 0b1001010000000111, 0b1111111000001111, (opcode_handler *) ror,false },
	{ "sbiw", 0b1001011100000000, 0b1111111100000000, (opcode_handler *) sbiw,false },
	{ "sbrs", 0b1111111000000000, 0b1111111000001000, (opcode_handler *) sbrs,false },
	{ "stxplus", 0b1001001000001101, 0b1111111000001111, (opcode_handler *) stxplus,false },
	{ "stdyplus", 0b1000001000001000, 0b1111111000001111, (opcode_handler *) stdyplus,false },
	{ "stdyplusq", 0b1000001000001000, 0b1101001000001000, (opcode_handler *) stdyplusq,false },
	{ "stdzplus", 0b1000001000000000, 0b1101001000001000, (opcode_handler *) stdzplus,false },
	{ "subi", 0b101000000000000, 0b1111000000000000, (opcode_handler *) subi,false },
	{ "sbci", 0b100000000000000, 0b1111000000000000, (opcode_handler *) sbci,false },
	{ "sbrc", 0b1111110000000000, 0b1111111000001000, (opcode_handler *) sbrc,false },
	{0}
};
// End of instructions