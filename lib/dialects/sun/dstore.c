/*
 * dstore.c - Solaris global storage for lsof
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

/*
 * Global storage definitions
 */


struct clone *Clone = NULL; /* clone list */
major_t CloneMaj;           /* clone major device number */

char **Fsinfo = NULL;             /* file system information */
int Fsinfomax = 0;                /* maximum file system type */
int HaveCloneMaj = 0;             /* clone major device number has
                                   * been identified and is in
                                   * CloneMaj */
struct l_vfs *Lvfs = NULL;        /* local vfs structure table */
struct netclone *Netclone = NULL; /* net clone devices from
                                   * /devices/pseudo */

#if defined(HASFSTRUCT)
/*
 * Pff_tab[] - table for printing file flags
 */

struct pff_tab Pff_tab[] = {{(long)FREAD, FF_READ},
                            {(long)FWRITE, FF_WRITE},
                            {(long)FNDELAY, FF_NDELAY},
                            {(long)FAPPEND, FF_APPEND},
                            {(long)FSYNC, FF_SYNC},

#    if defined(FREVOKED)
                            {(long)FREVOKED, FF_REVOKED},
#    endif /* defined(FREVOKED) */

                            {(long)FDSYNC, FF_DSYNC},
                            {(long)FRSYNC, FF_RSYNC},

#    if defined(FOFFMAX)
                            {(long)FOFFMAX, FF_LARGEFILE},
#    endif /* defined(FFOFFMAX) */

                            {(long)FNONBLOCK, FF_NBLOCK},
                            {(long)FNOCTTY, FF_NOCTTY},
                            {(long)FASYNC, FF_ASYNC},
                            {(long)FNODSYNC, FF_NODSYNC},
                            {(long)0, NULL}};

/*
 * Pof_tab[] - table for print process open file flags
 */

struct pff_tab Pof_tab[] = {

#    if defined(UF_EXCLOSE)
    {(long)UF_EXCLOSE, POF_CLOEXEC},
#    endif /* defined(UF_EXCLOSE) */

#    if defined(FD_CLOEXEC)
    {(long)FD_CLOEXEC, POF_CLOEXEC},
#    endif /* defined(FD_CLOEXEC) */

#    if defined(UF_FDLOCK)
    {(long)UF_FDLOCK, POF_FDLOCK},
#    endif /* defined(UF_FDLOCK) */

    {(long)0, NULL}};
#endif /* defined(HASFSTRUCT) */

struct pseudo *Pseudo = NULL; /* non-clone devices from
                               * /devices/pseudo */
int Unof;                     /* u_nofiles value */
