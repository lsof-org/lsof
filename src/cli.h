/*
 * cli.h - header file for lsof cli
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

#if !defined(CLI_H)
#    define CLI_H

#    include "lsof.h"
#    include "proto.h"

/* Global ctx in cli */
extern struct lsof_context *ctx;

struct fieldsel {
    char id;          /* field ID character */
    unsigned char st; /* field status */
    char *nm;         /* field name */
    int *opt;         /* option variable address */
    int ov;           /* value to OR with option variable */
};
extern struct fieldsel FieldSel[];

#    define LSOF_SEARCH_FAILURE                                                \
        (FsearchErr ? LSOF_EXIT_ERROR : LSOF_EXIT_SUCCESS)

extern int CmdLim;

extern int Fcntx;
extern int Ffield;
extern int Ffilesys;
extern int Fhelp;
extern int Fhuman;
extern int Fhost;
extern int Fnlink;
extern int Foffset;
extern int Fpgid;
extern int Fppid;
extern int Fport;
extern int FsearchErr;
extern int Fsv;
extern int FsvByf;
extern int FsvFlagX;
extern int Fsize;
extern int Fterse;
extern int Ftcptpi;
extern int Futol;
extern int Fverbose;
extern int Fxover;
extern int Fzone;

extern int CntxStatus;

extern char Terminator;

extern int RptTm;
extern int RptMaxCount;

extern int OffDecDig;

#    if !defined(HASNORPC_H)
extern int FportMap;
#    endif /* !defined(HASNORPC_H) */

extern void print_tcptpi(struct lsof_context *ctx, int nl);

#endif