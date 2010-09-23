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
extern int nregisters;

/*! Array holding the names of the pins */
extern char *pin_names[];
 /*! Amount of pins in the microcontroller */
extern int npins;

/*! Size of RAM available in the microcontroller */
extern size_t ramsize;         

typedef void (*instruction_handler)(VMState *state, VMStateDiff *diff);
/*! Table that maps instructions to functions ("instruction handlers") */
extern instruction_handler instruction_to_function[];
/*! n instructions for this microcontroller */
extern int ninstructions;

/* @} */
#endif /* simulator.h */