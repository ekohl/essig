#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "generated_simulator.h"

#define BIGTYPE int64_t
#define UBIGTYPE uint64_t

#ifndef CHUNK_OFFSET

#define CHUNK_OFFSET 0
#define CHUNK_END ROM_END
#define SIZEOF_CHUNK 1

#define REGISTER_OFFSET 0
#define REGISTER_END IO_END
#define SIZEOF_REGISTER 1

#define RAM_OFFSET 0x60
#define RAM_END (RAM_OFFSET + 1024)
#define SIZEOF_RAM 1

#define ROM_OFFSET (RAM_END + SIZEOF_PC)
#define ROM_END (ROM_OFFSET + 0xFFFFFF)
#define SIZEOF_ROM 2

#define IO_OFFSET REGISTER_END
#define IO_END 0x60
#define SIZEOF_IO 1

#define PC_OFFSET RAM_END
#define SIZEOF_PC 2

//#error
#endif

#ifndef OPCODE_TYPE
#   error The generator should #define OPCODE_TYPE
#endif

#if SIZEOF_PC == 1
#   define PC_TYPE uint8_t
#elif SIZEOF_PC == 2
#   define PC_TYPE uint16_t
#elif SIZEOF_PC <= 4
#   define PC_TYPE uint32_t
#else
#   define PC_TYPE uint64_t
#endif

#define GETPC(state) ((PC_TYPE) vm_info(state, VM_INFO_PC, 0, NULL))
#define SETPC(state, value) vm_write(state, NULL, VM_INFO_PC, 0, value)
#define GETBIT(value, bitpos) (!!((value) & (1 << (bitpos))))
#define SETBIT(value, bitpos, bitvalue) ((bitvalue) ? \
    ((value) | ((bitvalue) << (bitpos))) : \
    ((value) & ~(1 << (bitpos))))

#include "vm.h"

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
            BIGTYPE result;
            size_t register_dest;
            bool error = false;
            
            register_dest = extract register parameter from opcode;
            result = vm_info(state, VM_INFO_REGISTER, register_dest, &error))
            
            if (error)
                return false;
            
            return vm_write(state, diff, VM_INFO_REGISTER, 
                            register_dest, result * result);
        }
    
        nbits_cpu = 16;
        register_names = {
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

/*! NULL-terminated list of opcode handlers. */
extern OpcodeHandler opcode_handlers[];

/*! Amount of registers (or other variables like PC and SP) in the 
    microcontroller */
extern int nregisters;

/*! Holds the register/variable names and offsets.
    offsets are specified in CHUNK addressing mode. */
extern Register registers[];

/*! Indicates the endianness of the MCU */
extern bool is_big_endian;

/* @} */
#endif /* simulator.h */
