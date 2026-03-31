/*
 * dlsof.h -- Darwin header file for libproc-based lsof
 */

/*
 * Portions Copyright 2005-2007 Apple Inc.  All rights reserved.
 *
 * Copyright 2005 Purdue Research Foundation, West Lafayette, Indiana
 * 47907.  All rights reserved.
 *
 * Written by Allan Nathanson, Apple Inc., and Victor A. Abell, Purdue
 * University.
 *
 * This software is not subject to any license of the American Telephone
 * and Telegraph Company or the Regents of the University of California.
 *
 * Permission is granted to anyone to use this software for any purpose on
 * any computer system, and to alter it and redistribute it freely, subject
 * to the following restrictions:
 *
 * 1. Neither the authors, nor Apple Inc. nor Purdue University are
 *    responsible for any consequences of the use of this software.
 *
 * 2. The origin of this software must not be misrepresented, either
 *    by explicit claim or by omission.  Credit to the authors, Apple
 *    Inc. and Purdue University must appear in documentation and sources.
 *
 * 3. Altered versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 4. This notice may not be removed or altered.
 */

/*
 * $Id: dlsof.h,v 1.8 2012/04/10 16:41:04 abe Exp abe $
 */

#if !defined(DARWIN_LSOF_H)
#    define DARWIN_LSOF_H 1

#    include <stdlib.h>
#    include <dirent.h>
#    include <setjmp.h>
#    include <signal.h>
#    include <string.h>
#    include <unistd.h>
#    include <wctype.h>
#    include <arpa/inet.h>
#    include <sys/attr.h>
#    include <sys/fcntl.h>
#    include <sys/socket.h>
#    include <netinet/in.h>
#    include <netinet/tcp_fsm.h>
#    include <netinet/tcp_timer.h>
#    include <rpc/rpc.h>
#    include <rpc/pmap_prot.h>
#    include <libproc.h>

#    if DARWINV < 900
#        define vst_blksize st_blksize
#        define vst_dev st_dev
#        define vst_ino st_ino
#        define vst_mode st_mode
#        define vst_nlink st_nlink
#        define vst_rdev st_rdev
#        define vst_size st_size
#    endif /* DARWINV<=900 */

#    define COMP_P const void
#    define DEVINCR 1024   /* device table malloc() increment */
#    define DIRTYPE dirent /* directory entry type */
typedef uintptr_t KA_T;
#    define KA_T_FMT_X "0x%08lx"
#    define LOGINML MAXLOGNAME
#    define MALLOC_P void
#    define FREE_P MALLOC_P
#    define MALLOC_S size_t
#    define MAXSYSCMDL (MAXCOMLEN - 1) /* max system command name length */
#    define MOUNTED MNT_MNTTAB
#    define QSORT_P void
#    define READLEN_T int
#    define STRNCPY_L size_t
#    define SZOFFTYPE                                                          \
        unsigned long long /* size and offset internal storage type */
#    define SZOFFPSPEC                                                         \
        "ll" /* SZOFFTYPE printf specification                                 \
              * modifier */

/*
 * Global storage definitions (including their structure definitions)
 */

extern struct file *Cfp;

struct mounts {
    char *dir;           /* directory (mounted on) */
    char *fsname;        /* file system
                          * (symbolic links unresolved) */
    char *fsnmres;       /* file system
                          * (symbolic links resolved) */
    dev_t dev;           /* directory st_dev */
    dev_t rdev;          /* directory st_rdev */
    INODETYPE inode;     /* directory st_ino */
    mode_t mode;         /* directory st_mode */
    mode_t fs_mode;      /* file system st_mode */
    int is_nfs;          /* 1 if NFS file system, 0 if not */
    struct mounts *next; /* forward link */
};

struct sfile {
    char *aname;        /* argument file name */
    char *name;         /* file name (after readlink()) */
    char *devnm;        /* device name (optional) */
    dev_t dev;          /* device */
    dev_t rdev;         /* raw device */
    u_short mode;       /* S_IFMT mode bits from stat() */
    int type;           /* file type: 0 = file system
                         *	      1 = regular file */
    INODETYPE i;        /* inode number */
    int f;              /* file found flag */
    struct sfile *next; /* forward link */
};

#    define XDR_VOID (const xdrproc_t) xdr_void
#    define XDR_PMAPLIST (const xdrproc_t) xdr_pmaplist

#    if !defined(offsetof)
#        define offsetof(type, member) ((size_t)(&((type *)0)->member))
#    endif /* !defined(offsetof) */

struct lsof_context_dialect {
    /* Process information buffers */
    struct proc_fdinfo *fds; /* FD buffer */
    int *pids;               /* PID buffer */
    int nb_pids;             /* bytes allocated to Pids */
    int nb_fds;              /* bytes allocated to FDs */

#    if DARWINV >= 900
    int nb_threads;    /* Threads bytes allocated */
    uint64_t *threads; /* Thread buffer */
#    endif             /* DARWINV>=900 */

#    if defined(PROC_PIDLISTFILEPORTS)
    struct proc_fileportinfo *fps; /* fileport buffer */
    int nb_fps;                    /* bytes allocated to fileports */
#    endif                         /* PROC_PIDLISTFILEPORTS */

    /* Vnode info recording */
    struct darwin_vips_info {
        dev_t dev;
        unsigned long ino;
    } *vips;    /* recorded vnodes */
    int n_vips; /* entries allocated to Vips (capacity) */
};

/* Macros to access dialect-specific context fields */
#    define Fds (ctx->dialect.fds)
#    define Pids (ctx->dialect.pids)
#    define NbPids (ctx->dialect.nb_pids)
#    define NbFds (ctx->dialect.nb_fds)

#    if DARWINV >= 900
#        define Threads (ctx->dialect.threads)
#        define NbThreads (ctx->dialect.nb_threads)
#    endif /* DARWINV>=900 */

#    if defined(PROC_PIDLISTFILEPORTS)
#        define Fps (ctx->dialect.fps)
#        define NbFps (ctx->dialect.nb_fps)
#    endif /* PROC_PIDLISTFILEPORTS */

#    define Vips (ctx->dialect.vips)
#    define NVips (ctx->dialect.n_vips)

#endif /* DARWIN_LSOF_H */
