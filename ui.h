#pragma once

#include "common.h"

Status ui_init(void);
void ui_fini(void);
Status ui_get_sigmask(ULONG* sigmask_p);
Status ui_handle_events(BOOL* running_p);
void ui_set_busy(BOOL busy);
