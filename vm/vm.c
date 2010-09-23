#include "vm.h"

enum _vmerrno {
    #define __vm_errno__(a,b) a,
    #include "vmerrno.h"
    #undef __vm_errno__
    VM_ERROR_NUM
};

static char *vm_error_messages[] = { 
    #define __vm_errno__(a,b) b,
    #include "vmerrno.h"
    #undef __vm_errno__
};

