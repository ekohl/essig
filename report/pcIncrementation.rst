PC incrementation
=================

In different micro controllers the program counter either points to the current instruction or the next instruction. Because we delegate program counter incrementation to our instruction handlers both situations can be emulated quite easily by either incrementing the program counter at the beginning of the instruction handler (to indicate the next instruction) or at the end of the handler. 

To illustrate this consider the following two pseudo code implementations of rcall:

// PC points to next instruction

PC = PC + 1

PUSH PC

PC = PC + k

// PC points to current instruction

PUSH (PC + 1)

PC = PC + k

PC = PC + 1

It can be seen that in a micro controller in which the PC points to the next instruction the push operation is simpler.

In the above example the effects of the instruction was exactly the same but in micro controllers in which it is possible to push the PC (This is not so in atmel micro controllers) there can be a difference:

// PC points to next instruction

PC = PC + 1

STORE PC on stack

// PC points to current instruction

STORE PC on stack

PC = PC + 1

Now in the first the top of the stack is the address of the next instruction but in the second the address of the current instruction. It also illustrates that our language can handle both situations because the incrementation of the PC is handled by the instruction handler
