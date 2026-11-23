/*
 * dsock.c -- Darwin socket processing functions for libproc-based lsof
 */

/*
 * Portions Copyright 2005 Apple Computer, Inc.  All rights reserved.
 *
 * Copyright 2005 Purdue Research Foundation, West Lafayette, Indiana
 * 47907.  All rights reserved.
 *
 * Written by Allan Nathanson, Apple Computer, Inc., and Victor A.
 * Abell, Purdue University.
 *
 * This software is not subject to any license of the American Telephone
 * and Telegraph Company or the Regents of the University of California.
 *
 * Permission is granted to anyone to use this software for any purpose on
 * any computer system, and to alter it and redistribute it freely, subject
 * to the following restrictions:
 *
 * 1. Neither the authors, nor Apple Computer, Inc. nor Purdue University
 *    are responsible for any consequences of the use of this software.
 *
 * 2. The origin of this software must not be misrepresented, either
 *    by explicit claim or by omission.  Credit to the authors, Apple
 *    Computer, Inc. and Purdue University must appear in documentation
 *    and sources.
 *
 * 3. Altered versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 4. This notice may not be removed or altered.
 */

#ifndef lint
static char copyright[] = "@(#) Copyright 2005 Apple Computer, Inc. and Purdue "
                          "Research Foundation.\nAll rights reserved.\n";
#endif

#include "common.h"

#if defined(HASIPv6)
/*
 * normalize_v4compat() - convert IPv4-compatible `::a.b.c.d' addresses to
 * the standard IPv4-mapped `::ffff:a.b.c.d' form.  Darwin reports accepted
 * IPv4 connections in the former form, while lsof's common selection and
 * display logic uses the latter.
 */
static void normalize_v4compat(unsigned char *a) {
    unsigned char v4[4];
    int i;

    for (i = 0; i < 12; i++) {
        if (a[i])
            return;
    }
    /* Exclude :: and ::1; both have IPv6 meanings. */
    if (!(a[12] || a[13] || a[14] || a[15] > 1))
        return;
    (void)memcpy(v4, a + 12, sizeof(v4));
    (void)memset(a, 0, 10);
    a[10] = a[11] = 0xff;
    (void)memcpy(a + 12, v4, sizeof(v4));
}
#endif /* defined(HASIPv6) */

/*
 * process_socket() -- process socket file
 */

