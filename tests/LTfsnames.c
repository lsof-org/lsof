/* LTfsnames.c -- filesystem-name ownership in file record cleanup. */

#include "common.h"

static char *names[2];

static void tracked_free(void *ptr) {
    int i;
    for (i = 0; i < 2; i++) {
        if (ptr && ptr == names[i])
            names[i] = NULL;
    }
    free(ptr);
}

/* No kernel access: leave the test's orphaned file for lsof_gather to free. */
static void test_gather_proc_info(struct lsof_context *ctx) { (void)ctx; }

/* Intercept only these implementations' frees, without production test hooks
 * or platform-specific allocator/linker support. Link other sources normally. */
#define free tracked_free
#define gather_proc_info test_gather_proc_info
#include "../lib/proc.c"
#include "../lib/lsof.c"
#undef gather_proc_info
#undef free

static void set_names(struct lfile *lf) {
    names[0] = lf->fsdir = mkstrcpy("/mountpoint", NULL);
    names[1] = lf->fsdev = mkstrcpy("/dev/filesystem", NULL);
    if (!names[0] || !names[1]) {
        fprintf(stderr, "cannot allocate filesystem names\n");
        exit(1);
    }
}

static int expect_freed(const char *test) {
    int i, failed = 0;
    for (i = 0; i < 2; i++) {
        if (names[i]) {
            fprintf(stderr, "FAIL: %s did not free %s\n", test,
                    i ? "fsdev" : "fsdir");
            failed = 1;
            /* Reclaim missed allocations so all cases can run on broken code. */
            CLEAN(names[i]);
        }
    }
    if (!failed)
        printf("PASS: %s frees both filesystem names\n", test);
    return failed;
}

int main(int argc, char *argv[]) {
    struct lsof_context *ctx = lsof_new();
    struct lproc lp = {0};
    struct lsof_result *result = NULL;
    int failed = 0;
    (void)argc;
    (void)argv;
    if (!ctx) {
        fprintf(stderr, "lsof_new failed\n");
        return 1;
    }
    ctx->cur_proc = &lp;

    alloc_lfile(ctx, LSOF_FD_NUMERIC, 0);
    set_names(ctx->cur_file);
    alloc_lfile(ctx, LSOF_FD_NUMERIC, 1);
    failed |= expect_freed("reusing an unselected file record");

    set_names(ctx->cur_file);
    lp.file = ctx->cur_file;
    ctx->cur_file = NULL;
    free_lproc(&lp);
    failed |= expect_freed("freeing a process's file records");

    alloc_lfile(ctx, LSOF_FD_NUMERIC, 2);
    set_names(ctx->cur_file);
    ctx->frozen = 1; /* Skip kernel initialization as well as enumeration. */
    if (lsof_gather(ctx, &result) != LSOF_SUCCESS) {
        fprintf(stderr, "lsof_gather failed\n");
        failed = 1;
    }
    failed |= expect_freed("discarding an orphaned file after gathering");

    lsof_free_result(result);
    ctx->cur_proc = NULL;
    lsof_destroy(ctx);
    return failed;
}
