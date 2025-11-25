#ifndef __PROC_H__
#define __PROC_H__ 1

#include <stdint.h>
#include <kernel/sysdefs.h>
#include <arch/arch_proc/arch_proc.h>

#define MAX_PROC 32768

typedef struct __proc_t proc_info_t;

struct __proc_t {
    BOOLEAN exists;
    uint32_t id;
    uint32_t flags; // TODO
    uint8_t priority;
    uint8_t irq;
    proc_state_t state;
};

#endif // __PROC_H__
