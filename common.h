#pragma once

#include <exec/types.h>

// If FALSE, NULL, or StatusError then exit function with StatusError.
// Writes the failing expression to stderr.
#define ASSERT(EXPR)            \
    if (! (EXPR)) {             \
        print_error(#EXPR);     \
        status = StatusError;   \
        goto cleanup;           \
    }

// If FALSE, NULL, or StatusError then exit function with given status.
#define CHECK(EXPR, STATUS)     \
    if (! (EXPR)) {             \
        status = STATUS;        \
        goto cleanup;           \
    }

#define TRY Status status = StatusOK;
#define FINALLY cleanup:
#define THROW return status;

typedef enum {
    StatusError    = 0, // Must be zero, unifies !(status) handling with FALSE, NULL
    StatusOK       = (1 << 0),
    StatusCommFail = (1 << 1),
} Status;

void print_error(STRPTR msg);
