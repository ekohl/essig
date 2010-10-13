#ifndef VM_H
#define VM_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#ifdef VM_WITH_THREADS
#   include <pthread.h>
#endif

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
    VM_POLICY_INTERRUPT_AVOID,
    VM_POLICY_INTERRUPT_ALWAYS,
    VM_POLICY_INTERRUPT_RANDOM,
    VM_POLICY_INTERRUPT_INTERACTIVE,
    VM_POLICY_INTERRUPT_SCRIPTED
} VMInterruptPolicy;

/*! Types of interrupts */
typedef enum {
    VM_INTERRUPT_TIMER,
} VMInterruptType;

/*! Flags indicationg which part of the VM we want to know something 
    about/do something with */
typedef enum {
    VM_INFO_REGISTER,
    VM_INFO_RAM,
    VM_INFO_PIN
} VMInfoType;

struct VMState;
struct VMStateDiff;
struct VMInterruptItem;
struct VMBreakpoint;

typedef bool opcode_handler(struct VMState *, 
                            struct VMStateDiff *, 
                            unsigned int);

typedef struct {
    char *opcode_name;
    unsigned int opcode;
    unsigned int mask;
    opcode_handler *handler;
} OpcodeHandler;

typedef struct {
    int opcode_index;
    unsigned int opcode;
} Opcode;


/*! The state of the VM */
typedef struct VMState {
    Opcode *instructions;
    size_t instructions_size;
    size_t pc;
    void *ram;
    void *registers;
    VMInterruptPolicy interrupt_policy;
    struct VMInterruptItem *interrupt_queue;
#ifdef VM_WITH_THREADS
    /*! Make sure this mutex is recursive */
    pthread_mutex_t interrupt_queue_lock;
#endif
    bool break_async;
    struct VMBreakpoint *breakpoints;
} VMState;

/*! Use this macro as the first attribute of a struct to make it an iterable */
#define VMIterable_Head struct VMIterable *next
/*! A generic iterable type */
typedef struct VMIterable {
    VMIterable_Head;
} VMIterable;

/*! Single difference in the VMState */
typedef struct VMSingleStateDiff {
    VMIterable_Head;
    size_t oldval;
    size_t newval;
    VMInfoType type;
    size_t location;
} VMSingleStateDiff;

/*! Item in the interrupt queue */
typedef struct VMInterruptItem {
    VMIterable_Head;
    VMInterruptType interrupt_type;
    /*! This attribute can be set to anything and semantics depends on
        the interrupt_type */
    void *extra_arg;
} VMInterruptItem;

/*! Represents a difference in state between two consecutive steps.
    This means it has a list of "single differences" in the state
    (or is there only ever one difference with our ISA?) */
typedef struct VMStateDiff {
    VMIterable_Head;
    VMSingleStateDiff *singlediff;
} VMStateDiff;  

typedef struct VMBreakpoint {
    VMIterable_Head;
    size_t offset;
} VMBreakpoint;


/* @} */

/*! Create a new VMState */
VMState *vm_newstate(void *instructions, 
                     size_t instructions_size, 
                     VMInterruptPolicy interrupt_policy);
/*! Create a new diff that can be passed to functions like vm_cont() and 
    vm_step() */
VMStateDiff *vm_newdiff(void);

/*! Deallocate a VMState */
void vm_closestate(VMState *);
/*! Deallocate a VMStateDiff */
void vm_closediff(VMStateDiff *);

// #ifdef VM_WITH_THREADS
/*! Lock the interrupt queue */
void vm_acquire_interrupt_queue(VMState *);
/*! Release the interrupt queue */
void vm_release_interrupt_queue(VMState *);
// #endif

/*! \defgroup VMDEBUGGER  Debugger Functions */
/* @{ */
/*! Set a breakpoint
    \param state state The state of the VM
    \param[in] code_offset offset in the instructions table
    \return zero if successfull, nonzero otherwise, with a VM errno set
*/
bool vm_break(VMState *state, size_t code_offset);
/*! Resume execution until a breakpoint is met
    \param state The state of the VM
    \param[out] diffs If not NULL, keep track of differences in the VMState
*/
bool vm_cont(VMState *state, VMStateDiff *diffs, bool *hit_bp);
/*! Resume execution in reverse order (could also do snapshots if we want)
    \param state The state of the VM
    \param[in] diffs Each step applies one diff until we reach the beginning
    or until we hit a breakpoint */
void vm_rcont(VMState *state, VMStateDiff *diffs, bool *hit_bp);
/*! Step n steps
    \param state The state of the VM
    \param[in] nsteps Number of steps to take
    \param[out] diffs If not NULL, populate with the difference for each step
*/
bool vm_step(VMState *state, int nsteps, VMStateDiff *diffs, bool *hit_bp);
bool vm_rstep(VMState *state, int nsteps, VMStateDiff *diffs, bool *hit_bp);

/*! Set a breakpoint asynchronously when the interpreter is running */
void vm_break_async(VMState *state);
/* @} */



/*! 
Interrupt the microcontroller
\param state The state of the VM
\param[in] type The type of interrupt
\param[in] ... Extra arguments depending on the type of interrupt
*/
void vm_interrupt(VMState *state, VMInterruptType type, ...);
/* @} */

/*! Query the VM for information */
int vm_info(VMState *state, VMInfoType type, size_t vmaddr);

/*! \defgroup VMERRNO Error Reporting */
/* @{ */

/*! Retrieve the VM errno. */
int vm_errno(void);

/*! Turns a VM errno into an error message. */
char *vm_strerror(int err);

/* @} */

/* include the private API specification */
#include "simulator.h"

#endif /* vm.h */
