Interrupt Handling
==================

Interrupts in essig can occur at two different levels:

1. From outside of the VM through the vm_interrupt method 

2. From a peripharel

In either case a policy can govern whether or not the interrupt will be passed on to the simulator

The simulator than enforces the actual rules for the microcontroller that is simulated through a call to the handle_interrupt method

After that the vm resumes normal execution 
