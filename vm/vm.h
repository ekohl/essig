#ifndef VM_H
#define VM_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdarg.h>

#include "vmerrno.h"
#include "generated_simulator.h"

#ifdef VM_WITH_THREADS
#   include <pthread.h>
#   define ACQUIRE_STATE(state) do { \
        pthread_mutex_lock(&state->lock); \
    } while (0)
#   define RELEASE_STATE(state) do { \
        pthread_mutex_unlock(&state->lock); \
    } while (0)
#   define __THREAD __thread
#else
#   define ACQUIRE_STATE(state) do {} while (0)
#   define RELEASE_STATE(state) do {} while (0)
#   define __THREAD
#endif

#define STRINGIFY(msg) #msg
#define TOSTRING(msg) STRINGIFY(msg)
#define LOCATION __FILE__ ":" TOSTRING(__LINE__)
#if VM_DEBUG
#   define print_err(x) puts(x)
#else
#   define print_err(x)
#endif

#define err(result, msg, errno) \
    if (!(bool) (result)) { \
        print_err(LOCATION " " msg); \
        vm_seterrno(errno); \
        goto error; \
    }

#define err_malloc(result) err(result, "malloc", VM_NO_MEMORY)

/*! \mainpage
\section VMMainPage Microcontroller simulator API
\subsection VMMainPublicAPI The public API that will be exposed by the Simulator

The VM is the "generic" code that is incorporated into every generated
simulator. The public API that it exposes allows external programs to link
against the Simulator (the VM + the generated simulator) and use the API. 

The state of the Simulator is managed by a VMState. This needs to be passed 
into most the functions that operate on the state of the microcontroller.

Functions related to execution control (debugger functions like vm_cont(), 
vm_step(), etc) can create Diffs of the VMState to allow efficient updating
of user interfaces and reverse execution (vm_rcont(), vm_rstep()).

Then there are interrupt-related functions (vm_interrupt()), and error reporting functions that help interpret
errnos set by the Simulator (e.g. when a breakpoint is out of range).
The errnos are private, but their value can be retrieved by calling vm_errno(),
and the error message can be retrieved by calling vm_strerror().

\subsection VMMainPrivateAPI The private API (the interface that the generated simulator should export)
The generated simulator exposes microcontroller information by storing it in
predeclared global variables (see \ref VMPrivateAPI "The Private API").
*/

extern __THREAD int _vm_errno;

/*! \defgroup VMDataStructures Data Structures */
/* @{ */
/*! Bools are nice */
typedef enum {
    false,
    true
} bool;

/*! Policy the VM should use with regard to interrupts */
typedef enum {
    VM_POLICY_INTERRUPT_NEVER,
    // VM_POLICY_INTERRUPT_AVOID,
    VM_POLICY_INTERRUPT_ALWAYS,
    // VM_POLICY_INTERRUPT_INTERACTIVE,
} VMInterruptPolicy;

/*! Types of interrupts */
typedef enum {
    VM_INTERRUPT_TIMER,
    /*! Make this the last one */
    VM_N_INTERRUPT_TYPES,
} VMInterruptType;

/*! Flags indicating which part of the VM we want to know something 
    about/do something with */
typedef enum {
    VM_INFO_CHUNK,
    VM_INFO_REGISTER,
    VM_INFO_RAM,
    VM_INFO_ROM,
    VM_INFO_IO,
    VM_INFO_PC,
    VM_INFO_NTYPES,
} VMInfoType;

struct VMState;
struct VMStateDiff;
struct VMInterruptItem;
struct VMBreakpoint;
struct VMInterruptCallable;

/*! Signature of opcode handlers. */
typedef bool opcode_handler(struct VMState *, 
                            struct VMStateDiff *, 
                            OPCODE_TYPE);

typedef bool interrupt_callable(struct VMState *, void *argument);
                            
/*! Each OpcodeHandler corresponds with one instruction handler. */
typedef struct {
    char *opcode_name;
    OPCODE_TYPE opcode;
    OPCODE_TYPE mask;
    opcode_handler *handler;
    /*! Indicates whether the next opcode will be an argument of this opcode.*/
    bool next_is_arg;
} OpcodeHandler;

/*! Each Opcode corresponds to an instruction from the binary code. The code is
    disassembled and represented with entries of this type. */
typedef struct {
    int opcode_index;
    OPCODE_TYPE instruction;
} Opcode;


