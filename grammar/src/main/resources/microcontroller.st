#include \<stdio.h\>
#include "simulator.h"

// Spec for <name>

// Parameters
<parameters; separator="\n">
// End of parameters

// Registers
int nregisters = <length(registers)>;
char* register_names[] = { <registers:{"<it.name>"}; separator=", "> };
// End of registers

// Instructions
<instructions; separator="\n">

int n_opcode_handlers = <length(instructions)>;
OpcodeHandler opcode_handlers[] = { <instructions:opcodehandler(); separator=", "> };
// End of instructions
