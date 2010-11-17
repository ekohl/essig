/* Add errnos like __vm_errno__(VM_ERRNO_NAME, "DESCRIPTION") */
__vm_errno__(VM_NO_ERROR, "No error")
__vm_errno__(VM_NO_MEMORY, "No memory")
/* the error message string is returned through strerror() */
__vm_errno__(VM_OSERROR, "") 
__vm_errno__(VM_INVALID_BREAKPOINT, "Invalid breakpoint: address out of range")
__vm_errno__(VM_NOT_ELF, "Not an ELF file.")
__vm_errno__(VM_NO_SEGMENTS, "ELF file has no segments")
__vm_errno__(VM_MULTIPLE_EXECUTABLE_SEGMENTS, 
             "ELF file has multiple executable segments.")
__vm_errno__(VM_NO_EXECUTABLE_SEGMENT, 
             "ELF file has no executable segments")
__vm_errno__(VM_ERROR_PROGRAM_TOO_BIG, 
             "The program is too big for the microcontroller.")
__vm_errno__(VM_OUT_OF_BOUNDS_ERROR, 
             "Address points outside the address space")
__vm_errno__(VM_NO_SUCH_INTERRUPT_TYPE_SUPPORT_ERROR, 
             "Interrupt type is not supported.")
__vm_errno__(VM_ILLEGAL_INSTRUCTION, 
             "An instruction that could not be handled was encountered")
__vm_errno__(VM_PC_OUT_OF_BOUNDS,
             "Program counter points outside the executable segment.")
__vm_errno__(VM_INTERRUPT_CALLABLE_ERROR,
             "An error occurred in an interrupt callable.")