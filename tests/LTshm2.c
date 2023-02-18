/*
 * LTshm2.c -- Lsof Test POSIX shm test using liblsof
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
#include <sys/mman.h>

int main(int argc, char **argv) {
    struct lsof_result *result = NULL;
    struct lsof_context *ctx = NULL;
    struct lsof_process *p;
    struct lsof_file *f;
    int ti, pi, fi;
    char buf[128];
    int my_pid, fd;
    int shm_found = 0;
    int shm_path_correct = 0;
    int res = 0;
    char shm_name[128];
    int shm;

    my_pid = getpid();
    (void)snprintf(shm_name, sizeof(shm_name), "/LTshm2%ld.shm", (long)my_pid);

    /*
     * Create a named shm
     */
    if ((shm = shm_open(shm_name, O_RDONLY | O_CREAT, 0644)) < 0) {
        (void)fprintf(stderr, "ERROR!!!  can't create shm at %s\n", shm_name);
        res = 1;
        goto cleanup;
    }

    ctx = lsof_new();
    lsof_select_pid(ctx, my_pid, 0);
    lsof_freeze(ctx);
    lsof_gather(ctx, &result);

    for (pi = 0; pi < result->num_processes; pi++) {
        p = &result->processes[pi];
        if (p->pid != my_pid)
            continue;
        for (fi = 0; fi < p->num_files; fi++) {
            f = &p->files[fi];
            if (f->fd_type == LSOF_FD_NUMERIC && f->fd_num == shm) {
                /* shm can be implemented as a regular file */
                if (f->file_type != LSOF_FILE_POSIX_SHM &&
                    f->file_type != LSOF_FILE_REGULAR &&
                    f->file_type != LSOF_FILE_VNODE_VREG) {
                    (void)fprintf(stderr,
                                  "ERROR!!!  found non-shm file type %d\n",
                                  f->file_type);
                    res = 1;
                    goto cleanup;
                }
                shm_found = 1;

                /* on Linux, f->name is /dev/shm/LTshm2... */
                if (!f->name || strstr(f->name, shm_name) != NULL) {
                    shm_path_correct = 1;
                }
            }
        }
    }

    if (!shm_found) {
        fprintf(stderr, "ERROR!!!  shm %s not found by lsof\n", shm_name);
        res = 1;
    }
    if (!shm_path_correct) {
        fprintf(stderr, "ERROR!!!  shm %s path incorrect\n", shm_name);
        res = 1;
    }

cleanup:
    if (result) {
        lsof_free_result(result);
    }
    if (ctx) {
        lsof_destroy(ctx);
    }
    if (shm) {
        shm_unlink(shm_name);
    }
    return res;
}
