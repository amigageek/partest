#include "common.h"

#include <proto/intuition.h>
#include <stdio.h>

void print_error(STRPTR msg) {
    if (IntuitionBase) {
        struct EasyStruct easy = {
            .es_StructSize = sizeof(struct EasyStruct),
            .es_Flags = 0,
            .es_Title = "Parallel Test",
            .es_TextFormat = msg,
            .es_GadgetFormat = "OK",
        };

        EasyRequest(NULL, &easy, NULL);
    }
    else {
        fprintf(stderr, "partest: assert(%s) failed\n", msg);
    }
}
