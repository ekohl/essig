/* Add errnos like __vm_errno__(VM_ERRNO_NAME, "DESCRIPTION") */
__vm_errno__(VM_NO_ERROR, "No error")
__vm_errno__(VM_NO_MEMORY, "No memory")
/* the error message string is returned through strerror() */
__vm_errno__(VM_OSERROR, "") 
