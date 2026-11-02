/* LTfilemeta.c -- gather selected and filtered file metadata under a leak checker.
 */

#ifdef AUTOTOOLS
#    include "config.h"
#endif
#include "common.h"
#include "lsof.h"
#include <unistd.h>

int main(int argc, char *argv[]) {
    struct lsof_context *ctx = NULL;
    struct lsof_result *result = NULL;
    struct lsof_file *file;
    const char *tmp = getenv("TMPDIR");
    char path[MAXPATHLEN], resolved[MAXPATHLEN];
    int fd = -1, filtered_fd = -1, round, found;
    size_t pi, fi;
    int status = 1;

    (void)argc;
    (void)argv;

    if (snprintf(path, sizeof(path), "%s/lsof-filemeta-XXXXXX",
                 tmp ? tmp : "/tmp") >= sizeof(path))
        return 1;
    fd = mkstemp(path);
    if (fd < 0)
        return 1;
    filtered_fd = dup(fd);
    if (filtered_fd < 0 || !realpath(path, resolved))
        goto cleanup;

    ctx = lsof_new();
    if (!ctx || lsof_select_pid(ctx, getpid(), 0) != LSOF_SUCCESS ||
        lsof_logic_and(ctx) != LSOF_SUCCESS ||
        lsof_avoid_forking(ctx, 1) != LSOF_SUCCESS)
        goto cleanup;

    /* No public FD selector exists yet.  Select only fd within this process;
     * other records are gathered but their names are not transferred.
     */
    ctx->fd_list = calloc(1, sizeof(*ctx->fd_list));
    if (!ctx->fd_list)
        goto cleanup;
    ctx->fd_list->fd_type = LSOF_FD_NUMERIC;
    ctx->fd_list->lo = ctx->fd_list->hi = fd;
    ctx->fd_list_ty = 0;
    ctx->sel_flags |= SELFD;

    for (round = 0; round < 3; round++) {
        if (lsof_gather(ctx, &result) != LSOF_SUCCESS)
            goto cleanup;
        /* Result names must remain owned by the result, not the context. */
        if (round == 2) {
            lsof_destroy(ctx);
            ctx = NULL;
        }
        found = 0;
        for (pi = 0; pi < result->num_processes; pi++) {
            for (fi = 0; fi < result->processes[pi].num_files; fi++) {
                file = &result->processes[pi].files[fi];
                if (file->fd_type == LSOF_FD_NUMERIC &&
                    file->fd_num == filtered_fd) {
                    fprintf(stderr, "filtered FD returned by gather\n");
                    goto cleanup;
                }
                /* NAME may include filesystem details after the pathname. */
                if (file->fd_type == LSOF_FD_NUMERIC && file->fd_num == fd &&
                    file->name && strstr(file->name, resolved))
                    found = 1;
            }
        }
        if (!found) {
            fprintf(stderr, "selected file name missing or invalid\n");
            goto cleanup;
        }
        lsof_free_result(result);
        result = NULL;
    }
    status = 0;
cleanup:
    lsof_free_result(result);
    lsof_destroy(ctx);
    if (filtered_fd >= 0)
        close(filtered_fd);
    if (fd >= 0)
        close(fd);
    unlink(path);
    return status;
}
