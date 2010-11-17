#ifndef VMERRNO_H
#define VMERRNO_H

enum _vmerrno {
#   define __vm_errno__(a,b) a,
#   include "vmerrors.h"
#   undef __vm_errno__
    VM_ERROR_NUM
};

#endif /* !VMERRNO_H */