static void process_socket_common(struct lsof_context *ctx,
                                  struct socket_fdinfo *si) {
    unsigned char *fa = (unsigned char *)NULL;
    int fam, fp, lp, unl;
    unsigned char *la = (unsigned char *)NULL;
#if defined(HASIPv6)
    struct in6_addr la6, fa6;
#endif /* defined(HASIPv6) */

    /*
     * Enter basic socket values.
     */
    Lf->type = LSOF_FILE_SOCKET;
    Lf->inp_ty = 2;
    /*
     * Enter basic file information.
     */
    enter_file_info(ctx, &si->pfi);
    /*
     * Enable size or offset display.
     */
    if (Lf->access == LSOF_FILE_ACCESS_READ)
        Lf->sz = (SZOFFTYPE)si->psi.soi_rcv.sbi_cc;
    else if (Lf->access == LSOF_FILE_ACCESS_WRITE)
        Lf->sz = (SZOFFTYPE)si->psi.soi_snd.sbi_cc;
    else
        Lf->sz = (SZOFFTYPE)(si->psi.soi_rcv.sbi_cc + si->psi.soi_snd.sbi_cc);
    Lf->sz_def = 1;

#if defined(HASTCPTPIQ)
    /*
     * Enter send and receive queue sizes.
     */
    Lf->lts.rq = si->psi.soi_rcv.sbi_cc;
    Lf->lts.sq = si->psi.soi_snd.sbi_cc;
    Lf->lts.rqs = Lf->lts.sqs = (unsigned char)1;
#endif /* defined(HASTCPTPIQ) */

#if defined(HASSOOPT)
    /*
     * Enter socket options.
     */
    Lf->lts.ltm = (unsigned int)(si->psi.soi_linger & 0xffff);
    Lf->lts.opt = (unsigned int)(si->psi.soi_options & 0xffff);
    Lf->lts.pqlen = (unsigned int)si->psi.soi_incqlen;
    Lf->lts.qlen = (unsigned int)si->psi.soi_qlen;
    Lf->lts.qlim = (unsigned int)si->psi.soi_qlimit;
    Lf->lts.rbsz = (unsigned long)si->psi.soi_rcv.sbi_mbmax;
    Lf->lts.sbsz = (unsigned long)si->psi.soi_snd.sbi_mbmax;
    Lf->lts.pqlens = Lf->lts.qlens = Lf->lts.qlims = Lf->lts.rbszs =
        Lf->lts.sbszs = (unsigned char)1;
#endif /* defined(HASSOOPT) */

#if defined(HASSOSTATE)
    /*
     * Enter socket state.
     */
    Lf->lts.ss = (unsigned int)si->psi.soi_state;
#endif /* defined(HASSOSTATE) */

    /*
     * Process socket by its associated domain family.
     */
    switch ((fam = si->psi.soi_family)) {
    case AF_INET:
    case AF_INET6:

        /*
         * Process IPv[46] sockets.
         */
        Lf->type = (fam == AF_INET) ? LSOF_FILE_IPV4 : LSOF_FILE_IPV6;
        if ((si->psi.soi_kind != SOCKINFO_IN) &&
            (si->psi.soi_kind != SOCKINFO_TCP)) {
            break;
        }
        /*
         * Process TCP state inclusions and exclusions, as required.
         */
        if ((si->psi.soi_kind == SOCKINFO_TCP) && (TcpStXn || TcpStIn)) {
            int tsnx = (int)si->psi.soi_proto.pri_tcp.tcpsi_state + TcpStOff;

            if ((tsnx >= 0) && (tsnx < TcpNstates)) {
                if (TcpStXn) {
                    if (TcpStX[tsnx]) {
                        Lf->sf |= SELEXCLF;
                        return;
                    }
                }
                if (TcpStIn) {
                    if (TcpStI[tsnx])
                        TcpStI[tsnx] = 2;
                    else {
                        Lf->sf |= SELEXCLF;
                        return;
                    }
                }
            }
        }
        /*
         * Process an Internet domain socket.
         */
        if (Fnet) {
            if (!FnetTy || ((FnetTy == 4) && (fam == AF_INET)) ||
                ((FnetTy == 6) && (fam == AF_INET6)))
                Lf->sf |= SELNET;
        }
        printiproto(ctx, si->psi.soi_protocol);
        if ((si->psi.soi_kind == SOCKINFO_TCP) &&
            si->psi.soi_proto.pri_tcp.tcpsi_tp) {
            enter_dev_ch(ctx,
                         print_kptr((KA_T)si->psi.soi_proto.pri_tcp.tcpsi_tp,
                                    (char *)NULL, 0));
        } else
            enter_dev_ch(ctx,
                         print_kptr((KA_T)si->psi.soi_pcb, (char *)NULL, 0));
        if (fam == AF_INET) {

            /*
             * Enter IPv4 address information.
             */
            if (si->psi.soi_kind == SOCKINFO_TCP) {

                /*
                 * Enter information for a TCP socket.
                 */
                la = (unsigned char *)&si->psi.soi_proto.pri_tcp.tcpsi_ini
                         .insi_laddr.ina_46.i46a_addr4;
                lp = (int)ntohs(si->psi.soi_proto.pri_tcp.tcpsi_ini.insi_lport);
                fa = (unsigned char *)&si->psi.soi_proto.pri_tcp.tcpsi_ini
                         .insi_faddr.ina_46.i46a_addr4;
                fp = (int)ntohs(si->psi.soi_proto.pri_tcp.tcpsi_ini.insi_fport);
            } else {

                /*
                 * Enter information for a non-TCP socket.
                 */
                la = (unsigned char *)&si->psi.soi_proto.pri_in.insi_laddr
                         .ina_46.i46a_addr4;
                lp = (int)ntohs(si->psi.soi_proto.pri_in.insi_lport);
                fa = (unsigned char *)&si->psi.soi_proto.pri_in.insi_faddr
                         .ina_46.i46a_addr4;
                fp = (int)ntohs(si->psi.soi_proto.pri_in.insi_fport);
            }
            if ((fa && (*fa == INADDR_ANY)) && !fp) {
                fa = (unsigned char *)NULL;
                fp = 0;
            }
        } else {

            /*
             * Enter IPv6 address information.  Leave IPv4-mapped addresses
             * in their IPv6 form: Lsof.8 specifies that such a file is IPv6
             * and is selected by '6', not '4'.
             */
            if (si->psi.soi_kind == SOCKINFO_TCP) {

                /*
                 * Enter TCP socket information.
                 */
                la = (unsigned char *)&si->psi.soi_proto.pri_tcp.tcpsi_ini
                         .insi_laddr.ina_6;
                lp = (int)ntohs(si->psi.soi_proto.pri_tcp.tcpsi_ini.insi_lport);
                fa = (unsigned char *)&si->psi.soi_proto.pri_tcp.tcpsi_ini
                         .insi_faddr.ina_6;
                fp = (int)ntohs(si->psi.soi_proto.pri_tcp.tcpsi_ini.insi_fport);
            } else {

                /*
                 * Enter non-TCP socket information.
                 */
                la =
                    (unsigned char *)&si->psi.soi_proto.pri_in.insi_laddr.ina_6;
                lp = (int)ntohs(si->psi.soi_proto.pri_in.insi_lport);
                fa =
                    (unsigned char *)&si->psi.soi_proto.pri_in.insi_faddr.ina_6;
                fp = (int)ntohs(si->psi.soi_proto.pri_in.insi_fport);
            }
            if (IN6_IS_ADDR_UNSPECIFIED((struct in6_addr *)fa) && !fp) {
                fa = (unsigned char *)NULL;
                fp = 0;
            }
#if defined(HASIPv6)
            /*
             * Normalize Darwin's IPv4-compatible form to the standard
             * IPv4-mapped form before selection and display.
             */
            if (la) {
                la6 = *(struct in6_addr *)la;
                normalize_v4compat((unsigned char *)&la6);
                la = (unsigned char *)&la6;
            }
            if (fa) {
                fa6 = *(struct in6_addr *)fa;
                normalize_v4compat((unsigned char *)&fa6);
                fa = (unsigned char *)&fa6;
            }
#endif /* defined(HASIPv6) */
        }
        /*
         * Enter local and remote addresses by address family.
         */
        if (fa || la)
            (void)ent_inaddr(ctx, la, lp, fa, fp, fam);
        if (si->psi.soi_kind == SOCKINFO_TCP) {

            /*
             * Enter a TCP socket definition and its state.
             */
            Lf->lts.type = 0;
            Lf->lts.state.i = (int)si->psi.soi_proto.pri_tcp.tcpsi_state;
            /*
             * Enter TCP options.
             */

#if defined(HASSOOPT)
            Lf->lts.kai =
                (unsigned int)si->psi.soi_proto.pri_tcp.tcpsi_timer[TCPT_KEEP];
#endif /* defined(HASSOOPT) */

#if defined(HASTCPOPT)
            Lf->lts.mss = (unsigned long)si->psi.soi_proto.pri_tcp.tcpsi_mss;
            Lf->lts.msss = (unsigned char)1;
            Lf->lts.topt = (unsigned int)si->psi.soi_proto.pri_tcp.tcpsi_flags;
#endif /* defined(HASTCPOPT) */
        }
        break;
    case AF_UNIX:

        /*
         * Process a UNIX domain socket.
         */
        Lf->type = LSOF_FILE_UNIX;
        if (si->psi.soi_kind != SOCKINFO_UN)
            break;
        if (Funix)
            Lf->sf |= SELUNX;
        enter_dev_ch(ctx, print_kptr((KA_T)si->psi.soi_pcb, (char *)NULL, 0));
        /*
         * Enter information on a UNIX domain socket that has no address bound
         * to it, although it may be connected to another UNIX domain socket
         * as a pipe.
         */
        if (si->psi.soi_proto.pri_un.unsi_addr.ua_sun.sun_family != AF_UNIX) {
            if (si->psi.soi_proto.pri_un.unsi_addr.ua_sun.sun_family ==
                AF_UNSPEC) {
                if (si->psi.soi_proto.pri_un.unsi_conn_pcb) {
                    (void)snpf(
                        Namech, Namechl, "->%s",
                        print_kptr((KA_T)si->psi.soi_proto.pri_un.unsi_conn_pcb,
                                   (char *)NULL, 0));
                } else
                    (void)snpf(Namech, Namechl, "->(none)");
            } else
                (void)snpf(
                    Namech, Namechl, "unknown sun_family (%d)",
                    si->psi.soi_proto.pri_un.unsi_addr.ua_sun.sun_family);
            break;
        }
        if (si->psi.soi_proto.pri_un.unsi_addr.ua_sun.sun_path[0]) {
            unl = si->psi.soi_proto.pri_un.unsi_addr.ua_sun.sun_len -
                  offsetof(struct sockaddr_un, sun_path);
            if ((unl < 0) ||
                (unl >=
                 sizeof(si->psi.soi_proto.pri_un.unsi_addr.ua_sun.sun_path)))
                unl =
                    sizeof(si->psi.soi_proto.pri_un.unsi_addr.ua_sun.sun_path) -
                    1;
            si->psi.soi_proto.pri_un.unsi_addr.ua_sun.sun_path[unl] = '\0';
            if (si->psi.soi_proto.pri_un.unsi_addr.ua_sun.sun_path[0] &&
                Sfile &&
                is_file_named(
                    ctx, si->psi.soi_proto.pri_un.unsi_addr.ua_sun.sun_path, 0))
                Lf->sf |= SELNM;
            if (si->psi.soi_proto.pri_un.unsi_addr.ua_sun.sun_path[0] &&
                !Namech[0])
                (void)snpf(Namech, Namechl, "%s",
                           si->psi.soi_proto.pri_un.unsi_addr.ua_sun.sun_path);
        } else
            (void)snpf(Namech, Namechl, "no address");
        break;
    case AF_ROUTE:

        /*
         * Process a ROUTE domain socket.
         */
        Lf->type = LSOF_FILE_ROUTE;
        break;
    case AF_NDRV:

        /*
         * Process an NDRV domain socket.
         */
        Lf->type = LSOF_FILE_NET_DRIVER;
        if (si->psi.soi_kind != SOCKINFO_NDRV)
            break;
        enter_dev_ch(ctx, print_kptr((KA_T)si->psi.soi_pcb, (char *)NULL, 0));
        si->psi.soi_proto.pri_ndrv
            .ndrvsi_if_name[sizeof(si->psi.soi_proto.pri_ndrv.ndrvsi_if_name) -
                            1] = '\0';
        (void)snpf(Namech, Namechl, "-> %s%d",
                   si->psi.soi_proto.pri_ndrv.ndrvsi_if_name,
                   si->psi.soi_proto.pri_ndrv.ndrvsi_if_unit);
        break;
    case pseudo_AF_KEY:

        /*
         * Process an [internal] key-management function socket.
         */
        Lf->type = LSOF_FILE_INTERNAL_KEY;
        enter_dev_ch(ctx, print_kptr((KA_T)si->psi.soi_pcb, (char *)NULL, 0));
        break;
    case AF_SYSTEM:

        /*
         * Process a SYSTEM domain socket.
         */
        Lf->type = LSOF_FILE_SYSTEM;
        switch (si->psi.soi_kind) {
        case SOCKINFO_KERN_EVENT:
            enter_dev_ch(ctx,
                         print_kptr((KA_T)si->psi.soi_pcb, (char *)NULL, 0));
            (void)snpf(Namech, Namechl, "[event %x:%x:%x]",
                       si->psi.soi_proto.pri_kern_event.kesi_vendor_code_filter,
                       si->psi.soi_proto.pri_kern_event.kesi_class_filter,
                       si->psi.soi_proto.pri_kern_event.kesi_subclass_filter);
            break;
        case SOCKINFO_KERN_CTL:
            enter_dev_ch(ctx,
                         print_kptr((KA_T)si->psi.soi_pcb, (char *)NULL, 0));
            (void)snpf(Namech, Namechl, "[ctl %s id %d unit %d]",
                       si->psi.soi_proto.pri_kern_ctl.kcsi_name,
                       si->psi.soi_proto.pri_kern_ctl.kcsi_id,
                       si->psi.soi_proto.pri_kern_ctl.kcsi_unit);
            break;
        }
        break;
    case AF_PPP:

        /*
         * Process a PPP domain socket.
         */
        Lf->type = LSOF_FILE_PPP;
        enter_dev_ch(ctx, print_kptr((KA_T)si->psi.soi_pcb, (char *)NULL, 0));
        break;
    default:
        printunkaf(ctx, fam, 1);
    }
    /*
     * If there are NAME column characters, enter them.
     */
    if (Namech[0])
        enter_nm(ctx, Namech);
}

