/*
 * dnode.c - OpenBSD node functions for lsof
 */

/*
 * Copyright 1994 Purdue Research Foundation, West Lafayette, Indiana
 * 47907.  All rights reserved.
 *
 * Written by Victor A. Abell
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

#ifndef lint
static char copyright[] =
    "@(#) Copyright 1994 Purdue Research Foundation.\nAll rights reserved.\n";
#endif

#include "lsof.h"

/*
 * process_vnode() - process vnode
 */
void process_vnode(struct kinfo_file *file) {
    char *nm = NULL;
    int num = -1;
    uint32_t flag;
    char *type_name = NULL;
    int mib[3];
    size_t size;
    char path[PATH_MAX];

    /* Alloc Lf and set fd */
    switch (file->fd_fd) {
    case KERN_FILE_TEXT:
        nm = "txt";
        break;
    case KERN_FILE_CDIR:
        nm = "cwd";

        break;
    case KERN_FILE_RDIR:
        nm = "rtd";
        break;
    default:
        num = file->fd_fd;
        break;
    }
    alloc_lfile(nm, num);

    if (file->fd_fd == KERN_FILE_CDIR) {
        /*
         * Save current working directory information if available
         * sysctl(CTL_KERN, KERN_PROC_CWD, pid)
         */
        mib[0] = CTL_KERN;
        mib[1] = KERN_PROC_CWD;
        mib[2] = file->p_pid;
        size = sizeof(path);
        if (sysctl(mib, 3, path, &size, NULL, 0) >= 0) {
            (void)snpf(Namech, Namechl, "%s", path);
            enter_nm(Namech);
        }
    }

    /*
     * Construct access code.
     */
    if (file->fd_fd >= 0) {
        if ((flag = (file->f_flag & (FREAD | FWRITE))) == FREAD)
            Lf->access = 'r';
        else if (flag == FWRITE)
            Lf->access = 'w';
        else if (flag == (FREAD | FWRITE))
            Lf->access = 'u';
    }

    /* Fill file size/offset */
    if (file->v_type == VBLK || file->v_type == VCHR) {
        /* blk/char devices have no size, only offset */
        if (file->f_offset != (uint64_t)(-1)) {
            Lf->off = file->f_offset;
            Lf->off_def = 1;
        }
    } else {
        if (Foffset) {
            Lf->off = file->f_offset;
            Lf->off_def = 1;
        } else {
            Lf->sz = file->va_size;
            Lf->sz_def = 1;
        }
    }

    /* Fill inode */
    Lf->inode = file->va_fileid;
    Lf->inp_ty = 1;

    /* Fill dev && rdef */
    Lf->dev = file->va_fsid;
    Lf->dev_def = 1;
    if (file->v_type == VBLK || file->v_type == VCHR) {
        Lf->rdev = file->va_rdev;
        Lf->rdev_def = 1;
    }

    /* Fill type */
    switch (file->v_type) {
    case VREG:
        Lf->ntype = N_REGLR;
        type_name = "REG";
        break;
    case VDIR:
        type_name = "DIR";
        break;
    case VCHR:
        Lf->ntype = N_CHR;
        type_name = "CHR";
        break;
    case VFIFO:
        Lf->ntype = N_FIFO;
        type_name = "FIFO";
        break;
    }
    if (type_name)
        (void)snpf(Lf->type, sizeof(Lf->type), "%s", type_name);

    /* No way to read file path, request mount info  */
    Lf->lmi_srch = 1;

    /* Fill number of links */
    if (Fnlink) {
        Lf->nlink = file->va_nlink;
        Lf->nlink_def = 1;

        /* Handle link count filter */
        if (Nlink && (Lf->nlink < Nlink))
            Lf->sf |= SELNLINK;
    }

    /* Handle name match, must be done late, because if_file_named checks
     * Lf->dev etc. */
    if (is_file_named(nm, 0)) {
        Lf->sf |= SELNM;
    }

    /* Finish */
    if (Lf->sf)
        link_lfile();
}
