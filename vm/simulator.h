#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "generated_simulator.h"
#include "vm.h"

#ifndef OPCODE_TYPE
# define OPCODE_TYPE unsigned int
//# error The generator should #define OPCODE_TYPE
#endif

/*! \defgroup VMPrivateAPI Private API */
/* @{ */

/*! n-bit instruction set */
extern int nbits_cpu;

/*! Table holding the names of the registers */
extern char *register_names[];

/*! Amount of registers in the microcontroller */
extern int nregisters;	// FIXME n_registers

/*! Array holding the names of the pins */
extern char *pin_names[];
 /*! Amount of pins in the microcontroller */
extern int npins;

/*! Offset of the pins in the RAM */
extern size_t pinoffset;

/*! Size of RAM available in the microcontroller */
extern size_t ramsize;         

/*! Function that executes a single instruction */
extern OpcodeHandler opcode_handlers[];

/*! Amount of opcode handlers in the microcontroller */
extern int n_opcode_handlers;

/*! Function that handles an interrupt */
extern bool interrupt_handler(VMState *state, VMStateDiff *diff);

/* @} */
#endif /* simulator.h */
