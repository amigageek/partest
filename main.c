#include "common.h"
#include "parallel.h"
#include "ui.h"

#include <dos/dos.h>
#include <proto/exec.h>
#include <intuition/intuitionbase.h>

Status library_init(void);
void library_fini(void);
void library_close(struct Library** library_p);
Status event_loop(void);

struct Library* ButtonBase = NULL;
struct Library* ChooserBase = NULL;
struct IntuitionBase* IntuitionBase = NULL;
struct Library* LayoutBase = NULL;
struct Library* StringBase = NULL;
struct Library* WindowBase = NULL;

int main() {
    TRY
    ASSERT(library_init());
    ASSERT(parallel_init());
    ASSERT(ui_init());
    ASSERT(event_loop());

    FINALLY
    ui_fini();
    parallel_fini();
    library_fini();

    return (status == StatusOK ? 0 : 20);
}

Status library_init(void) {
    TRY
    ASSERT(ButtonBase = OpenLibrary("gadgets/button.gadget", 0));
    ASSERT(ChooserBase = OpenLibrary("gadgets/chooser.gadget", 0));
    ASSERT(IntuitionBase = (struct IntuitionBase*)OpenLibrary("intuition.library", 36));
    ASSERT(LayoutBase = OpenLibrary("gadgets/layout.gadget", 0));
    ASSERT(StringBase = OpenLibrary("gadgets/string.gadget", 0));
    ASSERT(WindowBase = OpenLibrary("window.class", 0));

    FINALLY
    THROW
}

void library_fini(void) {
    library_close(&WindowBase);
    library_close(&StringBase);
    library_close(&LayoutBase);
    library_close((struct Library**)&IntuitionBase);
    library_close(&ChooserBase);
    library_close(&ButtonBase);
}

void library_close(struct Library** library_p) {
    if (*library_p) {
        CloseLibrary(*library_p);
        *library_p = NULL;
    }
}

Status event_loop(void) {
    TRY
    ULONG break_sigmask = SIGBREAKF_CTRL_C;
    ULONG par_sigmask = parallel_get_sigmask();
    ULONG ui_sigmask;
    ASSERT(ui_get_sigmask(&ui_sigmask));

    for (BOOL running = TRUE; running; ) {
        ULONG signals = Wait(break_sigmask | par_sigmask | ui_sigmask);

        if (signals & break_sigmask) {
            running = FALSE;
        }

        if (signals & par_sigmask) {
            ASSERT(parallel_handle_events());
            ui_set_busy(FALSE);
        }

        if (signals & ui_sigmask) {
            ASSERT(ui_handle_events(&running));
        }
    }

    FINALLY
    THROW
}
