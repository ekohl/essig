#include <elf.h>
#include <string.h>

/* Note: this file is included by vm.c to support both 32 and 64 bit ELF files.
         Alternatively, we could have used libelf or BFD, but this seems easy
         enough.
*/

/* Load the loadable segments of the program into the VMState. Disassemble
   The Executable Segment. */
static bool
_elf32_read(VMState *state, char *program, size_t program_size)
{
    Elf32_Ehdr *ehdr;
    Elf32_Phdr *phdr;
    char *ram;
    int i;
    
    ram = (char *) state->ram;
    ehdr = (Elf32_Ehdr *) program;
    
    state->pc = ehdr->e_entry;

    if (!ehdr->e_phoff) {
        _vm_errno = VM_NO_SEGMENTS;
        return false;
    }
    
    phdr = (Elf32_Phdr *) (program + ehdr->e_phoff);
    for (i = 0; i < ehdr->e_phnum; i++) {
        phdr++;
        
        if (phdr->p_type & PT_LOAD) {
            char *startaddr;
    
            startaddr = ram + phdr->p_vaddr;
    
            if (phdr->p_flags & PF_X) {
                /* executable segment, disassemble */
                
                if (state->instructions) {
                    /* Will there ever be multiple executable segments?
                       Text and data segments may both be executable. */
                    _vm_errno = VM_MULTIPLE_EXECUTABLE_SEGMENTS;
                    return false;
                }
                state->instructions = disassemble(
                    (unsigned int *) (program + phdr->p_offset),
                    phdr->p_filesz);
            }
            
            if (startaddr + phdr->p_memsz > ram + ramsize ||
                startaddr + phdr->p_memsz < ram) {
                _vm_errno = VM_ERROR_PROGRAM_TOO_BIG;
                return false;
            }
            
            /* load segment into the RAM of the VM */
            memcpy(startaddr, program + phdr->p_offset, phdr->p_filesz);
            
            /* NUL the rest */
            memset(startaddr + phdr->p_filesz, 0, 
                   phdr->p_memsz - phdr->p_filesz);
        }
    }
    return true;
}