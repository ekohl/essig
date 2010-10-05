#ifndef VM_H
#define VM_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

/*! Item in the interrupt queue */
typedef struct VMInterruptItem {
    VMInterruptType interrupt_type;
    /*! This attribute can be set to anything and semantics depends on
        the interrupt_type */
    void *extra_arg;
    struct VMInterruptItem *next;
} VMInterruptItem;

/*! The state of the VM */
typedef struct VMState {
    void *instructions;
    void *current_instruction;
    void *ram;
    void *registers;
    VMInterruptPolicy interrupt_policy;
    VMInterruptItem *interrupt_queue;
#ifdef VM_WITH_THREADS
    /*! Make sure this mutex is recursive */
    pthread_mutex_t interrupt_queue_lock;
#endif
    bool break_async;
} VMState;

/*! Single difference in the VMState */
typedef struct VMSingleStateDiff {
    size_t oldval;
    size_t newval;
    VMInfoType type;
    size_t location;
    struct VMSingleStateDiff *next;
} VMSingleStateDiff;

/*! Represents a difference in state between two consecutive steps.
    This means it has a list of "single differences" in the state
    (or is there only ever one difference with our ISA?) */
typedef struct VMStateDiff {
    VMSingleStateDiff *singlediff;
    struct VMStateDiff *next;
} VMStateDiff;

/* @} */

/*! Create a new VMState */
VMState *vm_newstate(void *instructions, VMInterruptPolicy interrupt_policy);
/*! Create a new diff that can be passed to functions like vm_cont() and 
    vm_step() */
VMStateDiff *vm_newdiff(void);

#ifdef VM_WITH_THREADS
/*! Lock the interrupt queue */
void vm_acquire_interrupt_queue(VMState *);
/*! Release the interrupt queue */
void vm_release_interrupt_queue(VMState *);
#endif

/*! \defgroup VMDEBUGGER  Debugger Functions */
/* @{ */
/*! Set a breakpoint
    \param code_offset[in] offset in the instructions table */
int vm_break(VMState *state, size_t code_offset);
/*! Resume execution until a breakpoint is met
    \param diffs[out] If not NULL, keep track of differences in the VMState
*/
void vm_cont(VMState *state, VMStateDiff *diffs);
/*! Resume execution in reverse order (could also do snapshots if we want)
    \param diffs[in] Each step applies one diff until we reach the beginning 
    or until we hit a breakpoint */
void vm_rcont(VMState *state, VMStateDiff *diffs);
/*! Step n steps
    \param nsteps[in] Number of steps to take
    \param diffs[out] If not NULL, populate with the difference for each 
                      step 
*/
void vm_step(VMState *state, int nsteps, VMStateDiff *diffs);
void vm_rstep(VMState *state, int nsteps, VMStateDiff *diffs);

/*! Set a breakpoint asynchronously when the interpreter is running */
void vm_break_async(VMState *state);
/* @} */



/*! 
Interrupt the microcontroller
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
char *vm_strerror(int vm_errno);

/* @} */

/* include the private API specification */
#include "simulator.h"

#endif /* vm.h */
