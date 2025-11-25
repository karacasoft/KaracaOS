#include <proc/proc.h>

static proc_info_t proc__processes[MAX_PROC] = {0};

static uint32_t proc__next_proc_id = 0;

uint32_t get_next_proc_id(uint32_t current) {
    while(proc__processes[current].exists) {
        current++;
        if(current == MAX_PROC) {
            current = 0;
        }
    }
    return current;
}

SYS_RET proc_create_process(uint32_t *id, uint8_t priority) {
    *id = proc__next_proc_id;

    proc__processes[proc__next_proc_id].exists = TRUE;
    proc__processes[proc__next_proc_id].id = proc__next_proc_id;
    proc__processes[proc__next_proc_id].flags = 0;
    proc__processes[proc__next_proc_id].priority = 0;
    proc__processes[proc__next_proc_id].irq = 0;

    proc__next_proc_id = get_next_proc_id(proc__next_proc_id);

    return SYS_RET_NO_ERROR;
}

SYS_RET proc_switch_context(uint32_t current, uint32_t next) {
    void *curr_state_ptr = (void *) &(proc__processes[current].state);

    
}