/*! The state of the VM */
typedef struct VMState {
    /*! Disassembled code from the executable segment.
        FIXME: there may be multiple executable segments in an ELF file.
        Arguments of instructions (and extra word) is represented by a
        no-op Opcode to allow for constant time jumps */
    Opcode *instructions;
    /*! Size of the instructions array */
    size_t instructions_size;
    /*! Offset of the executable segment (in bytes) */
    size_t executable_segment_offset;
    /*! Number of executed cycles */
    unsigned int cycles;
    char *chunk;
    VMInterruptPolicy interrupt_policy;
    struct VMInterruptItem *interrupts;
    struct VMInterruptCallable *interrupt_callables;
    /*! If true, have the interpreter halt execution as soon as possible */
    volatile sig_atomic_t break_async;
    /*! List of breakpoints */
    struct VMBreakpoint *breakpoints;
    /*! Indicates whether the program stopped running (i.e. terminated) */
    bool stopped_running;
    /*! Keep this as the last member in the struct in case people build their
        code without VM_WITH_THREADS and vm.so with VM_WITH_THREADS (or vice
        versa). */
#ifdef VM_WITH_THREADS
    pthread_mutex_t lock;
#endif
} VMState;

/*! Use this macro as the first attribute of a struct to make it an iterable */
#define VMIterable_Head(type) struct type *next

/*! A generic iterable type */
typedef struct VMIterable {
    VMIterable_Head(VMIterable);
} VMIterable;

/*! Single difference in the VMState */
typedef struct VMSingleStateDiff {
    VMIterable_Head(VMSingleStateDiff);
    size_t oldval;
    VMInfoType type;
    size_t location;
    int nbytes;
#ifdef VM_DEBUG
    size_t newval;
#endif
} VMSingleStateDiff;

/*! Item in the interrupt queue. */
typedef struct VMInterruptItem {
    VMIterable_Head(VMInterruptItem);
    VMInterruptType interrupt_type;
    /*! Specifies how many cycles should have passed (in total) before 
        the interrupt handler should be invoked. */
    unsigned int cycles;
} VMInterruptItem;

/*! Represents a single breakpoint */
typedef struct VMBreakpoint {
    VMIterable_Head(VMBreakpoint);
    size_t offset;
} VMBreakpoint;

typedef struct VMInterruptCallable {
    VMIterable_Head(VMInterruptCallable);
    interrupt_callable *func;
    void *argument;
} VMInterruptCallable;

/*! Represents a single register holding the name of the register and the 
    offset in the chunk. This means it does not have to be an actual register.
*/
typedef struct {
   char *name;
   size_t offset;
} Register;

/* include the private API specification */
#include "simulator.h"

/*! Represents a difference in state between two consecutive steps.
    This means it has a list of "single differences" in the state. */
typedef struct VMStateDiff {
    VMIterable_Head(VMStateDiff);
    VMSingleStateDiff *singlediff;
    PC_TYPE pc;
    unsigned int cycles;
} VMStateDiff;


/* @} */

/*! Create a new VMState
    \param[in] program The entire ELF executable read into memory 
    \param[in] program_size Size of the program buffer
*/
VMState *vm_newstate(void *program,
                     size_t program_size, 
                     VMInterruptPolicy interrupt_policy);
/*! Create a new VMState without any kind of instructions. */
VMState *vm_newstate_no_code(VMInterruptPolicy interrupt_policy);

/*! Create a new diff that can be passed to functions like vm_cont() and 
    vm_step() */
VMStateDiff *vm_newdiff(void);

/*! Deallocate a VMState */
void vm_closestate(VMState *);
/*! Deallocate a VMStateDiff */
void vm_closediff(VMStateDiff *);

/*! Print a diff if this is a debug build */
void vm_print_diff(VMState *, VMStateDiff *);

/*! \defgroup VMDEBUGGER  Debugger Functions */
/* @{ */

/*! Set a breakpoint
    \param state state The state of the VM
    \param[in] code_offset offset in the instructions table
    \return zero if successfull, nonzero otherwise, with a VM errno set
*/
bool vm_break(VMState *state, size_t code_offset);
/*! Resume execution until a breakpoint is met.
    \param state The state of the VM
    \param[out] diff If not NULL, keep track of differences in the VMState
*/
bool vm_cont(VMState *state, VMStateDiff **diff, bool *hit_bp);
/*! Start executing a program. This is like vm_cont, except that it doesn't
    run an instruction before checking for breakpoints. vm_cont() does run an
    instruction first, otherwise it would never pass any breakpoint it hit. */
