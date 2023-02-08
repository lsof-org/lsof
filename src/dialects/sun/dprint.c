/*
 * dprint.c - Solaris printing functions for lsof
 */

/*
 * Copyright 1997 Purdue Research Foundation, West Lafayette, Indiana
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

#include "common.h"
#include "cli.h"

/*
 * print_tcptpi() - print TCP/TPI info
 */

void print_tcptpi(struct lsof_context *ctx, int nl) /* 1 == '\n' required */
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
            if ((i = Lf->lts.state.i + TcpStOff) < 0 || i >= TcpNstates) {
                (void)snpf(sbuf, sizeof(sbuf), "UNKNOWN_TCP_STATE_%d",
                           Lf->lts.state.i);
                cp = sbuf;
            } else
                cp = TcpSt[i];
            break;
        case 1: /* TPI */
            if (!UdpSt)
                (void)build_IPstates(ctx);
            if ((u = Lf->lts.state.ui + UdpStOff) < 0 || u >= UdpNstates) {
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

#if defined(HASTCPTPIQ)
    if (Ftcptpi & TCPTPI_QUEUES) {
        if (Lf->lts.rqs) {
            if (Ffield)
                putchar(LSOF_FID_TCPTPI);
            else {
                if (ps)
                    putchar(' ');
                else
                    putchar('(');
            }
            (void)printf("QR=%lu", Lf->lts.rq);
            if (Ffield)
                putchar(Terminator);
            ps++;
        }
        if (Lf->lts.sqs) {
            if (Ffield)
                putchar(LSOF_FID_TCPTPI);
            else {
                if (ps)
                    putchar(' ');
                else
                    putchar('(');
            }
            (void)printf("QS=%lu", Lf->lts.sq);
            if (Ffield)
                putchar(Terminator);
            ps++;
        }
    }
#endif /* defined(HASTCPTPIQ) */

#if defined(HASSOOPT)
    if (Ftcptpi & TCPTPI_FLAGS) {
        int opt;

        if ((opt = Lf->lts.opt) || Lf->lts.pqlens || Lf->lts.qlens ||
            Lf->lts.qlims || Lf->lts.rbszs || Lf->lts.sbsz) {
            char sep = ' ';

            if (Ffield)
                sep = LSOF_FID_TCPTPI;
            else if (!ps)
                sep = '(';
            (void)printf("%cSO", sep);
            ps++;
            sep = '=';

#    if defined(SO_BROADCAST)
            if (opt & SO_BROADCAST) {
                (void)printf("%cBROADCAST", sep);
                opt &= ~SO_BROADCAST;
                sep = ',';
            }
#    endif /* defined(SO_BROADCAST) */

#    if defined(SO_DEBUG)
            if (opt & SO_DEBUG) {
                (void)printf("%cDEBUG", sep);
                opt &= ~SO_DEBUG;
                sep = ',';
            }
#    endif /* defined(SO_DEBUG) */

#    if defined(SO_DGRAM_ERRIND)
            if (opt & SO_DGRAM_ERRIND) {
                (void)printf("%cDGRAM_ERRIND", sep);
                opt &= ~SO_DGRAM_ERRIND;
                sep = ',';
            }
#    endif /* defined(SO_DGRAM_ERRIND) */

#    if defined(SO_DONTROUTE)
            if (opt & SO_DONTROUTE) {
                (void)printf("%cDONTROUTE", sep);
                opt &= ~SO_DONTROUTE;
                sep = ',';
            }
#    endif /* defined(SO_DONTROUTE) */

#    if defined(SO_KEEPALIVE)
            if (opt & SO_KEEPALIVE) {
                (void)printf("%cKEEPALIVE", sep);
                if (Lf->lts.kai)
                    (void)printf("=%d", Lf->lts.kai);
                opt &= ~SO_KEEPALIVE;
                sep = ',';
            }
#    endif /* defined(SO_KEEPALIVE) */

#    if defined(SO_LINGER)
            if (opt & SO_LINGER) {
                (void)printf("%cLINGER", sep);
                if (Lf->lts.ltm)
                    (void)printf("=%d", Lf->lts.ltm);
                opt &= ~SO_LINGER;
                sep = ',';
            }
#    endif /* defined(SO_LINGER) */

#    if defined(SO_OOBINLINE)
            if (opt & SO_OOBINLINE) {
                (void)printf("%cOOBINLINE", sep);
                opt &= ~SO_OOBINLINE;
                sep = ',';
            }
#    endif /* defined(SO_OOBINLINE) */

            if (Lf->lts.pqlens) {
                (void)printf("%cPQLEN=%u", sep, Lf->lts.pqlen);
                sep = ',';
            }
            if (Lf->lts.qlens) {
                (void)printf("%cQLEN=%u", sep, Lf->lts.qlen);
                sep = ',';
            }
            if (Lf->lts.qlims) {
                (void)printf("%cQLIM=%u", sep, Lf->lts.qlim);
                sep = ',';
            }
            if (Lf->lts.rbszs) {
                (void)printf("%cRCVBUF=%lu", sep, Lf->lts.rbsz);
                sep = ',';
            }

#    if defined(SO_REUSEADDR)
            if (opt & SO_REUSEADDR) {
                (void)printf("%cREUSEADDR", sep);
                opt &= ~SO_REUSEADDR;
                sep = ',';
            }
#    endif /* defined(SO_REUSEADDR) */

            if (Lf->lts.sbszs) {
                (void)printf("%cSNDBUF=%lu", sep, Lf->lts.sbsz);
                sep = ',';
            }

#    if defined(SO_TIMESTAMP)
            if (opt & SO_TIMESTAMP) {
                (void)printf("%cTIMESTAMP", sep);
                opt &= ~SO_TIMESTAMP;
                sep = ',';
            }
#    endif /* defined(SO_TIMESTAMP) */

#    if defined(SO_USELOOPBACK)
            if (opt & SO_USELOOPBACK) {
                (void)printf("%cUSELOOPBACK", sep);
                opt &= ~SO_USELOOPBACK;
                sep = ',';
            }
#    endif /* defined(SO_USELOOPBACK) */

            if (opt)
                (void)printf("%cUNKNOWN=%#x", sep, opt);
            if (Ffield)
                putchar(Terminator);
        }
    }
#endif /* defined(HASSOOPT) */

#if defined(HASTCPOPT)
    if (Ftcptpi & TCPTPI_FLAGS) {
        int topt;

        if ((topt = Lf->lts.topt) || Lf->lts.msss) {
            char sep = ' ';

            if (Ffield)
                sep = LSOF_FID_TCPTPI;
            else if (!ps)
                sep = '(';
            (void)printf("%cTF", sep);
            ps++;
            sep = '=';

#    if defined(TF_ACKNOW)
            if (topt & TF_ACKNOW) {
                (void)printf("%cACKNOW", sep);
                topt &= ~TF_ACKNOW;
                sep = ',';
            }
#    endif /* defined(TF_ACKNOW) */

#    if defined(TF_DELACK)
            if (topt & TF_DELACK) {
                (void)printf("%cDELACK", sep);
                topt &= ~TF_DELACK;
                sep = ',';
            }
#    endif /* defined(TF_DELACK) */

            if (Lf->lts.msss) {
                (void)printf("%cMSS=%lu", sep, Lf->lts.mss);
                sep = ',';
            }

#    if defined(TF_NODELAY)
            if (topt & TF_NODELAY) {
                (void)printf("%cNODELAY", sep);
                topt &= ~TF_NODELAY;
                sep = ',';
            }
#    endif /* defined(TF_NODELAY) */

#    if defined(TF_NOOPT)
            if (topt & TF_NOOPT) {
                (void)printf("%cNOOPT", sep);
                topt &= ~TF_NOOPT;
                sep = ',';
            }
#    endif /* defined(TF_NOOPT) */

#    if defined(TF_SENTFIN)
            if (topt & TF_SENTFIN) {
                (void)printf("%cSENTFIN", sep);
                topt &= ~TF_SENTFIN;
                sep = ',';
            }
#    endif /* defined(TF_SENTFIN) */

            if (topt)
                (void)printf("%cUNKNOWN=%#x", sep, topt);
            if (Ffield)
                putchar(Terminator);
        }
    }
#endif /* defined(HASTCPOPT) */

#if defined(HASTCPTPIW)
    if (Ftcptpi & TCPTPI_WINDOWS) {
        if (Lf->lts.rws) {
            if (Ffield)
                putchar(LSOF_FID_TCPTPI);
            else {
                if (ps)
                    putchar(' ');
                else
                    putchar('(');
            }
            (void)printf("WR=%lu", Lf->lts.rw);
            if (Ffield)
                putchar(Terminator);
            ps++;
        }
        if (Lf->lts.wws) {
            if (Ffield)
                putchar(LSOF_FID_TCPTPI);
            else {
                if (ps)
                    putchar(' ');
                else
                    putchar('(');
            }
            (void)printf("WW=%lu", Lf->lts.ww);
            if (Ffield)
                putchar(Terminator);
            ps++;
        }
    }
#endif /* defined(HASTCPTPIW) */

    if (Ftcptpi && !Ffield && ps)
        putchar(')');
    if (nl)
        putchar('\n');
}