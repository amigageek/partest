#include "parallel.h"

#include <devices/parallel.h>
#include <proto/exec.h>

static struct {
    struct MsgPort* par_mp;
    struct IOExtPar* par_io;
    BOOL par_open;
    ParallelMode par_mode;
} g;

Status parallel_init(void) {
    TRY
    ASSERT(g.par_mp = CreateMsgPort());
    ASSERT(g.par_io = (struct IOExtPar*)CreateIORequest(g.par_mp, sizeof(struct IOExtPar)));
    ASSERT(OpenDevice(PARALLELNAME, 0, (struct IORequest*)g.par_io, 0) == 0);
    g.par_open = TRUE;
    ASSERT(parallel_set_mode(PM_Ack));

    FINALLY
    THROW
}

void parallel_fini(void) {
    if (g.par_open) {
        AbortIO((struct IORequest*)g.par_io);
        WaitIO((struct IORequest*)g.par_io);
        CloseDevice((struct IORequest*)g.par_io);
        g.par_open = FALSE;
    }

    if (g.par_io) {
        DeleteIORequest(g.par_io);
        g.par_io = NULL;
    }

    if (g.par_mp) {
        DeleteMsgPort(g.par_mp);
        g.par_mp = NULL;
    }
}

ULONG parallel_get_sigmask(void) {
    return 1UL << g.par_mp->mp_SigBit;
}

Status parallel_handle_events(void) {
    TRY
    ASSERT(WaitIO((struct IORequest*)g.par_io) == 0);

    FINALLY
    THROW
}

ParallelMode parallel_get_mode(void) {
    return g.par_mode;
}

Status parallel_set_mode(ParallelMode par_mode) {
    TRY
    if (par_mode != g.par_mode) {
        g.par_mode = par_mode;
        g.par_io->io_ParFlags &= ~(PARF_ACKMODE | PARF_FASTMODE);
        g.par_io->io_ParFlags |= (par_mode == PM_Ack ? PARF_ACKMODE : PARF_FASTMODE);
        g.par_io->IOPar.io_Command = PDCMD_SETPARAMS;
        ASSERT(DoIO((struct IORequest*)g.par_io) == 0);
    }

    FINALLY
    THROW
}

void parallel_send(STRPTR text) {
    g.par_io->IOPar.io_Length = -1;
    g.par_io->IOPar.io_Data = text;
    g.par_io->IOPar.io_Command = CMD_WRITE;
    SendIO((struct IORequest*)g.par_io);
}
