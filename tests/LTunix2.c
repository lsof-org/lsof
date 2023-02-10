/*
 * LTunix2.c -- Lsof Test UNIX domain socket test using liblsof
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
#include <sys/un.h>
#include <string.h>

int main(int argc, char **argv) {
    struct lsof_result *result = NULL;
    struct lsof_context *ctx = NULL;
    struct lsof_process *p;
    struct lsof_file *f;
    int ti, pi, fi;
    char path[128], buf[128];
    int my_pid, fd;
    int fd_found = 0;
    int path_correct = 0;
    int res = 0;
    struct sockaddr_un ua; /* UNIX socket address */

    my_pid = getpid();
    (void)snprintf(path, sizeof(path), "/tmp/config.LTunix%ld", (long)my_pid);

    /*
     * Create a unix socket at the specified path
     */
    (void)unlink(path);
    if ((fd = socket(AF_UNIX, SOCK_STREAM, PF_UNSPEC)) < 0) {
        (void)fprintf(stderr, "ERROR!!!  can't create unix socket at %s\n",
                      path);
        res = 1;
        goto cleanup;
    }
    (void)memset((void *)&ua, 0, sizeof(ua));
    ua.sun_family = AF_UNIX;
    snprintf(ua.sun_path, sizeof(ua.sun_path), "%s", path);

    if (bind(fd, (struct sockaddr *)&ua, sizeof(ua)) != 0) {
        (void)fprintf(stderr, "ERROR!!!  can't create unix socket at %s\n",
                      path);
        res = 1;
        goto cleanup;
    }

    ctx = lsof_new();
    lsof_select_unix_socket(ctx);
    lsof_freeze(ctx);
    lsof_gather(ctx, &result);

    for (pi = 0; pi < result->num_processes; pi++) {
        p = &result->processes[pi];
        if (p->pid != my_pid)
            continue;
        for (fi = 0; fi < p->num_files; fi++) {
            f = &p->files[fi];
            if (f->file_type != LSOF_FILE_UNIX) {
                (void)fprintf(stderr, "ERROR!!!  found non-unix file type %d\n",
                              f->file_type);
                res = 1;
                goto cleanup;
            }
            if (f->fd_type == LSOF_FD_NUMERIC && f->fd_num == fd) {
                /* check if name matches, only consider prefix because it can be
                 * `/some/path type=STREAM` */
                fd_found = 1;
                if (strncmp(f->name, path, strlen(path)) == 0) {
                    path_correct = 1;
                }
            }
        }
    }

    if (!fd_found) {
        fprintf(stderr, "ERROR!!!  test file %s not found by lsof\n", path);
    }
    if (!path_correct) {
        fprintf(stderr, "ERROR!!!  test file %s path incorrect\n", path);
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
