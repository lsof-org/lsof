/*
 * dprint.c -- Darwin file processing functions for libproc-based lsof
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

#ifndef lint
static char copyright[] = "@(#) Copyright 2005-2007 Apple Inc. and Purdue "
                          "Research Foundation.\nAll rights reserved.\n";
#endif

#include "common.h"

#if defined(PROC_FP_GUARDED)
extern struct pff_tab Pgf_tab[];
#endif /* defined(PROC_FP_GUARDED) */

/*
 * print_nm() -- print Name column
 */
void print_nm(struct lsof_context *ctx, struct lfile *lf) {
    unsigned char extra = 0;

    printname(ctx, 0);

#if defined(PROC_PIDLISTFILEPORTS)
    if (lf->fileport)
        extra++;
#endif /* defined(PROC_PIDLISTFILEPORTS) */

#if defined(PROC_FP_GUARDED)
    if (lf->guardflags)
        extra++;
#endif /* defined(PROC_FP_GUARDED) */

    if (extra)
        (void)printf(" (");

#if defined(PROC_PIDLISTFILEPORTS)
    if (lf->fileport)
        (void)printf("fileport=0x%04x", lf->fileport);
#endif /* defined(PROC_PIDLISTFILEPORTS) */

#if defined(PROC_FP_GUARDED)
    if (extra > 1)
        putchar(',');
    if (lf->guardflags) {
        struct pff_tab *tp;
        long gf;

        (void)printf("guard=");
        tp = Pgf_tab;
        gf = lf->guardflags;
        while (gf && !FsvFlagX) {
            while (tp->nm) {
                if (gf & tp->val)
                    break;
                tp++;
            }
            if (!tp->nm)
                break;
            gf &= ~(tp->val);
            (void)printf("%s%s", tp->nm, gf ? "," : "");
        }
        /*
         * If flag bits remain, print them in hex.  If hex output was
         * specified with +fG, print all flag values, including zero,
         * in hex.
         */
        if (gf || FsvFlagX)
            (void)printf("0x%lx", gf);
    }
#endif /* defined(PROC_FP_GUARDED) */

    if (extra)
        (void)printf(")\n");
    else
        putchar('\n');
}