void process_socket(struct lsof_context *ctx, /* context */
                    int pid,                  /* PID */
                    int32_t fd)               /* FD */
{
    int nb;
    struct socket_fdinfo si;
    /*
     * Get socket information.
     */
    nb = proc_pidfdinfo(pid, fd, PROC_PIDFDSOCKETINFO, &si, sizeof(si));
    if (nb <= 0) {
        (void)err2nm(ctx, "socket");
        return;
    } else if (nb < sizeof(si)) {
        (void)fprintf(
            stderr,
            "%s: PID %d, FD %d: proc_pidfdinfo(PROC_PIDFDSOCKETINFO);\n", Pn,
            pid, fd);
        (void)fprintf(stderr, "      too few bytes; expected %ld, got %d\n",
                      sizeof(si), nb);
        Error(ctx);
    }

    process_socket_common(ctx, &si);
}

#if defined(PROC_PIDLISTFILEPORTS)
void process_fileport_socket(struct lsof_context *ctx, /* context */
                             int pid,                  /* PID */
                             uint32_t fp)              /* FILEPORT */
{
    int nb;
    struct socket_fdinfo si;
    /*
     * Get socket information.
     */
    nb = proc_pidfileportinfo(pid, fp, PROC_PIDFILEPORTSOCKETINFO, &si,
                              sizeof(si));
    if (nb <= 0) {
        (void)err2nm(ctx, "socket");
        return;
    } else if (nb < sizeof(si)) {
        (void)fprintf(stderr,
                      "%s: PID %d, FILEPORT %u: "
                      "proc_pidfileportinfo(PROC_PIDFILEPORTSOCKETINFO);\n",
                      Pn, pid, fp);
        (void)fprintf(stderr, "      too few bytes; expected %ld, got %d\n",
                      sizeof(si), nb);
        Error(ctx);
    }

    process_socket_common(ctx, &si);
}
#endif /* PROC_PIDLISTFILEPORTS */
