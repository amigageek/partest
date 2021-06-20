#include "ui.h"
#include "parallel.h"

#include <classes/window.h>
#include <gadgets/button.h>
#include <gadgets/chooser.h>
#include <gadgets/layout.h>
#include <gadgets/string.h>
#include <proto/button.h>
#include <proto/chooser.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/layout.h>
#include <proto/string.h>
#include <proto/window.h>
#include <reaction/reaction.h>
#include <reaction/reaction_macros.h>

#include <stdio.h>

typedef enum {
    OI_Window,
    OI_ParMode,
    OI_ParText,
    OI_Status,
    OI_Last
} ObjectId;

static struct {
    struct MsgPort* app_port;
    Object* objects[OI_Last];
    struct Window* window;
} g;

static STRPTR parmode_labels[] = {"Ack", "Fast", NULL};

Status ui_init(void) {
    TRY
    ASSERT(g.app_port = CreateMsgPort());

    g.objects[OI_Window] = WindowObject,
        WA_ScreenTitle, (ULONG)"Parallel Test",
        WA_Title, (ULONG)"ParTest",
        WA_Activate, TRUE,
        WA_DragBar, TRUE,
        WA_DepthGadget, TRUE,
        WA_CloseGadget, TRUE,
        WA_SizeGadget, TRUE,
        WINDOW_IconifyGadget, TRUE,
        WINDOW_IconTitle, (ULONG)"ParTest",
        WINDOW_AppPort, (ULONG)g.app_port,
        WINDOW_Position, WPOS_CENTERSCREEN,
        WINDOW_ParentGroup, HGroupObject,
            LAYOUT_DeferLayout, TRUE,
            LAYOUT_SpaceOuter, TRUE,
            LAYOUT_VertAlignment, LALIGN_CENTER,

            LAYOUT_AddChild, g.objects[OI_ParMode] = ChooserObject,
                GA_ID, OI_ParMode,
                GA_RelVerify, TRUE,
                CHOOSER_PopUp, TRUE,
                CHOOSER_LabelArray, parmode_labels,
                CHOOSER_Selected, parallel_get_mode(),
                CHOOSER_AutoFit, TRUE,
            ChooserEnd,
            CHILD_WeightedWidth, 0,

            LAYOUT_AddChild, g.objects[OI_ParText] = StringObject,
                GA_ID, OI_ParText,
                GA_RelVerify, TRUE,
                STRINGA_MinVisible, 16,
            StringEnd,

            LAYOUT_AddChild, g.objects[OI_Status] = ButtonObject,
                GA_ID, OI_Status,
                GA_ReadOnly, TRUE,
                BUTTON_DomainString, (ULONG)"       ",
            FuelGaugeEnd,
            CHILD_WeightedWidth, 0,
        EndGroup,
    EndWindow;

    ASSERT(g.objects[OI_Window]);

    ui_set_busy(FALSE);
    ASSERT(g.window = RA_OpenWindow(g.objects[OI_Window]));

    FINALLY
    THROW
}

void ui_fini(void) {
    if (g.objects[OI_Window]) {
        DisposeObject(g.objects[OI_Window]);
        g.objects[OI_Window] = NULL;
    }

    if (g.app_port) {
        DeleteMsgPort(g.app_port);
        g.app_port = NULL;
    }
}

Status ui_get_sigmask(ULONG* sigmask_p) {
    TRY
    ASSERT(GetAttr(WINDOW_SigMask, g.objects[OI_Window], sigmask_p));
    *sigmask_p |= 1UL << g.app_port->mp_SigBit;

    FINALLY
    THROW
}

Status ui_handle_events(BOOL* running_p) {
    TRY
    ULONG result;
    UWORD code;

    while ((result = RA_HandleInput(g.objects[OI_Window], &code)) != WMHI_LASTMSG) {
        switch (result & WMHI_CLASSMASK) {
        case WMHI_CLOSEWINDOW:
            *running_p = FALSE;
            break;
        
        case WMHI_ICONIFY:
            RA_Iconify(g.objects[OI_Window]);
            g.window = NULL;
            break;

        case WMHI_UNICONIFY:
            ASSERT(g.window = RA_OpenWindow(g.objects[OI_Window]));
            break;
        
        case WMHI_GADGETUP:
            switch (result & WMHI_GADGETMASK) {
            case OI_ParMode:
                parallel_set_mode(code);
                break;

            case OI_ParText:
                ui_set_busy(TRUE);
                {
                    STRPTR par_text;
                    ASSERT(GetAttr(STRINGA_TextVal, g.objects[OI_ParText], (ULONG*)&par_text));
                    parallel_send(par_text);
                }
                break;
            }
        }
    }

    FINALLY
    THROW
}

void ui_set_busy(BOOL busy) {
    SetGadgetAttrs((struct Gadget*)g.objects[OI_ParMode], g.window, NULL, GA_Disabled, busy, TAG_END);
    SetGadgetAttrs((struct Gadget*)g.objects[OI_ParText], g.window, NULL, GA_Disabled, busy, TAG_END);
    SetGadgetAttrs((struct Gadget*)g.objects[OI_Status], g.window, NULL,
        GA_Text, busy ? "Sent" : "Ready",
        BUTTON_BackgroundPen, busy ? ~0 : BACKGROUNDPEN, TAG_END);
}
