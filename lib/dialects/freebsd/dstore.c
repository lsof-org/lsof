/*
 * dstore.c - FreeBSD global storage for lsof
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

#include "common.h"

struct file *Cfp; /* curent file's file struct pointer */

/*
 * Drive_Nl -- table to drive the building of Nl[] via build_Nl()
 *             (See lsof.h and misc.c.)
 */

struct drive_Nl Drive_Nl[] = {

    {X_BADFILEOPS, "badfileops"},

    {X_NCACHE, "_nchashtbl"},

    {X_NCSIZE, "_nchash"},

    {"", ""},
    {NULL, NULL}};

kvm_t *Kd = NULL; /* kvm descriptor */

#if defined(P_ADDR)
KA_T Kpa; /* kernel proc struct address */
#endif    /* defined(P_ADDR) */

struct l_vfs *Lvfs = NULL; /* local vfs structure table */

int Np = 0; /* number of kernel processes */

struct kinfo_proc *P = NULL; /* local process table copy */

#if defined(HASFSTRUCT)
/*
 * Pff_tab[] - table for printing file flags
 */

struct pff_tab Pff_tab[] = {{(long)FREAD, FF_READ},
                            {(long)FWRITE, FF_WRITE},
                            {(long)FNONBLOCK, FF_NBLOCK},
                            {(long)FNDELAY, FF_NDELAY},
                            {(long)FAPPEND, FF_APPEND},
                            {(long)FASYNC, FF_ASYNC},
                            {(long)FFSYNC, FF_FSYNC},

#    if defined(FMARK)
                            {(long)FMARK, FF_MARK},
#    endif /* defined(FMARK) */

#    if defined(FDEFER)
                            {(long)FDEFER, FF_DEFER},
#    endif /* defined(FDEFER) */

                            {(long)FHASLOCK, FF_HASLOCK},
                            {(long)O_NOCTTY, FF_NOCTTY},
                            {(long)0, NULL}};

/*
 * Pof_tab[] - table for print process open file flags
 */

struct pff_tab Pof_tab[] = {

#    if defined(UF_EXCLOSE)
    {(long)UF_EXCLOSE, POF_CLOEXEC},
#    endif /* defined(UF_EXCLOSE) */

#    if defined(UF_MAPPED)
    {(long)UF_MAPPED, POF_MAPPED},
#    endif /* defined(UF_MAPPED) */

    {(long)0, NULL}};
#endif /* defined(HASFSTRUCT) */

/*
 * Kernel's bad file operations address
 */

KA_T X_bfopsa; /* badfileops kernel address */

/* FreeBSD 12 has a bug that leaves undefined symbol __start_set_pcpu and
 * __stop_set_pcpu but not actually using it. Workaround the issue like
 * https://github.com/freebsd/freebsd-src/commit/56e8cb87c6054a302503c33b39a4bd658f505c4a
 */
uintptr_t *__start_set_pcpu;
uintptr_t *__stop_set_pcpu;

