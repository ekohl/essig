#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "generated_simulator.h"
#include "vm.h"

#ifndef OPCODE_TYPE
#   error The generator should #define OPCODE_TYPE
#endif

/*! All these variables must be set by the generated simulator.
    Additionally the generator should overwrite the file generated_simulator.h
    and define OPCODE_TYPE accordingly.

    On error the opcode handlers should return false. Errnos *must* be defined
    in vmerrno.h, and can be set by assigning to _vm_errno. The vm_write()
    function can be used to operate on the VMState and update the list of 
    diffs. This function returns false on error (errors should be propagated).
    
    For the discussion on opcodes and masks see the redmine forum thread.
    Anything related to pins can be ignored for now.
    Examples:
    \code
        bool square(VMState *state, VMStateDiff *diff, OPCODE_TYPE opcode) {
            OPCODE_TYPE result;
            size_t register_dest;
            
            register_dest = extract register parameter from opcode;
            if (!vm_info(state, VM_INFO_REGISTER, register_dest, &result))
                return false;
            
            return vm_write(state, diff, VM_INFO_REGISTER, 
                            register_dest, result * result);
        }
    
        nbits_cpu = 16;
        register_names = {
            "V",
            "C",
            "R0",
        };
        
        nregisters = 3;
        
        ramsize = 4096;
        
        opcode_handlers = {
            { "square", 0b010110, 0b1010101, square },
        };
        
        ...
    \endcode
*/

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

/*! Holds the register names and offsets */
extern Register registers[];

/*! Indicates the endianness of the MCU */
extern bool is_big_endian;

/* @} */
#endif /* simulator.h */
