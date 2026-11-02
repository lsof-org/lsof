/* LTnull.c -- public liblsof entry points must reject NULL arguments. */

#include "lsof.h"
#include <stdio.h>

static int expect_invalid(const char *name, enum lsof_error result) {
    if (result == LSOF_ERROR_INVALID_ARGUMENT)
        return 0;
    fprintf(stderr, "%s: expected invalid argument, got %d\n", name, result);
    return 1;
}

int main(void) {
    struct lsof_context *ctx;
    struct lsof_result *result = NULL;
    int failed = 0;

    failed |= expect_invalid("gather(NULL, &result)", lsof_gather(NULL, &result));
    failed |= expect_invalid("gather(NULL, NULL)", lsof_gather(NULL, NULL));
    failed |= expect_invalid("freeze(NULL)", lsof_freeze(NULL));
    failed |= expect_invalid("select_pid(NULL)", lsof_select_pid(NULL, 1, 0));
    failed |= expect_invalid("select_pgid(NULL)", lsof_select_pgid(NULL, 1, 0));

    ctx = lsof_new();
    if (!ctx) {
        fprintf(stderr, "lsof_new failed\n");
        return 1;
    }
    failed |= expect_invalid("gather(ctx, NULL)", lsof_gather(ctx, NULL));
    /* Rejected arguments must not freeze or otherwise invalidate the context. */
    if (lsof_select_pid(ctx, 1, 0) != LSOF_SUCCESS ||
        lsof_select_pgid(ctx, 1, 0) != LSOF_SUCCESS) {
        fprintf(stderr, "context is unusable after rejected gather\n");
        failed = 1;
    }
    lsof_destroy(ctx);
    return failed;
}
