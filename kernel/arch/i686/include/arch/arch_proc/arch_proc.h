#ifndef __ARCH__PROC_H__
#define __ARCH__PROC_H__

#include <stdint.h>
#include <kernel/sysdefs.h>

typedef struct __proc_state_t proc_state_t;

struct __proc_state_t {
    uint32_t CR3;
    uint32_t EIP;
    uint32_t EFLAGS;
    uint32_t EAX;
    uint32_t EBX;
    uint32_t ECX;
    uint32_t EDX;
    uint32_t ESP;
    uint32_t EBP;
    uint32_t ESI;
    uint32_t EDI;
    uint16_t CS;
    uint16_t DS;
    uint16_t ES;
    uint16_t FS;
    uint16_t GS;
    uint16_t SS;
};

SYS_RET arch_proc__save_proc_state(void *data);
SYS_RET arch_proc__restore_proc_state(void *data);

#endif // __ARCH__PROC_H__