bool vm_run(VMState *state, VMStateDiff **diff, bool *hit_bp);

/*! Resume execution in reverse order (could also do snapshots if we want)
    \param state The state of the VM
    \param[in] diff Each step applies one diff until we reach the beginning
               or until we hit a breakpoint.
    \return a pointer to the current diff.
     */
void vm_rcont(VMState *state, VMStateDiff **diff, bool *hit_bp);

/*! Step n steps
    \param state The state of the VM
    \param[in] nsteps Number of steps to take
    \param[out] diff If not NULL, populate with the difference for each step.
                     Also updates the list to point to the last element in the
                     reversed diff chain by doing pointer assignment.
    
    For every step, if diff is not NULL, a new diff is allocated and that diff
    is populated. So if a diff list should be created, the following steps
    should be taken:
    
        allocate the first diff using vm_newdiff; this diff is not used
        step using the first diff
        
    Then the diff list can be unwound until the first diff is encountered, 
    in which case in can be unwound no further.
*/
bool vm_step(VMState *state, unsigned long nsteps, VMStateDiff **diff, 
             bool *hit_bp);
/*! Reverse step. The list of diffs must be in reverse order and must be at 
    least as long as nsteps. This function cleans diffs that are used during the 
    reverse stepping and returns a pointer to the current VMStateDiff. */
void vm_rstep(VMState *state, unsigned long nsteps, VMStateDiff **diff, 
              bool *hit_bp);

/*! Set a breakpoint asynchronously when the interpreter is running. This
    function is async-signal-safe. */
void vm_break_async_from_signal(VMState *state);
/*! Set a breakpoint asynchronously when the interpreter is running. This
    function is thread-safe. */
void vm_break_async_from_thread(VMState *state);
/* @} */



/*! 
Interrupt the microcontroller
\param state The state of the VM
\param[in] type The type of interrupt
\param[in] ... Extra arguments depending on the type of interrupt
*/
bool vm_interrupt(VMState *state, VMInterruptType type, ...);

/*! Register an interrupt handler in a VMState with an extra argument */
bool vm_register_interrupt_callable(VMState *, interrupt_callable *, void *);
/* @} */

/*! Query the VM for information. 
    \param[out] errorp if not NULL, indicates whether there was an error during
		the operation. */
BIGTYPE vm_info(VMState *state, VMInfoType type, size_t vmaddr, bool *errorp);
/*! Write a value to a destination of type 'type' at addr 'destaddr'. 
    This function updates 'state' and 'diff' appropriately. 
    On error, this function returns 'false' with the VM errno set 
    appropriately (opcode handlers can just propagate this error by returning
    'false'). */
bool vm_write(VMState *state, VMStateDiff *diff, VMInfoType type, 
              size_t destaddr, BIGTYPE value);

BIGTYPE vm_info_nbytes(VMState *state, VMInfoType type, size_t vmaddr, 
                       bool *errorp, int nbytes);
bool vm_write_nbytes(VMState *state, VMStateDiff *diff, VMInfoType type, 
                     size_t destaddr, BIGTYPE value, int nbytes);

/*! \defgroup VMERRNO Error Reporting */
/* @{ */

/*! Retrieve the VM errno. */
int vm_errno(void);

/*! Turns a VM errno into an error message. */
char *vm_strerror(int err);

/*! Useful to break in when debugging. Sets the VM errno */
void vm_seterrno(int err);

/* @} */

/*! Return a reversed version of an iterable. Mutates inplace. */
VMIterable *vm_reversed_it(VMIterable *it);

/*! Convert value from the MCU's architecture to the host architecture.
    Modifies in-place. */
void vm_convert_endianness(char *value, size_t length);

/*! Convert unsigned two-complement's value of nbits to signed. */
BIGTYPE vm_convert_to_signed(BIGTYPE value, int nbits);

Opcode *get_opcode(VMState *state, PC_TYPE pc);

/*! \defgroup User-written code */
/* @{ */
/*! Interrupts */
bool set_interrupt(VMState *state, VMStateDiff *diff);
bool handle_interrupt(VMState *state, VMStateDiff *diff);
/* @} */

#endif /* vm.h */
