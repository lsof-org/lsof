/*
 * LTszoff2.c -- Lsof Test small file (< 32 bits) size and offset tests
 *
 * V. Abell
 * Purdue University
 */

/*
 * Copyright 2002 Purdue Research Foundation, West Lafayette, Indiana
 * 47907.  All rights reserved.
 *
 * Written by V. Abell.
 *
 * This software is not subject to any license of the American Telephone
 * and Telegraph Company or the Regents of the University of California.
 *
 * Permission is granted to anyone to use this software for any purpose on
 * any computer system, and to alter it and redistribute it freely, subject
 * to the following restrictions:
 *
 * 1. Neither the authors nor Purdue University are responsible for any
 *    consequences of the use of this software.
 *
 * 2. The origin of this software must not be misrepresented, either by
 *    explicit claim or by omission.  Credit to the authors and Purdue
 *    University must appear in documentation and sources.
 *
 * 3. Altered versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 4. This notice may not be removed or altered.
 */

#include "lsof.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define TSTFSZ 32768 /* test file size */

int main(int argc, char **argv) {
    struct lsof_result *result = NULL;
    struct lsof_context *ctx = NULL;
    struct lsof_process *p;
    struct lsof_file *f;
    int ti, pi, fi;
    char path[128], buf[128];
    int my_pid, fd;
    int fd_found = 0;
    int sz_correct = 0;
    int off_correct = 0;
    int res = 0;

    my_pid = getpid();
    (void)snprintf(path, sizeof(path), "./config.LTszoff%ld", (long)my_pid);

    /*
     * Open a new test file at the specified path.
     */
    (void)unlink(path);
    if ((fd = open(path, O_RDWR | O_CREAT, 0600)) < 0) {
        (void)fprintf(stderr, "ERROR!!!  can't open %s\n", path);
        res = 1;
        goto cleanup;
    }

    /*
     * Write the test file to its expected size.
     */
    for (ti = 0; ti < sizeof(buf); ti++) {
        buf[ti] = (char)(ti & 0xff);
    }
    for (ti = 0; ti < TSTFSZ; ti += sizeof(buf)) {
        if (write(fd, buf, sizeof(buf)) != sizeof(buf)) {
            (void)fprintf(stderr, "ERROR!!!  can't write %d bytes to %s\n",
                          (int)sizeof(buf), path);
            res = 1;
            goto cleanup;
        }
    }
    /*
     * Fsync() the file.
     */
    if (fsync(fd)) {
        (void)fprintf(stderr, "ERROR!!!  can't fsync %s\n", path);
        res = 1;
        goto cleanup;
    }

    ctx = lsof_new();
    /* filter by PID and FD */
    lsof_select_pid(ctx, my_pid, 0);
    lsof_select_fd(ctx, LSOF_FD_NUMERIC, fd, fd, 0);
    lsof_logic_and(ctx);
    lsof_freeze(ctx);
    lsof_gather(ctx, &result);

    for (pi = 0; pi < result->num_processes; pi++) {
        p = &result->processes[pi];
        if (p->pid != my_pid) {
            (void)fprintf(stderr, "ERROR!!!  found not selected pid %d\n",
                          p->pid);
            res = 1;
            goto cleanup;
        }
        for (fi = 0; fi < p->num_files; fi++) {
            f = &p->files[fi];
            if (f->fd_type == LSOF_FD_NUMERIC && f->fd_num == fd) {
                /* check if fd, size and offset matches */
                fd_found = 1;
                if ((f->flags & LSOF_FILE_FLAG_SIZE_VALID) &&
                    f->size == TSTFSZ) {
                    sz_correct = 1;
                }
                if ((f->flags & LSOF_FILE_FLAG_OFFSET_VALID) &&
                    f->offset == TSTFSZ) {
                    off_correct = 1;
                }
            } else {
                (void)fprintf(stderr, "ERROR!!!  found not selected fd %d\n",
                              f->fd_num);
                res = 1;
                goto cleanup;
            }
        }
    }

    if (!fd_found) {
        fprintf(stderr, "ERROR!!!  test file %s not found by lsof\n", path);
    }
    if (!sz_correct) {
        fprintf(stderr, "ERROR!!!  test file %s size incorrect\n", path);
    }
    if (!off_correct) {
        fprintf(stderr, "ERROR!!!  test file %s offset incorrect\n", path);
    }

cleanup:
    if (result) {
        lsof_free_result(result);
    }
    if (ctx) {
        lsof_destroy(ctx);
    }
    if (fd >= 0) {
        (void)close(fd);
        (void)unlink(path);
    }
    return res;
}