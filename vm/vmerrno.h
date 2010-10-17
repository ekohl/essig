/* Add errnos like __vm_errno__(VM_ERRNO_NAME, "DESCRIPTION") */
__vm_errno__(VM_NO_ERROR, "No error")
__vm_errno__(VM_NO_MEMORY, "No memory")
/* the error message string is returned through strerror() */
__vm_errno__(VM_OSERROR, "") 
__vm_errno__(VM_INVALID_BREAKPOINT, "Invalid breakpoint: address out of range")
__vm_errno__(VM_NOT_ELF, "Not an ELF file.")
__vm_errno__(VM_NO_SEGMENTS, "ELF file has no segments")
__vm_errno__(VM_MULTIPLE_EXECUTABLE_SEGMENTS, "ELF file has multiple "
             "executable segments. That is clearly too much trouble hehehe.")
          