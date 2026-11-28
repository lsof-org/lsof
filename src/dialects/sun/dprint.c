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

/*
 * dprint.c - Solaris CLI printing functions
 */

#include "common.h"

/*
 * print_tcptpi() - print TCP/TPI info
 */

void print_tcptpi(struct lsof_context *ctx, /* context */
                  int nl)                   /* 1 == '\n' required */
{
    char *cp = (char *)NULL;
    char sbuf[128];
    int i;
    int ps = 0;
    unsigned int u;

    if (Ftcptpi & TCPTPI_STATE) {
        switch (Lf->lts.type) {
        case 0: /* TCP */
            if (!TcpSt)
                (void)build_IPstates(ctx);
            if (Lf->lts.state.i < 0 ||
                (i = Lf->lts.state.i + TcpStOff) >= TcpNstates) {
                (void)snpf(sbuf, sizeof(sbuf), "UNKNOWN_TCP_STATE_%d",
                           Lf->lts.state.i);
                cp = sbuf;
            } else
                cp = TcpSt[i];
            break;
        case 1: /* TPI */
            if (!UdpSt)
                (void)build_IPstates(ctx);
            u = Lf->lts.state.ui + UdpStOff;
            if (Lf->lts.state.ui == (unsigned)-1 ||
                u >= (unsigned)UdpNstates) {
                (void)snpf(sbuf, sizeof(sbuf), "UNKNOWN_UDP_STATE_%u",
                           Lf->lts.state.ui);
                cp = sbuf;
            } else
                cp = UdpSt[u];
        }
        if (cp) {
            if (Ffield)
                (void)printf("%cST=%s%c", LSOF_FID_TCPTPI, cp, Terminator);
            else {
                putchar('(');
                (void)fputs(cp, stdout);
            }
            ps++;
        }
    }

    if (Ftcptpi && !Ffield && ps)
        putchar(')');
    if (nl)
        putchar('\n');
}

