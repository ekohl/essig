#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "vm.h"

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
