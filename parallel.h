#pragma once

#include "common.h"

#include <exec/ports.h>

typedef enum {
    PM_Ack,
    PM_Fast
} ParallelMode;

Status parallel_init(void);
void parallel_fini(void);
ULONG parallel_get_sigmask(void);
Status parallel_handle_events(void);
ParallelMode parallel_get_mode(void);
Status parallel_set_mode(ParallelMode par_mode);
void parallel_send(STRPTR text);
