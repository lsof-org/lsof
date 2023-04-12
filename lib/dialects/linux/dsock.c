/*
 * dsock.c - Linux socket processing functions for /proc-based lsof
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
#include <sys/xattr.h>
#include "hash.h"

#if defined(HASEPTOPTS) && defined(HASUXSOCKEPT)
/*
 * UNIX endpoint definitions
 */

#    include <sys/socket.h>      /* for AF_NETLINK */
#    include <linux/rtnetlink.h> /* for NETLINK_INET_DIAG */
#    include <linux/sock_diag.h> /* for SOCK_DIAG_BY_FAMILY */
#    include <linux/unix_diag.h> /* for unix_diag_req */
#    include <string.h>          /* memset */
#    include <stdint.h>          /* for unt8_t */
#    include <unistd.h>          /* for getpagesize */
#    define SOCKET_BUFFER_SIZE (getpagesize() < 8192L ? getpagesize() : 8192L)
#endif /* defined(HASEPTOPTS) && defined(HASUXSOCKEPT) */

#if defined(HASSOSTATE)
#    include <linux/net.h> /* for SS_* */
#endif                     /* defined(HASSOSTATE) */

/*
 * Local definitions
 */

#define INOBUCKS                                                               \
    128 /* inode hash bucket count -- must be                                  \
         * a power of two */
#define INOHASH(ino) ((int)((ino * 31415) >> 3) & (INOBUCKS - 1))
#define TCPUDPHASH(ino) ((int)((ino * 31415) >> 3) & (TcpUdp_bucks - 1))
#define TCPUDP6HASH(ino) ((int)((ino * 31415) >> 3) & (TcpUdp6_bucks - 1))

#define IPCBUCKS                                                               \
    4096 /* IPC hash bucket count -- must be                                   \
          * a power of two */

/* If a socket is used for IPC, we store both end points for the socket
 * to the same hash bucket. This makes seaching the counter part of
 * an end point easier. See get_netpeeri(). */
#define TCPUDP_IPC_HASH(tp)                                                    \
    ((int)(((((tp)->faddr * 0x109 + (tp)->laddr * 0x109 +                      \
              (tp)->fport * 0x121 + (tp)->lport * 0x121 +                      \
              (tp)->proto * 0x181) *                                           \
             31415) >>                                                         \
            3) &                                                               \
           (IPCBUCKS - 1)))

#define TCPUDP6_IPC_ADDR_INT32(a, n) (((a)->s6_addr32[n]))
#define TCPUDP6_IPC_ADDR_MK_INT(a)                                             \
    ((int)TCPUDP6_IPC_ADDR_INT32(a, 0x0) * 0x123 +                             \
     (int)TCPUDP6_IPC_ADDR_INT32(a, 0x1) * 0x111 +                             \
     (int)TCPUDP6_IPC_ADDR_INT32(a, 0x2) * 0x149 +                             \
     (int)TCPUDP6_IPC_ADDR_INT32(a, 0x3) * 0x185)

#define TCPUDP6_IPC_HASH(tp)                                                   \
    ((int)((((TCPUDP6_IPC_ADDR_MK_INT(&(tp)->faddr) +                          \
              TCPUDP6_IPC_ADDR_MK_INT(&(tp)->laddr) + (tp)->fport * 0x109 +    \
              (tp)->lport * 0x109 + (tp)->proto * 0x141) *                     \
             31415) >>                                                         \
            3) &                                                               \
           (IPCBUCKS - 1)))

/*
 * Local structures
 */

/*
 * Local static values
 */

static char *ax25st[] = {
    "LISTENING",   /* 0 */
    "SABM SENT",   /* 1 */
    "DISC SENT",   /* 2 */
    "ESTABLISHED", /* 3 */
    "RECOVERY"     /* 4 */
};
#define NAX25ST (sizeof(ax25st) / sizeof(char *))
static char *SCTPPath[] = {
    /* paths to /proc/net STCP info */
    PROCFS "/net/sctp/assocs", /* 0 = /proc/net/sctp/assocs */
    PROCFS "/net/sctp/eps"     /* 1 = /proc/net/sctp/eps */
};
#define NSCTPPATHS sizeof(SCTPPath) / sizeof(char *)
/* path to /proc/net socket status */
static char *SockStatPath = PROCFS "/net/sockstat";

#if defined(HASIPv6)
/* path to /proc/net IPv6 socket
 * status */
static char *SockStatPath6 = PROCFS "/net/sockstat6";
#endif /* defined(HASIPv6) */

/*
 * Local function prototypes
 */

static struct ax25sin *check_ax25(struct lsof_context *ctx, INODETYPE i);

#if defined(HASEPTOPTS) && defined(HASUXSOCKEPT)
static void enter_uxsinfo(struct lsof_context *ctx, uxsin_t *up);
static void fill_uxicino(struct lsof_context *ctx, INODETYPE si, INODETYPE sc);
static void fill_uxpino(struct lsof_context *ctx, INODETYPE si, INODETYPE pi);
static int get_diagmsg(int sockfd);
static void get_uxpeeri(struct lsof_context *ctx);
static void parse_diag(struct lsof_context *ctx, struct unix_diag_msg *dm,
                       int len);
static void prt_uxs(struct lsof_context *ctx, uxsin_t *p, int mk);
#endif /* defined(HASEPTOPTS) && defined(HASUXSOCKEPT) */

#if defined(HASEPTOPTS)
static void enter_netsinfo(struct lsof_context *ctx, struct tcp_udp *tp);
static void get_netpeeri(struct lsof_context *ctx);
#endif /* defined(HASEPTOPTS) */

#if defined(HASIPv6)
#    if defined(HASEPTOPTS)
static void enter_nets6info(struct lsof_context *ctx, struct tcp_udp6 *tp);
static void get_net6peeri(struct lsof_context *ctx);
#    endif /* defined(HASEPTOPTS) */
#endif     /* defined(HASIPv6) */

static struct icmpin *check_icmp(struct lsof_context *ctx, INODETYPE i);
static struct ipxsin *check_ipx(struct lsof_context *ctx, INODETYPE i);
static struct nlksin *check_netlink(struct lsof_context *ctx, INODETYPE i);
static struct packin *check_pack(struct lsof_context *ctx, INODETYPE i);
static struct rawsin *check_raw(struct lsof_context *ctx, INODETYPE i);
static struct sctpsin *check_sctp(struct lsof_context *ctx, INODETYPE i);
static struct tcp_udp *check_tcpudp(struct lsof_context *ctx, INODETYPE i,
                                    enum lsof_protocol *p);
static uxsin_t *check_unix(struct lsof_context *ctx, INODETYPE i);
static void get_ax25(struct lsof_context *ctx, char *p);
static void get_icmp(struct lsof_context *ctx, char *p);
static void get_ipx(struct lsof_context *ctx, char *p);
static void get_netlink(struct lsof_context *ctx, char *p);
static void get_pack(struct lsof_context *ctx, char *p);
static void get_raw(struct lsof_context *ctx, char *p);
static void get_sctp(struct lsof_context *ctx);
static char *get_sctpaddrs(char **fp, int i, int nf, int *x);
static void get_tcpudp(struct lsof_context *ctx, char *p, int pr, int clr);
static void get_unix(struct lsof_context *ctx, char *p);
static int isainb(char *a, char *b);
static void print_ax25info(struct lsof_context *ctx, struct ax25sin *ap);
static void print_ipxinfo(struct lsof_context *ctx, struct ipxsin *ip);
static char *socket_type_to_str(uint32_t ty, int *rf);
static char *netlink_proto_to_str(unsigned int pr);
static enum lsof_protocol ethernet_proto_convert(unsigned int pr);

#if defined(HASIPv6)
static struct rawsin *check_raw6(struct lsof_context *ctx, INODETYPE i);
static struct tcp_udp6 *check_tcpudp6(struct lsof_context *ctx, INODETYPE i,
                                      enum lsof_protocol *p);
static void get_raw6(struct lsof_context *ctx, char *p);
static void get_tcpudp6(struct lsof_context *ctx, char *p, int pr, int clr);
static int hex_ipv6_to_in6(char *as, struct in6_addr *ad);
#endif /* defined(HASIPv6) */

/*
 * build_IPstates() -- build the TCP and UDP state tables
 */
void build_IPstates(struct lsof_context *ctx) {
    if (!TcpSt) {
        (void)enter_IPstate(ctx, "TCP", "ESTABLISHED", TCP_ESTABLISHED);
        (void)enter_IPstate(ctx, "TCP", "SYN_SENT", TCP_SYN_SENT);
        (void)enter_IPstate(ctx, "TCP", "SYN_RECV", TCP_SYN_RECV);
        (void)enter_IPstate(ctx, "TCP", "FIN_WAIT1", TCP_FIN_WAIT1);
        (void)enter_IPstate(ctx, "TCP", "FIN_WAIT2", TCP_FIN_WAIT2);
        (void)enter_IPstate(ctx, "TCP", "TIME_WAIT", TCP_TIME_WAIT);
        (void)enter_IPstate(ctx, "TCP", "CLOSE", TCP_CLOSE);
        (void)enter_IPstate(ctx, "TCP", "CLOSE_WAIT", TCP_CLOSE_WAIT);
        (void)enter_IPstate(ctx, "TCP", "LAST_ACK", TCP_LAST_ACK);
        (void)enter_IPstate(ctx, "TCP", "LISTEN", TCP_LISTEN);
        (void)enter_IPstate(ctx, "TCP", "CLOSING", TCP_CLOSING);
        (void)enter_IPstate(ctx, "TCP", "CLOSED", 0);
        (void)enter_IPstate(ctx, "TCP", (char *)NULL, 0);
    }
    /* Linux kernel reuses tcp state for udp sockets */
    if (!UdpSt) {
        (void)enter_IPstate(ctx, "UDP", "CLOSE", TCP_CLOSE);
        (void)enter_IPstate(ctx, "UDP", (char *)NULL, 0);
    }
}

/*
 * check_ax25() - check for AX25 socket file
 */
static struct ax25sin *check_ax25(struct lsof_context *ctx,
                                  INODETYPE i) /* socket file's inode number */
{
    return HASH_FIND_ELEMENT(AX25sin, INOHASH, struct ax25sin, inode, i);
}

/*
 * check_icmp() - check for ICMP socket
 */
static struct icmpin *check_icmp(struct lsof_context *ctx,
                                 INODETYPE i) /* socket file's inode number */
{
    return HASH_FIND_ELEMENT(Icmpin, INOHASH, struct icmpin, inode, i);
}

/*
 * check_ipx() - check for IPX socket file
 */
static struct ipxsin *check_ipx(struct lsof_context *ctx,
                                INODETYPE i) /* socket file's inode number */
{
    return HASH_FIND_ELEMENT(Ipxsin, INOHASH, struct ipxsin, inode, i);
}

/*
 * check_netlink() - check for Netlink socket file
 */
static struct nlksin *
check_netlink(struct lsof_context *ctx,
              INODETYPE i) /* socket file's inode number */
{
    return HASH_FIND_ELEMENT(Nlksin, INOHASH, struct nlksin, inode, i);
}

/*
 * check_pack() - check for packet file
 */
static struct packin *check_pack(struct lsof_context *ctx,
                                 INODETYPE i) /* packet file's inode number */
{
    return HASH_FIND_ELEMENT(Packin, INOHASH, struct packin, inode, i);
}

/*
 * check_raw() - check for raw socket file
 */

static struct rawsin *check_raw(struct lsof_context *ctx,
                                INODETYPE i) /* socket file's inode number */
{
    return HASH_FIND_ELEMENT(Rawsin, INOHASH, struct rawsin, inode, i);
}

/*
 * check_sctp() - check for SCTP socket file
 */
static struct sctpsin *check_sctp(struct lsof_context *ctx,
                                  INODETYPE i) /* socket file's inode number */
{
    return HASH_FIND_ELEMENT(SCTPsin, INOHASH, struct sctpsin, inode, i);
}

/*
 * check_tcpudp() - check for IPv4 TCP or UDP socket file
 */
static struct tcp_udp *
check_tcpudp(struct lsof_context *ctx,
             INODETYPE i,           /* socket file's inode number */
             enum lsof_protocol *p) /* protocol return */
{
    struct tcp_udp *tp;
    tp = HASH_FIND_ELEMENT(TcpUdp, TCPUDPHASH, struct tcp_udp, inode, i);

    if (tp) {
        switch (tp->proto) {
        case 0:
            *p = LSOF_PROTOCOL_TCP;
            break;
        case 1:
            *p = LSOF_PROTOCOL_UDP;
            break;
        case 2:
            *p = LSOF_PROTOCOL_UDPLITE;
            break;
        default:
            *p = LSOF_PROTOCOL_UNKNOWN;
        }
    }
    return tp;
}

/*
 * check_inet() - check for locally used INET domain socket
 */
static struct tcp_udp *check_inet(struct lsof_context *ctx,
                                  INODETYPE i) /* socket file's inode number */
{
    return HASH_FIND_ELEMENT(TcpUdp, TCPUDPHASH, struct tcp_udp, inode, i);
}

/*
 * clear_netsinfo -- clear allocated INET socket info
 */
void clear_netsinfo(struct lsof_context *ctx) {
    int h;                   /* hash index */
    struct tcp_udp *ti, *tp; /* temporary pointers */

#if defined(HASEPTOPTS)
    pxinfo_t *pp, *pnp;
#endif /* defined(HASEPTOPTS) */

    if (TcpUdp) {
        for (h = 0; h < TcpUdp_bucks; h++) {
            if ((ti = TcpUdp[h])) {
                do {
                    tp = ti->next;

#if defined(HASEPTOPTS)
                    for (pp = ti->pxinfo; pp; pp = pnp) {
                        pnp = pp->next;
                        (void)free((FREE_P *)pp);
                    }
#endif /* defined(HASEPTOPTS) */

                    (void)free((FREE_P *)ti);
                    ti = tp;
                } while (ti);
                TcpUdp[h] = (struct tcp_udp *)NULL;
            }
        }
    }
    if (TcpUdpIPC) {
        for (h = 0; h < IPCBUCKS; h++)
            TcpUdpIPC[h] = (struct tcp_udp *)NULL;
    }
}

#if defined(HASIPv6)
/*
 * check_raw6() - check for raw IPv6 socket file
 */
static struct rawsin *check_raw6(struct lsof_context *ctx,
                                 INODETYPE i) /* socket file's inode number */
{
    return HASH_FIND_ELEMENT(Rawsin6, INOHASH, struct rawsin, inode, i);
}

/*
 * check_tcpudp6() - check for IPv6 TCP or UDP socket file
 */
static struct tcp_udp6 *
check_tcpudp6(struct lsof_context *ctx,
              INODETYPE i,           /* socket file's inode number */
              enum lsof_protocol *p) /* protocol return */
{
    struct tcp_udp6 *tp6;
    tp6 = HASH_FIND_ELEMENT(TcpUdp6, TCPUDP6HASH, struct tcp_udp6, inode, i);

    if (tp6) {
        switch (tp6->proto) {
        case 0:
            *p = LSOF_PROTOCOL_TCP;
            break;
        case 1:
            *p = LSOF_PROTOCOL_UDP;
            break;
        case 2:
            *p = LSOF_PROTOCOL_UDPLITE;
            break;
        default:
            *p = LSOF_PROTOCOL_UNKNOWN;
        }
    }
    return tp6;
}

/*
 * check_inet6() - check for locally used INET6 domain socket
 */
static struct tcp_udp6 *
check_inet6(struct lsof_context *ctx,
            INODETYPE i) /* socket file's inode number */
{
    return HASH_FIND_ELEMENT(TcpUdp6, TCPUDP6HASH, struct tcp_udp6, inode, i);
}

/*
 * clear_nets6info -- clear allocated INET6 socket info
 */
void clear_nets6info(struct lsof_context *ctx) {
    int h;                    /* hash index */
    struct tcp_udp6 *ti, *tp; /* temporary pointers */

#    if defined(HASEPTOPTS)
    pxinfo_t *pp, *pnp;
#    endif /* defined(HASEPTOPTS) */

    if (TcpUdp6) {
        for (h = 0; h < TcpUdp6_bucks; h++) {
            if ((ti = TcpUdp6[h])) {
                do {
                    tp = ti->next;

#    if defined(HASEPTOPTS)
                    for (pp = ti->pxinfo; pp; pp = pnp) {
                        pnp = pp->next;
                        (void)free((FREE_P *)pp);
                    }
#    endif /* defined(HASEPTOPTS) */

                    (void)free((FREE_P *)ti);
                    ti = tp;
                } while (ti);
                TcpUdp6[h] = (struct tcp_udp6 *)NULL;
            }
        }
    }
    if (TcpUdp6IPC) {
        for (h = 0; h < IPCBUCKS; h++)
            TcpUdp6IPC[h] = (struct tcp_udp6 *)NULL;
    }
}

#endif /* defined(HASIPv6) */

/*
 * check_unix() - check for UNIX domain socket
 */
static uxsin_t *check_unix(struct lsof_context *ctx,
                           INODETYPE i) /* socket file's inode number */
{
    return HASH_FIND_ELEMENT(Uxsin, INOHASH, uxsin_t, inode, i);
}

/*
 * clear_uxsinfo -- clear allocated UNIX socket info
 */
void clear_uxsinfo(struct lsof_context *ctx) {
    int h;            /* hash index */
    uxsin_t *ui, *up; /* remporary pointers */

#if defined(HASEPTOPTS) && defined(HASUXSOCKEPT)
    pxinfo_t *pp, *pnp;
#endif /* defined(HASEPTOPTS) && defined(HASUXSOCKEPT) */

    if (!Uxsin)
        return;
    for (h = 0; h < INOBUCKS; h++) {
        if ((ui = Uxsin[h])) {
            do {
                up = ui->next;

#if defined(HASEPTOPTS) && defined(HASUXSOCKEPT)
                for (pp = ui->pxinfo; pp; pp = pnp) {
                    pnp = pp->next;
                    (void)free((FREE_P *)pp);
                }
#endif /* defined(HASEPTOPTS) && defined(HASUXSOCKEPT) */

                if (ui->path)
                    (void)free((FREE_P *)ui->path);
                if (ui->pcb)
                    (void)free((FREE_P *)ui->pcb);
                (void)free((FREE_P *)ui);
                ui = up;
            } while (ui);
            Uxsin[h] = (uxsin_t *)NULL;
        }
    }
}

/*
 * clean_ax25() - cleanup /proc/net/ax25 info
 */
void clean_ax25(struct lsof_context *ctx) {
    struct ax25sin *ap, *np;
    int h;
    if (AX25sin) {
        for (h = 0; h < INOBUCKS; h++) {
            for (ap = AX25sin[h]; ap; ap = np) {
                np = ap->next;
                CLEAN(ap->da);
                CLEAN(ap->dev_ch);
                CLEAN(ap->sa);
                CLEAN(ap);
            }
            AX25sin[h] = (struct ax25sin *)NULL;
        }
        CLEAN(AX25sin);
    }
}

/*
 * get_ax25() - get /proc/net/ax25 info
 */
static void get_ax25(struct lsof_context *ctx,
                     char *p) /* /proc/net/ax25 path */
{
    struct ax25sin *ap, *np;
    FILE *as;
    char buf[MAXPATHLEN], *da = NULL, *dev_ch = NULL, *ep, **fp, *sa = NULL;
    int h;
    INODETYPE inode;
    unsigned long rq, sq, state;
    MALLOC_S len;
    unsigned char rqs, sqs;
    char *vbuf = (char *)NULL;
    size_t vsz = (size_t)0;
    /*
     * Do second time cleanup or first time setup.
     */
    clean_ax25(ctx);
    AX25sin = (struct ax25sin **)calloc(INOBUCKS, sizeof(struct ax25sin *));
    if (!AX25sin) {
        if (ctx->err)
            (void)fprintf(ctx->err,
                          "%s: can't allocate %d AX25 hash pointer bytes\n", Pn,
                          (int)(INOBUCKS * sizeof(struct ax25sin *)));
        Error(ctx);
        return;
    }
    /*
     * Open the /proc/net/ax25 file, assign a page size buffer to the stream,
     * and read it.  Store AX25 socket info in the AX25sin[] hash buckets.
     */
    if (!(as = open_proc_stream(ctx, p, "r", &vbuf, &vsz)))
        return;
    while (fgets(buf, sizeof(buf) - 1, as)) {
        if (get_fields(ctx, buf, (char *)NULL, &fp, (int *)NULL, 0) < 24)
            continue;
        /*
         * /proc/net/ax25 has no title line, a very poor deficiency in its
         * implementation.
         *
         * The ax25_info_show() function in kern module .../net/ax25/af_ax25.c
         * says the format of the lines in the file is:
         *
         *     magic dev src_addr dest_addr,digi1,digi2,.. st vs vr va t1 t1 \
         *     t2 t2 t3 t3 idle idle n2 n2 rtt window paclen Snd-Q Rcv-Q \
         *     inode
         *
         * The code in this function is forced to assume that format is in
         * effect..
         */

        /*
         * Assemble the inode number and see if it has already been recorded.
         * If it has, skip this line.
         */
        ep = (char *)NULL;
        if (!fp[23] || !*fp[23] ||
            (inode = strtoull(fp[23], &ep, 0)) == ULONG_MAX || !ep || *ep)
            continue;
        /* Skip if already exists in hash table */
        if (HASH_FIND_ELEMENT(AX25sin, INOHASH, struct ax25sin, inode, inode))
            continue;
        /*
         * Assemble the send and receive queue values and the state.
         */
        ep = (char *)NULL;
        if (!fp[21] || !*fp[21] ||
            (sq = strtoul(fp[21], &ep, 0)) == ULONG_MAX || !ep || *ep)
            continue;
        sqs = (unsigned char)1;
        ep = (char *)NULL;
        if (!fp[22] || !*fp[22] ||
            (rq = strtoul(fp[22], &ep, 0)) == ULONG_MAX || !ep || *ep)
            continue;
        rqs = (unsigned char)1;
        ep = (char *)NULL;
        if (!fp[4] || !*fp[4] ||
            (state = strtoul(fp[4], &ep, 0)) == ULONG_MAX || !ep || *ep)
            continue;
        /*
         * Allocate space for the destination address.
         */
        if (!fp[3] || !*fp[3])
            da = (char *)NULL;
        else if ((len = strlen(fp[3]))) {
            if (!(da = (char *)malloc(len + 1))) {
                if (ctx->err)
                    (void)fprintf(ctx->err,
                                  "%s: can't allocate %d destination AX25 addr "
                                  "bytes: %s\n",
                                  Pn, (int)(len + 1), fp[3]);
                Error(ctx);
                goto cleanup;
            }
            (void)snpf(da, len + 1, "%s", fp[3]);
        } else
            da = (char *)NULL;
        /*
         * Allocate space for the source address.
         */
        if (!fp[2] || !*fp[2])
            sa = (char *)NULL;
        else if ((len = strlen(fp[2]))) {
            if (!(sa = (char *)malloc(len + 1))) {
                if (ctx->err)
                    (void)fprintf(
                        ctx->err,
                        "%s: can't allocate %d source AX25 address bytes: %s\n",
                        Pn, (int)(len + 1), fp[2]);
                Error(ctx);
                goto cleanup;
            }
            (void)snpf(sa, len + 1, "%s", fp[2]);
        } else
            sa = (char *)NULL;
        /*
         * Allocate space for the device characters.
         */
        if (!fp[1] || !*fp[1])
            dev_ch = (char *)NULL;
        else if ((len = strlen(fp[1]))) {
            if (!(dev_ch = (char *)malloc(len + 1))) {
                if (ctx->err)
                    (void)fprintf(ctx->err,
                                  "%s: can't allocate %d destination AX25 dev "
                                  "bytes: %s\n",
                                  Pn, (int)(len + 1), fp[1]);
                Error(ctx);
                goto cleanup;
            }
            (void)snpf(dev_ch, len + 1, "%s", fp[1]);
        } else
            dev_ch = (char *)NULL;
        /*
         * Allocate space for an ax25sin entry, fill it, and link it to its
         * hash bucket.
         */
        if (!(ap = (struct ax25sin *)malloc(sizeof(struct ax25sin)))) {
            if (ctx->err)
                (void)fprintf(ctx->err,
                              "%s: can't allocate %d byte ax25sin structure\n",
                              Pn, (int)sizeof(struct ax25sin));
            Error(ctx);
            goto cleanup;
        }
        ap->da = da;
        da = NULL;
        ap->dev_ch = dev_ch;
        dev_ch = NULL;
        ap->inode = inode;
        ap->rq = rq;
        ap->rqs = rqs;
        ap->sa = sa;
        sa = NULL;
        ap->sq = sq;
        ap->sqs = sqs;
        ap->state = (int)state;
        HASH_INSERT_ELEMENT(AX25sin, INOHASH, ap, inode);
    }
cleanup:
    if (as)
        (void)fclose(as);
    CLEAN(vbuf);
    CLEAN(da);
    CLEAN(sa);
    CLEAN(dev_ch);
}

#if defined(HASEPTOPTS) && defined(HASUXSOCKEPT)
/*
 * enter_uxsinfo() -- enter unix socket info
 * 	entry	Lf = local file structure pointer
 * 		Lp = local process structure pointer
 */
static void enter_uxsinfo(struct lsof_context *ctx, uxsin_t *up) {
    pxinfo_t *pi;     /* pxinfo_t structure pointer */
    struct lfile *lf; /* local file structure pointer */
    struct lproc *lp; /* local proc structure pointer */
    pxinfo_t *np;     /* new pxinfo_t structure pointer */

    for (pi = up->pxinfo; pi; pi = pi->next) {
        lf = pi->lf;
        lp = &Lproc[pi->lpx];
        if (pi->ino == Lf->inode) {
            if ((lp->pid == Lp->pid) && (lf->fd_type == Lf->fd_type) &&
                (lf->fd_num == Lf->fd_num))
                return;
        }
    }
    if (!(np = (pxinfo_t *)malloc(sizeof(pxinfo_t)))) {
        if (ctx->err)
            (void)fprintf(ctx->err,
                          "%s: no space for pipeinfo in uxsinfo, PID %d\n", Pn,
                          Lp->pid);
        Error(ctx);
        return;
    }
    np->ino = Lf->inode;
    np->lf = Lf;
    np->lpx = Lp - Lproc;
    np->next = up->pxinfo;
    up->pxinfo = np;
}

/*
 * fill_uxicino() -- fill incoming connection inode number
 */
static void fill_uxicino(struct lsof_context *ctx,
                         INODETYPE si, /* UNIX socket inode number */
                         INODETYPE ic) /* incoming UNIX socket connection
                                        * inode number */
{
    uxsin_t *psi; /* pointer to socket's information */
    uxsin_t *pic; /* pointer to incoming connection's
                   * information */

    if ((psi = check_unix(ctx, si))) {
        if (psi->icstat || psi->icons)
            return;
        if ((pic = check_unix(ctx, ic))) {
            psi->icstat = 1;
            psi->icons = pic;
        }
    }
}

/*
 * fill_uxpino() -- fill in UNIX socket's peer inode number
 */
static void fill_uxpino(struct lsof_context *ctx,
                        INODETYPE si, /* UNIX socket inode number */
                        INODETYPE pi) /* UNIX socket peer's inode number */
{
    uxsin_t *pp, *up;

    if ((up = check_unix(ctx, si))) {
        if (!up->peer) {
            if ((pp = check_unix(ctx, pi)))
                up->peer = pp;
        }
    }
}

/*
 * find_uxepti(lf) -- find UNIX socket endpoint info
 */
uxsin_t *find_uxepti(struct lsof_context *ctx,
                     struct lfile *lf) /* pipe's lfile */
{
    uxsin_t *up;

    up = check_unix(ctx, lf->inode);
    return (up ? up->peer : (uxsin_t *)NULL);
}

/*
 * get_diagmsg() -- get UNIX socket's diag message
 */

static int get_diagmsg(int sockfd) /* socket's file descriptor */
{
    struct msghdr msg;         /* message header */
    struct nlmsghdr nlh;       /* header length */
    struct unix_diag_req creq; /* connection request */
    struct sockaddr_nl sa;     /* netlink socket address */
    struct iovec iov[2];       /* I/O vector */
    /*
     * Build and send message to socket's file descriptor, asking for its
     * diagnostic message.
     */
    zeromem((char *)&msg, sizeof(msg));
    zeromem((char *)&sa, sizeof(sa));
    zeromem((char *)&nlh, sizeof(nlh));
    zeromem((char *)&creq, sizeof(creq));
    sa.nl_family = AF_NETLINK;
    creq.sdiag_family = AF_UNIX;
    creq.sdiag_protocol = 0;
    memset((void *)&creq.udiag_states, -1, sizeof(creq.udiag_states));
    creq.udiag_ino = (INODETYPE)0;
    creq.udiag_show = UDIAG_SHOW_PEER | UDIAG_SHOW_ICONS;
    nlh.nlmsg_len = NLMSG_LENGTH(sizeof(creq));
    nlh.nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST;
    nlh.nlmsg_type = SOCK_DIAG_BY_FAMILY;
    iov[0].iov_base = (void *)&nlh;
    iov[0].iov_len = sizeof(nlh);
    iov[1].iov_base = (void *)&creq;
    iov[1].iov_len = sizeof(creq);
    msg.msg_name = (void *)&sa;
    msg.msg_namelen = sizeof(sa);
    msg.msg_iov = iov;
    msg.msg_iovlen = 2;
    return (sendmsg(sockfd, &msg, 0));
}

/*
 * get_uxpeeri() - get UNIX socket peer inode information
 */
static void get_uxpeeri(struct lsof_context *ctx) {
    struct unix_diag_msg *dm;       /* pointer to diag message */
    struct nlmsghdr *hp;            /* netlink structure header pointer */
    int nb = 0;                     /* number of bytes */
    int ns = 0;                     /* netlink socket */
    uint8_t rb[SOCKET_BUFFER_SIZE]; /* receive buffer */
    int rl = 0;                     /* route info length */
    /*
     * Get a netlink socket.
     */
    if ((ns = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_SOCK_DIAG)) == -1) {
        if (ctx->err)
            (void)fprintf(ctx->err, "%s: netlink socket error: %s\n", Pn,
                          strerror(errno));
        return;
        return;
    }
    /*
     * Request peer information.
     */
    if (get_diagmsg(ns) < 0) {
        if (ctx->err)
            (void)fprintf(ctx->err, "%s: netlink peer request error: %s\n", Pn,
                          strerror(errno));
        goto get_uxpeeri_exit;
    }
    /*
     * Receive peer information.
     */
    while (1) {
        if ((nb = recv(ns, rb, sizeof(rb), 0)) <= 0)
            goto get_uxpeeri_exit;
        hp = (struct nlmsghdr *)rb;
        while (NLMSG_OK(hp, nb)) {
            if (hp->nlmsg_type == NLMSG_DONE)
                goto get_uxpeeri_exit;
            if (hp->nlmsg_type == NLMSG_ERROR) {
                if (ctx->err)
                    (void)fprintf(
                        ctx->err,
                        "%s: netlink UNIX socket msg peer info error\n", Pn);
                goto get_uxpeeri_exit;
            }
            dm = (struct unix_diag_msg *)NLMSG_DATA(hp);
            rl = hp->nlmsg_len - NLMSG_LENGTH(sizeof(*dm));
            parse_diag(ctx, dm, rl);
            hp = NLMSG_NEXT(hp, nb);
        }
    }

get_uxpeeri_exit:

    (void)close(ns);
}

/*
 * parse_diag() -- parse UNIX diag message
 */
static void parse_diag(struct lsof_context *ctx,
                       struct unix_diag_msg *dm, /* pointer to diag message */
                       int len)                  /* message length */
{
    struct rtattr *rp;   /* route info pointer */
    int i;               /* tmporary index */
    int icct;            /* incoming connection count */
    uint32_t *icp;       /* incoming connection pointer */
    uint32_t inoc, inop; /* inode numbers */

    if (!dm || (dm->udiag_family != AF_UNIX) || !(inop = dm->udiag_ino) ||
        (len <= 0)) {
        return;
    }
    rp = (struct rtattr *)(dm + 1);
    /*
     * Process route information.
     */
    while (RTA_OK(rp, len)) {
        switch (rp->rta_type) {
        case UNIX_DIAG_PEER:
            if (len < 4) {
                if (ctx->err)
                    (void)fprintf(ctx->err,
                                  "%s: unix_diag: msg length (%d) < 4)\n", Pn,
                                  len);
                return;
            }
            if ((inoc = *(uint32_t *)RTA_DATA(rp))) {
                fill_uxpino(ctx, (INODETYPE)inop, (INODETYPE)inoc);
                fill_uxpino(ctx, (INODETYPE)inoc, (INODETYPE)inop);
            }
            break;
        case UNIX_DIAG_ICONS:
            icct = RTA_PAYLOAD(rp), icp = (uint32_t *)RTA_DATA(rp);

            for (i = 0; i < icct; i += sizeof(uint32_t), icp++) {
                fill_uxicino(ctx, (INODETYPE)inop, (INODETYPE)*icp);
            }
        }
        rp = RTA_NEXT(rp, len);
    }
}

/*
 * prt_uxs() -- print UNIX socket information
 */
static void prt_uxs(struct lsof_context *ctx, uxsin_t *p, /* peer info */
                    int mk) /* 1 == mark for later processing */
{
    struct lproc *ep; /* socket endpoint process */
    struct lfile *ef; /* socket endpoint file */
    int i;            /* temporary index */
    int len;          /* string length */
    char nma[1024];   /* character buffer */
    pxinfo_t *pp;     /* previous pipe info of socket */
    char fd[FDLEN];

    (void)strcpy(nma, "->INO=");
    len = (int)strlen(nma);
    (void)snpf(&nma[len], sizeof(nma) - len - 1, "%" INODEPSPEC "d", p->inode);
    (void)add_nma(ctx, nma, strlen(nma));
    for (pp = p->pxinfo; pp; pp = pp->next) {

        /*
         * Add a linked socket's PID, command name and FD to the name column
         * addition.
         */
        ep = &Lproc[pp->lpx];
        ef = pp->lf;
        print_fd(ef->fd_type, ef->fd_num, fd);
        (void)snpf(nma, sizeof(nma) - 1, "%d,%s,%s%c", ep->pid, ep->cmd, fd,
                   print_access(ef->access));
        (void)add_nma(ctx, nma, strlen(nma));
        if (mk && FeptE == 2) {

            /*
             * Endpoint files have been selected, so mark this
             * one for selection later.
             */
            ef->chend = CHEND_UXS;
            ep->ept |= EPT_UXS_END;
        }
    }
}

/*
 * process_uxsinfo() -- process UNIX socket information, adding it to selected
 *			UNIX socket files and selecting UNIX socket end point
 *			files (if requested)
 */
void process_uxsinfo(struct lsof_context *ctx,
                     int f) /* function:
                             *     0 == process selected socket
                             *     1 == process socket end point
                             */
{
    uxsin_t *p;  /* peer UNIX socket info pointer */
    uxsin_t *tp; /* temporary UNIX socket info pointer */

    if (!FeptE)
        return;
    for (Lf = Lp->file; Lf; Lf = Lf->next) {
        if (Lf->type != LSOF_FILE_UNIX)
            continue;
        switch (f) {
        case 0:

            /*
             * Process already selected socket.
             */
            if (is_file_sel(ctx, Lp, Lf)) {

                /*
                 * This file has been selected by some criterion other than its
                 * being a socket.  Look up the socket's endpoints.
                 */
                p = find_uxepti(ctx, Lf);
                if (p && p->inode)
                    prt_uxs(ctx, p, 1);
                if ((tp = check_unix(ctx, Lf->inode))) {
                    if (tp->icons) {
                        if (tp->icstat) {
                            p = tp->icons;
                            while (p && p != tp) {
                                if (p->inode)
                                    prt_uxs(ctx, p, 1);
                                p = p->icons;
                            }
                        } else {
                            for (p = tp->icons; p && !p->icstat; p = p->icons)
                                ; /* DO NOTHING */
                            if (p && p->inode)
                                prt_uxs(ctx, p, 1);
                        }
                    }
                }
            }
            break;
        case 1:
            if (!is_file_sel(ctx, Lp, Lf) && (Lf->chend & CHEND_UXS)) {

                /*
                 * This is an unselected end point UNIX socket file.  Select it
                 * and add its end point information to peer's name column
                 * addition.
                 */
                Lf->sf = Selflags;
                Lp->pss |= PS_SEC;
                p = find_uxepti(ctx, Lf);
                if (p && p->inode)
                    prt_uxs(ctx, p, 0);
                else if ((tp = check_unix(ctx, Lf->inode))) {
                    if (tp->icons) {
                        if (tp->icstat) {
                            p = tp->icons;
                            while (p && p != tp) {
                                if (p->inode)
                                    prt_uxs(ctx, p, 0);
                                p = p->icons;
                            }
                        } else {
                            for (p = tp->icons; p && !p->icstat; p = p->icons)
                                ; /* DO NOTHING */
                            if (p && p->inode)
                                prt_uxs(ctx, p, 0);
                        }
                    }
                }
            }
            break;
        }
    }
}
#endif /* defined(HASEPTOPTS) && defined(HASUXSOCKEPT) */

#if defined(HASEPTOPTS)
/*
 * enter_netsinfo_common() -- enter inet or inet6 socket info
 * 	tp = tcp/udp on ipv4 or ipv4 socket pointer
 */
static void enter_netsinfo_common(struct lsof_context *ctx, void *tp,
                                  pxinfo_t *(*get_pxinfo)(void *),
                                  void (*set_pxinfo)(void *, pxinfo_t *)) {
    pxinfo_t *pi;     /* pxinfo_t structure pointer */
    struct lfile *lf; /* local file structure pointer */
    struct lproc *lp; /* local proc structure pointer */
    pxinfo_t *np;     /* new pxinfo_t structure pointer */

    for (pi = (*get_pxinfo)(tp); pi; pi = pi->next) {
        lf = pi->lf;
        lp = &Lproc[pi->lpx];
        if (pi->ino == Lf->inode) {
            if ((lp->pid == Lp->pid) && (lf->fd_type == Lf->fd_type) &&
                (lf->fd_num == Lf->fd_num))
                return;
        }
    }
    if (!(np = (pxinfo_t *)malloc(sizeof(pxinfo_t)))) {
        if (ctx->err)
            (void)fprintf(ctx->err,
                          "%s: no space for pipeinfo in netsinfo, PID %d\n", Pn,
                          Lp->pid);
        Error(ctx);
        return;
    }
    np->ino = Lf->inode;
    np->lf = Lf;
    np->lpx = Lp - Lproc;
    np->next = (*get_pxinfo)(tp);
    (*set_pxinfo)(tp, np);
}

/*
 * prt_nets_common() -- print locally used INET or INET6 socket information
 */
static void prt_nets_common(struct lsof_context *ctx, void *p, /* peer info */
                            int mk, /* 1 == mark for later
                                     * processing */
                            pxinfo_t *(*get_pxinfo)(void *),
                            unsigned char chend, short ept_flag) {
    struct lproc *ep; /* socket endpoint process */
    struct lfile *ef; /* socket endpoint file */
    int i;            /* temporary index */
    char nma[1024];   /* character buffer */
    pxinfo_t *pp;     /* previous pipe info of socket */
    char fd[FDLEN];

    for (pp = (*get_pxinfo)(p); pp; pp = pp->next) {

        /*
         * Add a linked socket's PID, command name and FD to the name column
         * addition.
         */
        ep = &Lproc[pp->lpx];
        ef = pp->lf;
        print_fd(ef->fd_type, ef->fd_num, fd);
        (void)snpf(nma, sizeof(nma) - 1, "%d,%s,%s%c", ep->pid, ep->cmd, fd,
                   print_access(ef->access));
        (void)add_nma(ctx, nma, strlen(nma));
        if (mk && FeptE == 2) {

            /*
             * Endpoint files have been selected, so mark this
             * one for selection later.
             */
            ef->chend = chend;
            ep->ept |= ept_flag;
        }
    }
}

/*
 * enter_netsinfo() -- enter inet socket info
 * 	tp = tcp/udp on ipv4 socket pointer
 */
static pxinfo_t *tcp_udp_get_pxinfo(void *vp) {
    struct tcp_udp *tp = vp;
    return tp->pxinfo;
}

static void tcp_udp_set_pxinfo(void *vp, pxinfo_t *np) {
    struct tcp_udp *tp = vp;
    tp->pxinfo = np;
}

static void enter_netsinfo(struct lsof_context *ctx, struct tcp_udp *tp) {
    enter_netsinfo_common(ctx, tp, tcp_udp_get_pxinfo, tcp_udp_set_pxinfo);
}

/*
 * find_netsepti(lf) -- find locally used INET socket endpoint info
 */
static struct tcp_udp *find_netsepti(struct lsof_context *ctx,
                                     struct lfile *lf) /* socket's lfile */
{
    struct tcp_udp *tp;

    tp = check_inet(ctx, lf->inode);
    return (tp ? tp->ipc_peer : (struct tcp_udp *)NULL);
}

/*
 * get_netpeeri() - get INET socket peer inode information
 */
static void get_netpeeri(struct lsof_context *ctx) {
    int h;
    struct tcp_udp *np, *tp;

    for (h = 0; h < IPCBUCKS; h++) {
        for (tp = TcpUdpIPC[h]; tp; tp = tp->ipc_next) {
            if (tp->ipc_peer)
                continue;
            for (np = TcpUdpIPC[h]; np; np = np->ipc_next) {
                if (np->ipc_peer)
                    continue;
                if (tp->faddr == np->laddr && tp->laddr == np->faddr &&
                    tp->fport == np->lport && tp->lport == np->fport &&
                    tp->proto == np->proto) {
                    tp->ipc_peer = np;
                    np->ipc_peer = tp;
                    break;
                }
            }
        }
    }
}

/*
 * prt_nets() -- print locally used INET socket information
 */
static void prt_nets(struct lsof_context *ctx,
                     struct tcp_udp *p, /* peer info */
                     int mk)            /* 1 == mark for later processing */
{
    prt_nets_common(ctx, p, mk, tcp_udp_get_pxinfo, CHEND_NETS, EPT_NETS_END);
}

/*
 * process_netsinfo() -- process locally used INET socket information, adding
 *			it to selected INET socket files and selecting INET
 *			socket end point files (if requested)
 */
void process_netsinfo(struct lsof_context *ctx,
                      int f) /* function:
                              *     0 == process selected socket
                              *     1 == process socket end point
                              */
{
    struct tcp_udp *p; /* peer INET socket info pointer */

    if (!FeptE)
        return;
    for (Lf = Lp->file; Lf; Lf = Lf->next) {
#    if defined(HASIPv6)
        enum lsof_file_type type = LSOF_FILE_IPV4;
#    else  /* !defined(HASIPv6) */
        enum lsof_file_type type = LSOF_FILE_INET;
#    endif /* defined(HASIPv6) */
        if (Lf->type != type)
            continue;
        switch (f) {
        case 0:

            /*
             * Process already selected socket.
             */
            if (is_file_sel(ctx, Lp, Lf)) {

                /*
                 * This file has been selected by some criterion other than its
                 * being a socket.  Look up the socket's endpoints.
                 */
                p = find_netsepti(ctx, Lf);
                if (p && p->inode)
                    prt_nets(ctx, p, 1);
            }
            break;
        case 1:
            if (!is_file_sel(ctx, Lp, Lf) && (Lf->chend & CHEND_NETS)) {

                /*
                 * This is an unselected end point INET socket file.  Select it
                 * and add its end point information to peer's name column
                 * addition.
                 */
                Lf->sf = Selflags;
                Lp->pss |= PS_SEC;
                p = find_netsepti(ctx, Lf);
                if (p && p->inode)
                    prt_nets(ctx, p, 0);
            }
            break;
        }
    }
}
#endif /* defined(HASEPTOPTS) */

#if defined(HASIPv6)
#    if defined(HASEPTOPTS)
/*
 * enter_nets6info() -- enter inet socket info
 * 	tp = tcp/udp on ipv6 socket pointer
 */
static pxinfo_t *tcp_udp6_get_pxinfo(void *vp) {
    struct tcp_udp6 *tp = vp;
    return tp->pxinfo;
}

static void tcp_udp6_set_pxinfo(void *vp, pxinfo_t *np) {
    struct tcp_udp6 *tp = vp;
    tp->pxinfo = np;
}

static void enter_nets6info(struct lsof_context *ctx, struct tcp_udp6 *tp) {
    enter_netsinfo_common(ctx, tp, tcp_udp6_get_pxinfo, tcp_udp6_set_pxinfo);
}

/*
 * find_nets6epti(lf) -- find locally used INET6 socket endpoint info
 */
static struct tcp_udp6 *find_nets6epti(struct lsof_context *ctx,
                                       struct lfile *lf) /* socket's lfile */
{
    struct tcp_udp6 *tp;

    tp = check_inet6(ctx, lf->inode);
    return (tp ? tp->ipc_peer : (struct tcp_udp6 *)NULL);
}

/*
 * get_net6peeri() - get INET6 socket peer inode information
 */
static void get_net6peeri(struct lsof_context *ctx) {
    int h;
    struct tcp_udp6 *np, *tp;

    for (h = 0; h < IPCBUCKS; h++) {
        for (tp = TcpUdp6IPC[h]; tp; tp = tp->ipc_next) {
            if (tp->ipc_peer)
                continue;
            for (np = TcpUdp6IPC[h]; np; np = np->ipc_next) {
                if (np->ipc_peer)
                    continue;
                if (IN6_ARE_ADDR_EQUAL(&tp->faddr, &np->laddr) &&
                    IN6_ARE_ADDR_EQUAL(&tp->laddr, &np->faddr) &&
                    tp->fport == np->lport && tp->lport == np->fport &&
                    tp->proto == np->proto) {
                    tp->ipc_peer = np;
                    np->ipc_peer = tp;
                    break;
                }
            }
        }
    }
}

/*
 * prt_nets6() -- print locally used INET6 socket information
 */
static void prt_nets6(struct lsof_context *ctx,
                      struct tcp_udp6 *p, /* peer info */
                      int mk)             /* 1 == mark for later processing */
{
    prt_nets_common(ctx, p, mk, tcp_udp6_get_pxinfo, CHEND_NETS6,
                    EPT_NETS6_END);
}

/*
 * process_nets6info() -- process locally used INET6 socket information, adding
 *			it to selected INET6 socket files and selecting INET6
 *			socket end point files (if requested)
 */
void process_nets6info(struct lsof_context *ctx,
                       int f) /* function:
                               *     0 == process selected socket
                               *     1 == process socket end point
                               */
{
    struct tcp_udp6 *p; /* peer INET6 socket info pointer */

    if (!FeptE)
        return;
    for (Lf = Lp->file; Lf; Lf = Lf->next) {
        if (Lf->type != LSOF_FILE_IPV6)
            continue;
        switch (f) {
        case 0:

            /*
             * Process already selected socket.
             */
            if (is_file_sel(ctx, Lp, Lf)) {
                /*
                 * This file has been selected by some criterion other than its
                 * being a socket.  Look up the socket's endpoints.
                 */
                p = find_nets6epti(ctx, Lf);
                if (p && p->inode)
                    prt_nets6(ctx, p, 1);
            }
            break;
        case 1:
            if (!is_file_sel(ctx, Lp, Lf) && (Lf->chend & CHEND_NETS6)) {

                /*
                 * This is an unselected end point INET6 socket file.  Select it
                 * and add its end point information to peer's name column
                 * addition.
                 */
                Lf->sf = Selflags;
                Lp->pss |= PS_SEC;
                p = find_nets6epti(ctx, Lf);
                if (p && p->inode)
                    prt_nets6(ctx, p, 0);
            }
            break;
        }
    }
}
#    endif /* defined(HASEPTOPTS) */
#endif     /* defined(HASIPv6) */

/*
 * clean_icmp() - cleanup ICMP net info
 */
void clean_icmp(struct lsof_context *ctx) {
    struct icmpin *np, *icmpp;
    int h;
    if (Icmpin) {
        for (h = 0; h < INOBUCKS; h++) {
            for (icmpp = Icmpin[h]; icmpp; icmpp = np) {
                np = icmpp->next;
                CLEAN(icmpp->la);
                CLEAN(icmpp->ra);
                CLEAN(icmpp);
            }
            Icmpin[h] = (struct icmpin *)NULL;
        }
        CLEAN(Icmpin);
    }
}

/*
 * get_icmp() - get ICMP net info
 */
static void get_icmp(struct lsof_context *ctx,
                     char *p) /* /proc/net/icmp path */
{
    char buf[MAXPATHLEN], *ep, **fp, *la = NULL, *ra = NULL;
    int fl = 1;
    int h;
    INODETYPE inode;
    struct icmpin *np, *icmpp;
    MALLOC_S lal, ral;
    char *vbuf = (char *)NULL;
    size_t vsz = (size_t)0;
    FILE *xs;
    /*
     * Do second time cleanup or first time setup.
     */
    clean_icmp(ctx);
    Icmpin = (struct icmpin **)calloc(INOBUCKS, sizeof(struct icmpin *));
    if (!Icmpin) {
        if (ctx->err)
            (void)fprintf(ctx->err,
                          "%s: can't allocate %d icmp hash pointer bytes\n", Pn,
                          (int)(INOBUCKS * sizeof(struct icmpin *)));
        Error(ctx);
        return;
    }
    /*
     * Open the /proc/net/icmp file, assign a page size buffer to its stream,
     * and read the file.  Store icmp info in the Icmpin[] hash buckets.
     */
    if (!(xs = open_proc_stream(ctx, p, "r", &vbuf, &vsz)))
        return;
    while (fgets(buf, sizeof(buf) - 1, xs)) {
        if (get_fields(ctx, buf, (char *)NULL, &fp, (int *)NULL, 0) < 11)
            continue;
        if (fl) {

            /*
             * Check the column labels in the first line.
             *
             * NOTE:
             *       In column header, "inode" is at the 11th column.
             *       However, in data rows, inode appears at the 9th column.
             *
             *       In column header, "tx_queue" and "rx_queue" are separated
             *       by a space.  It is the same for "tr" and "tm->when"; in
             *       data rows they are connected with ":".
             */
            if (!fp[1] || strcmp(fp[1], "local_address") || !fp[2] ||
                strcmp(fp[2], "rem_address") || !fp[11] ||
                strcmp(fp[11], "inode")) {
                if (ctx->err && !Fwarn) {
                    (void)fprintf(ctx->err,
                                  "%s: WARNING: unsupported format: %s\n", Pn,
                                  p);
                }
                break;
            }
            fl = 0;
            continue;
        }
        /*
         * Assemble the inode number and see if the inode is already
         * recorded.
         */
        ep = (char *)NULL;
        if (!fp[9] || !*fp[9] ||
            (inode = strtoull(fp[9], &ep, 0)) == ULONG_MAX || !ep || *ep)
            continue;
        /* Skip if already exists in hash table */
        if (HASH_FIND_ELEMENT(Icmpin, INOHASH, struct icmpin, inode, inode))
            continue;
        /*
         * Save the local address, and remote address.
         */
        if (!fp[1] || !*fp[1] || (lal = strlen(fp[1])) < 1) {
            la = (char *)NULL;
            lal = (MALLOC_S)0;
        } else {
            if (!(la = (char *)malloc(lal + 1))) {
                if (ctx->err)
                    (void)fprintf(
                        ctx->err,
                        "%s: can't allocate %d local icmp address bytes: %s\n",
                        Pn, (int)(lal + 1), fp[1]);
                Error(ctx);
                goto cleanup;
            }
            (void)snpf(la, lal + 1, "%s", fp[1]);
        }
        if (!fp[2] || !*fp[2] || (ral = strlen(fp[2])) < 1) {
            ra = (char *)NULL;
            ral = (MALLOC_S)0;
        } else {
            if (!(ra = (char *)malloc(ral + 1))) {
                if (ctx->err)
                    (void)fprintf(
                        ctx->err,
                        "%s: can't allocate %d remote icmp address bytes: %s\n",
                        Pn, (int)(ral + 1), fp[2]);
                Error(ctx);
                goto cleanup;
            }
            (void)snpf(ra, ral + 1, "%s", fp[2]);
        }
        /*
         * Allocate space for a icmpin entry, fill it, and link it to its
         * hash bucket.
         */
        if (!(icmpp = (struct icmpin *)malloc(sizeof(struct icmpin)))) {
            if (ctx->err)
                (void)fprintf(ctx->err,
                              "%s: can't allocate %d byte icmp structure\n", Pn,
                              (int)sizeof(struct icmpin));
            Error(ctx);
            goto cleanup;
        }
        icmpp->inode = inode;
        icmpp->la = la;
        la = NULL;
        icmpp->lal = lal;
        icmpp->ra = ra;
        ra = NULL;
        icmpp->ral = ral;
        HASH_INSERT_ELEMENT(Icmpin, INOHASH, icmpp, inode);
    }
cleanup:
    (void)fclose(xs);
    CLEAN(vbuf);
    CLEAN(ra);
    CLEAN(la);
}

/*
 * clean_ipx() - cleanup IPX net info
 */
void clean_ipx(struct lsof_context *ctx) {
    struct ipxsin *ip, *np;
    int h;
    if (Ipxsin) {
        for (h = 0; h < INOBUCKS; h++) {
            for (ip = Ipxsin[h]; ip; ip = np) {
                np = ip->next;
                CLEAN(ip->la);
                CLEAN(ip->ra);
                CLEAN(ip);
            }
            Ipxsin[h] = (struct ipxsin *)NULL;
        }
        CLEAN(Ipxsin);
    }
}

/*
 * get_ipx() - get /proc/net/ipx info
 */
static void get_ipx(struct lsof_context *ctx, char *p) /* /proc/net/ipx path */
{
    char buf[MAXPATHLEN], *ep, **fp, *la = NULL, *ra = NULL;
    int fl = 1;
    int h;
    INODETYPE inode;
    unsigned long rxq, state, txq;
    struct ipxsin *ip, *np;
    MALLOC_S len;
    char *vbuf = (char *)NULL;
    size_t vsz = (size_t)0;
    FILE *xs;
    /*
     * Do second time cleanup or first time setup.
     */
    clean_ipx(ctx);
    Ipxsin = (struct ipxsin **)calloc(INOBUCKS, sizeof(struct ipxsin *));
    if (!Ipxsin) {
        if (ctx->err)
            (void)fprintf(ctx->err,
                          "%s: can't allocate %d IPX hash pointer bytes\n", Pn,
                          (int)(INOBUCKS * sizeof(struct ipxsin *)));
        Error(ctx);
        return;
    }
    /*
     * Open the /proc/net/ipx file, assign a page size buffer to the stream,
     * and read it.  Store IPX socket info in the Ipxsin[] hash buckets.
     */
    if (!(xs = open_proc_stream(ctx, p, "r", &vbuf, &vsz)))
        return;
    while (fgets(buf, sizeof(buf) - 1, xs)) {
        if (get_fields(ctx, buf, (char *)NULL, &fp, (int *)NULL, 0) < 7)
            continue;
        if (fl) {

            /*
             * Check the column labels in the first line.
             */
            if (!fp[0] || strcmp(fp[0], "Local_Address") || !fp[1] ||
                strcmp(fp[1], "Remote_Address") || !fp[2] ||
                strcmp(fp[2], "Tx_Queue") || !fp[3] ||
                strcmp(fp[3], "Rx_Queue") || !fp[4] || strcmp(fp[4], "State") ||
                !fp[5] || strcmp(fp[5], "Uid") || !fp[6] ||
                strcmp(fp[6], "Inode")) {
                if (ctx->err && !Fwarn) {
                    (void)fprintf(ctx->err,
                                  "%s: WARNING: unsupported format: %s\n", Pn,
                                  p);
                }
                break;
            }
            fl = 0;
            continue;
        }
        /*
         * Assemble the inode number and see if the inode is already
         * recorded.
         */
        ep = (char *)NULL;
        if (!fp[6] || !*fp[6] ||
            (inode = strtoull(fp[6], &ep, 0)) == ULONG_MAX || !ep || *ep)
            continue;
        /* Skip if already exists in hash table */
        if (HASH_FIND_ELEMENT(Ipxsin, INOHASH, struct ipxsin, inode, inode))
            continue;
        /*
         * Assemble the transmit and receive queue values and the state.
         */
        ep = (char *)NULL;
        if (!fp[2] || !*fp[2] || (txq = strtoul(fp[2], &ep, 16)) == ULONG_MAX ||
            !ep || *ep)
            continue;
        ep = (char *)NULL;
        if (!fp[3] || !*fp[3] || (rxq = strtoul(fp[3], &ep, 16)) == ULONG_MAX ||
            !ep || *ep)
            continue;
        ep = (char *)NULL;
        if (!fp[4] || !*fp[4] ||
            (state = strtoul(fp[4], &ep, 16)) == ULONG_MAX || !ep || *ep)
            continue;
        /*
         * Allocate space for the local address, unless it is "Not_Connected".
         */
        if (!fp[0] || !*fp[0] || strcmp(fp[0], "Not_Connected") == 0)
            la = (char *)NULL;
        else if ((len = strlen(fp[0]))) {
            if (!(la = (char *)malloc(len + 1))) {
                if (ctx->err)
                    (void)fprintf(
                        ctx->err,
                        "%s: can't allocate %d local IPX address bytes: %s\n",
                        Pn, (int)(len + 1), fp[0]);
                Error(ctx);
                goto cleanup;
            }
            (void)snpf(la, len + 1, "%s", fp[0]);
        } else
            la = (char *)NULL;
        /*
         * Allocate space for the remote address, unless it is "Not_Connected".
         */
        if (!fp[1] || !*fp[1] || strcmp(fp[1], "Not_Connected") == 0)
            ra = (char *)NULL;
        else if ((len = strlen(fp[1]))) {
            if (!(ra = (char *)malloc(len + 1))) {
                if (ctx->err)
                    (void)fprintf(
                        ctx->err,
                        "%s: can't allocate %d remote IPX address bytes: %s\n",
                        Pn, (int)(len + 1), fp[1]);
                Error(ctx);
                goto cleanup;
            }
            (void)snpf(ra, len + 1, "%s", fp[1]);
        } else
            ra = (char *)NULL;
        /*
         * Allocate space for an ipxsin entry, fill it, and link it to its
         * hash bucket.
         */
        if (!(ip = (struct ipxsin *)malloc(sizeof(struct ipxsin)))) {
            if (ctx->err)
                (void)fprintf(ctx->err,
                              "%s: can't allocate %d byte ipxsin structure\n",
                              Pn, (int)sizeof(struct ipxsin));
            Error(ctx);
            goto cleanup;
        }
        ip->inode = inode;
        ip->la = la;
        la = NULL;
        ip->ra = ra;
        ra = NULL;
        ip->txq = txq;
        ip->rxq = rxq;
        ip->state = (int)state;
        HASH_INSERT_ELEMENT(Ipxsin, INOHASH, ip, inode);
    }
cleanup:
    (void)fclose(xs);
    CLEAN(vbuf);
    CLEAN(la);
    CLEAN(ra);
}

/*
 * clean_netlink() - cleanup netlink net info
 */
void clean_netlink(struct lsof_context *ctx) {
    struct nlksin *np, *lp;
    int h;
    if (Nlksin) {
        for (h = 0; h < INOBUCKS; h++) {
            for (lp = Nlksin[h]; lp; lp = np) {
                np = lp->next;
                CLEAN(lp);
            }
            Nlksin[h] = (struct nlksin *)NULL;
        }
        CLEAN(Nlksin);
    }
}

/*
 * get_netlink() - get /proc/net/netlink info
 */
static void get_netlink(struct lsof_context *ctx,
                        char *p) /* /proc/net/netlink path */
{
    char buf[MAXPATHLEN], *ep, **fp;
    int fr = 1;
    int h, pr;
    INODETYPE inode;
    struct nlksin *np, *lp;
    char *vbuf = (char *)NULL;
    size_t vsz = (size_t)0;
    FILE *xs;
    /*
     * Do second time cleanup or first time setup.
     */
    clean_netlink(ctx);
    Nlksin = (struct nlksin **)calloc(INOBUCKS, sizeof(struct nlksin *));
    if (!Nlksin) {
        if (ctx->err)
            (void)fprintf(ctx->err,
                          "%s: can't allocate %d netlink hash pointer bytes\n",
                          Pn, (int)(INOBUCKS * sizeof(struct nlksin *)));
        Error(ctx);
        return;
    }
    /*
     * Open the /proc/net/netlink file, assign a page size buffer to its stream,
     * and read the file.  Store Netlink info in the Nlksin[] hash buckets.
     */
    if (!(xs = open_proc_stream(ctx, p, "r", &vbuf, &vsz)))
        return;
    while (fgets(buf, sizeof(buf) - 1, xs)) {
        if (get_fields(ctx, buf, (char *)NULL, &fp, (int *)NULL, 0) < 10)
            continue;
        if (fr) {

            /*
             * Check the column labels in the first line.
             */
            if (!fp[1] || strcmp(fp[1], "Eth") || !fp[9] ||
                strcmp(fp[9], "Inode")) {
                if (ctx->err && !Fwarn) {
                    (void)fprintf(ctx->err,
                                  "%s: WARNING: unsupported format: %s\n", Pn,
                                  p);
                }
                break;
            }
            fr = 0;
            continue;
        }
        /*
         * Assemble the inode number and see if the inode is already
         * recorded.
         */
        ep = (char *)NULL;
        if (!fp[9] || !*fp[9] ||
            (inode = strtoull(fp[9], &ep, 0)) == ULONG_MAX || !ep || *ep)
            continue;
        /* Skip if already exists in hash table */
        if (HASH_FIND_ELEMENT(Nlksin, INOHASH, struct nlksin, inode, inode))
            continue;
        /*
         * Save the protocol from the Eth column.
         */
        if (!fp[1] || !*fp[1] || (strlen(fp[1])) < 1)
            continue;
        pr = atoi(fp[1]);
        /*
         * Allocate space for a nlksin entry, fill it, and link it to its
         * hash bucket.
         */
        if (!(lp = (struct nlksin *)malloc(sizeof(struct nlksin)))) {
            if (ctx->err)
                (void)fprintf(ctx->err,
                              "%s: can't allocate %d byte Netlink structure\n",
                              Pn, (int)sizeof(struct nlksin));
            Error(ctx);
            goto cleanup;
        }
        lp->inode = inode;
        lp->pr = pr;
        HASH_INSERT_ELEMENT(Nlksin, INOHASH, lp, inode);
    }
cleanup:
    (void)fclose(xs);
    CLEAN(vbuf);
}

/*
 * clean_pack() - cleanup packet net info
 */
void clean_pack(struct lsof_context *ctx) {
    struct packin *np, *pp;
    int h;
    if (Packin) {
        for (h = 0; h < INOBUCKS; h++) {
            for (pp = Packin[h]; pp; pp = np) {
                np = pp->next;
                (void)free((FREE_P *)pp);
            }
            Packin[h] = (struct packin *)NULL;
        }
        CLEAN(Packin);
    }
}

/*
 * get_pack() - get /proc/net/packet info
 */
static void get_pack(struct lsof_context *ctx, char *p) /* /proc/net/raw path */
{
    char buf[MAXPATHLEN], *ep, **fp;
    int fl = 1;
    int h, ty;
    INODETYPE inode;
    struct packin *np, *pp;
    unsigned long pr;
    char *vbuf = (char *)NULL;
    size_t vsz = (size_t)0;
    FILE *xs;
    /*
     * Do second time cleanup or first time setup.
     */
    clean_pack(ctx);
    Packin = (struct packin **)calloc(INOBUCKS, sizeof(struct packin *));
    if (!Packin) {
        if (ctx->err)
            (void)fprintf(ctx->err,
                          "%s: can't allocate %d packet hash pointer bytes\n",
                          Pn, (int)(INOBUCKS * sizeof(struct packin *)));
        Error(ctx);
        return;
    }
    /*
     * Open the /proc/net/packet file, assign a page size buffer to its stream,
     * and read the file.  Store packet info in the Packin[] hash buckets.
     */
    if (!(xs = open_proc_stream(ctx, p, "r", &vbuf, &vsz)))
        return;
    while (fgets(buf, sizeof(buf) - 1, xs)) {
        if (get_fields(ctx, buf, (char *)NULL, &fp, (int *)NULL, 0) < 9)
            continue;
        if (fl) {

            /*
             * Check the column labels in the first line.
             */
            if (!fp[2] || strcmp(fp[2], "Type") || !fp[3] ||
                strcmp(fp[3], "Proto") || !fp[8] || strcmp(fp[8], "Inode")) {
                if (ctx->err && !Fwarn) {
                    (void)fprintf(ctx->err,
                                  "%s: WARNING: unsupported format: %s\n", Pn,
                                  p);
                }
                break;
            }
            fl = 0;
            continue;
        }
        /*
         * Assemble the inode number and see if the inode is already
         * recorded.
         */
        ep = (char *)NULL;
        if (!fp[8] || !*fp[8] ||
            (inode = strtoull(fp[8], &ep, 0)) == ULONG_MAX || !ep || *ep)
            continue;
        /* Skip if already exists in hash table */
        if (HASH_FIND_ELEMENT(Packin, INOHASH, struct packin, inode, inode))
            continue;
        /*
         * Save the socket type and protocol.
         */
        if (!fp[2] || !*fp[2] || (strlen(fp[2])) < 1)
            continue;
        ty = atoi(fp[2]);
        ep = (char *)NULL;
        if (!fp[3] || !*fp[3] || (strlen(fp[3]) < 1) ||
            ((pr = strtoul(fp[3], &ep, 16)) == ULONG_MAX) || !ep || *ep)
            continue;
        /*
         * Allocate space for a packin entry, fill it, and link it to its
         * hash bucket.
         */
        if (!(pp = (struct packin *)malloc(sizeof(struct packin)))) {
            if (ctx->err)
                (void)fprintf(ctx->err,
                              "%s: can't allocate %d byte packet structure\n",
                              Pn, (int)sizeof(struct packin));
            Error(ctx);
            goto cleanup;
        }
        pp->inode = inode;
        pp->pr = (int)pr;
        pp->ty = ty;
        HASH_INSERT_ELEMENT(Packin, INOHASH, pp, inode);
    }
cleanup:
    (void)fclose(xs);
    CLEAN(vbuf);
}

/*
 * clean_raw() - cleanup raw net info
 */
void clean_raw(struct lsof_context *ctx) {
    struct rawsin *np, *rp;
    int h;
    if (Rawsin) {
        for (h = 0; h < INOBUCKS; h++) {
            for (rp = Rawsin[h]; rp; rp = np) {
                np = rp->next;
                CLEAN(rp->la);
                CLEAN(rp->ra);
                (void)free((FREE_P *)rp);
            }
            Rawsin[h] = (struct rawsin *)NULL;
        }
        CLEAN(Rawsin);
    }
}

/*
 * get_raw() - get /proc/net/raw info
 */
static void get_raw(struct lsof_context *ctx, char *p) /* /proc/net/raw path */
{
    char buf[MAXPATHLEN], *ep, **fp, *la = NULL, *ra = NULL, *sp = NULL;
    int h;
    INODETYPE inode;
    int nf = 12;
    struct rawsin *np, *rp;
    MALLOC_S lal, ral, spl;
    char *vbuf = (char *)NULL;
    size_t vsz = (size_t)0;
    FILE *xs;
    /*
     * Do second time cleanup or first time setup.
     */
    clean_raw(ctx);
    Rawsin = (struct rawsin **)calloc(INOBUCKS, sizeof(struct rawsin *));
    if (!Rawsin) {
        if (ctx->err)
            (void)fprintf(ctx->err,
                          "%s: can't allocate %d raw hash pointer bytes\n", Pn,
                          (int)(INOBUCKS * sizeof(struct rawsin *)));
        Error(ctx);
        return;
    }
    /*
     * Open the /proc/net/raw file, assign a page size buffer to its stream,
     * and read the file.  Store raw socket info in the Rawsin[] hash buckets.
     */
    if (!(xs = open_proc_stream(ctx, p, "r", &vbuf, &vsz)))
        return;
    while (fgets(buf, sizeof(buf) - 1, xs)) {
        if (get_fields(ctx, buf, (char *)NULL, &fp, (int *)NULL, 0) < nf)
            continue;
        if (nf == 12) {

            /*
             * Check the column labels in the first line.
             */
            if (!fp[1] || strcmp(fp[1], "local_address") || !fp[2] ||
                strcmp(fp[2], "rem_address") || !fp[3] || strcmp(fp[3], "st") ||
                !fp[11] || strcmp(fp[11], "inode")) {
                if (ctx->err && !Fwarn) {
                    (void)fprintf(ctx->err,
                                  "%s: WARNING: unsupported format: %s\n", Pn,
                                  p);
                }
                break;
            }
            nf = 10;
            continue;
        }
        /*
         * Assemble the inode number and see if the inode is already
         * recorded.
         */
        ep = (char *)NULL;
        if (!fp[9] || !*fp[9] ||
            (inode = strtoull(fp[9], &ep, 0)) == ULONG_MAX || !ep || *ep)
            continue;
        /* Skip if already exists in hash table */
        if (HASH_FIND_ELEMENT(Rawsin, INOHASH, struct rawsin, inode, inode))
            continue;
        /*
         * Save the local address, remote address, and state.
         */
        if (!fp[1] || !*fp[1] || (lal = strlen(fp[1])) < 1) {
            la = (char *)NULL;
            lal = (MALLOC_S)0;
        } else {
            if (!(la = (char *)malloc(lal + 1))) {
                if (ctx->err)
                    (void)fprintf(
                        ctx->err,
                        "%s: can't allocate %d local raw address bytes: %s\n",
                        Pn, (int)(lal + 1), fp[1]);
                Error(ctx);
                goto cleanup;
            }
            (void)snpf(la, lal + 1, "%s", fp[1]);
        }
        if (!fp[2] || !*fp[2] || (ral = strlen(fp[2])) < 1) {
            ra = (char *)NULL;
            ral = (MALLOC_S)0;
        } else {
            if (!(ra = (char *)malloc(ral + 1))) {
                if (ctx->err)
                    (void)fprintf(
                        ctx->err,
                        "%s: can't allocate %d remote raw address bytes: %s\n",
                        Pn, (int)(ral + 1), fp[2]);
                Error(ctx);
                goto cleanup;
            }
            (void)snpf(ra, ral + 1, "%s", fp[2]);
        }
        if (!fp[3] || !*fp[3] || (spl = strlen(fp[3])) < 1) {
            sp = (char *)NULL;
            spl = (MALLOC_S)0;
        } else {
            if (!(sp = (char *)malloc(spl + 1))) {
                if (ctx->err)
                    (void)fprintf(
                        ctx->err,
                        "%s: can't allocate %d remote raw state bytes: %s\n",
                        Pn, (int)(spl + 1), fp[2]);
                Error(ctx);
                goto cleanup;
            }
            (void)snpf(sp, spl + 1, "%s", fp[3]);
        }
        /*
         * Allocate space for an rawsin entry, fill it, and link it to its
         * hash bucket.
         */
        if (!(rp = (struct rawsin *)malloc(sizeof(struct rawsin)))) {
            if (ctx->err)
                (void)fprintf(ctx->err,
                              "%s: can't allocate %d byte rawsin structure\n",
                              Pn, (int)sizeof(struct rawsin));
            Error(ctx);
            goto cleanup;
        }
        rp->inode = inode;
        rp->la = la;
        la = NULL;
        rp->lal = lal;
        rp->ra = ra;
        ra = NULL;
        rp->ral = ral;
        rp->sp = sp;
        sp = NULL;
        rp->spl = spl;
        HASH_INSERT_ELEMENT(Rawsin, INOHASH, rp, inode);
    }
cleanup:
    (void)fclose(xs);
    CLEAN(vbuf);
    CLEAN(la);
    CLEAN(ra);
    CLEAN(sp);
}

/*
 * clean_sctp() - cleanup sctp net info
 */
void clean_sctp(struct lsof_context *ctx) {
    struct sctpsin *sp, *np;
    int h;
    if (SCTPsin) {
        for (h = 0; h < INOBUCKS; h++) {
            for (sp = SCTPsin[h]; sp; sp = np) {
                np = sp->next;
                CLEAN(sp->addr);
                CLEAN(sp->assocID);
                CLEAN(sp->lport);
                CLEAN(sp->rport);
                CLEAN(sp->laddrs);
                CLEAN(sp->raddrs);
                (void)free((FREE_P *)sp);
            }
            SCTPsin[h] = (struct sctpsin *)NULL;
        }
        CLEAN(SCTPsin);
    }
}

/*
 * get_sctp() - get /proc/net/sctp/assocs info
 */
static void get_sctp(struct lsof_context *ctx) {
    char buf[MAXPATHLEN], *a = NULL, *ep, **fp, *id = NULL, *la = NULL,
                          *lp = NULL, *ra = NULL, *rp = NULL, *ta = NULL;
    int d, err, fl, h, i, j, nf, ty, x;
    INODETYPE inode;
    MALLOC_S len, plen;
    struct sctpsin *sp, *np;
    FILE *ss;
    char *vbuf = (char *)NULL;
    size_t vsz = (size_t)0;
    /*
     * Do second time cleanup or first time setup.
     */
    clean_sctp(ctx);
    SCTPsin = (struct sctpsin **)calloc(INOBUCKS, sizeof(struct sctpsin *));
    if (!SCTPsin) {
        if (ctx->err)
            (void)fprintf(ctx->err,
                          "%s: can't allocate %d SCTP hash pointer bytes\n", Pn,
                          (int)(INOBUCKS * sizeof(struct sctpsin *)));
        Error(ctx);
        return;
    }
    /*
     * Open the /proc/net/sctp files, assign a page size buffer to the streams,
     * and read them.  Store SCTP socket info in the SCTPsin[] hash buckets.
     */
    for (i = 0; i < NSCTPPATHS; i++) {
        if (!(ss = open_proc_stream(ctx, SCTPPath[i], "r", &vbuf, &vsz)))
            continue;
        fl = 1;
        while (fgets(buf, sizeof(buf) - 1, ss)) {
            if ((nf = get_fields(ctx, buf, (char *)NULL, &fp, (int *)NULL, 0)) <
                (i ? 9 : 16)) {
                continue;
            }
            if (fl) {

                /*
                 * Check the column labels in the first line.
                 */
                err = 0;
                switch (i) {
                case 0:
                    if (!fp[0] || strcmp(fp[0], "ASSOC") || !fp[6] ||
                        strcmp(fp[6], "ASSOC-ID") || !fp[10] ||
                        strcmp(fp[10], "INODE") || !fp[11] ||
                        strcmp(fp[11], "LPORT") || !fp[12] ||
                        strcmp(fp[12], "RPORT") || !fp[13] ||
                        strcmp(fp[13], "LADDRS") || !fp[14] ||
                        strcmp(fp[14], "<->") || !fp[15] ||
                        strcmp(fp[15], "RADDRS")) {
                        err = 1;
                    }
                    break;
                case 1:
                    if (!fp[0] || strcmp(fp[0], "ENDPT") || !fp[5] ||
                        strcmp(fp[5], "LPORT") || !fp[7] ||
                        strcmp(fp[7], "INODE") || !fp[8] ||
                        strcmp(fp[8], "LADDRS")) {
                        err = 1;
                    }
                }
                if (err) {
                    if (ctx->err && !Fwarn)
                        (void)fprintf(ctx->err,
                                      "%s: WARNING: unsupported format: %s\n",
                                      Pn, SCTPPath[i]);
                    break;
                }
                fl = 0;
                continue;
            }
            /*
             * Assemble the inode number and see if it has already been
             * recorded.
             */
            ep = (char *)NULL;
            j = i ? 7 : 10;
            if (!fp[j] || !*fp[j] ||
                (inode = strtoull(fp[j], &ep, 0)) == ULONG_MAX || !ep || *ep)
                continue;
            sp = HASH_FIND_ELEMENT(SCTPsin, INOHASH, struct sctpsin, inode,
                                   inode);
            /*
             * Set the entry type.
             */
            if (sp)
                ty = (sp->type == i) ? i : 3;
            else
                ty = i;
            /*
             * Allocate space for this line's sctpsin members.
             *
             * The association or endpoint address is in the first field.
             */
            a = NULL;
            if (fp[0] && *fp[0] && (len = strlen(fp[0]))) {
                if (sp && sp->addr) {
                    if (isainb(fp[0], sp->addr)) {
                        plen = strlen(sp->addr);
                        a = (char *)malloc(plen + len + 2);
                        d = 0;
                    } else
                        d = 1;
                } else {
                    plen = (MALLOC_S)0;
                    a = (char *)malloc(len + 1);
                    d = 0;
                }
                if (!d) {
                    if (!a) {
                        if (ctx->err)
                            (void)fprintf(
                                ctx->err,
                                "%s: can't allocate %d SCTP ASSOC bytes: %s\n",
                                Pn, (int)(len + 1), fp[0]);
                        Error(ctx);
                        goto cleanup;
                    }
                    if (plen)
                        (void)snpf(a, plen + len + 2, "%s,%s", sp->addr, fp[0]);
                    else
                        (void)snpf(a, len + 1, "%s", fp[0]);
                }
            }
            /*
             * The association ID is in the seventh field.
             */
            id = NULL;
            if (!i && fp[6] && *fp[6] && (len = strlen(fp[6]))) {
                if (sp && sp->assocID) {
                    if (isainb(fp[6], sp->assocID)) {
                        plen = strlen(sp->assocID);
                        id = (char *)malloc(plen + len + 2);
                        d = 0;
                    } else
                        d = 1;
                } else {
                    plen = (MALLOC_S)0;
                    id = (char *)malloc(len + 1);
                    d = 0;
                }
                if (!d) {
                    if (!id) {
                        if (ctx->err)
                            (void)fprintf(ctx->err,
                                          "%s: can't allocate %d SCTP ASSOC-ID "
                                          "bytes: %s\n",
                                          Pn, (int)(len + 1), fp[6]);
                        Error(ctx);
                        goto cleanup;
                    }
                    if (plen)
                        (void)snpf(id, plen + len + 2, "%s,%s", sp->assocID,
                                   fp[6]);
                    else
                        (void)snpf(id, len + 1, "%s", fp[6]);
                }
            }
            /*
             * The field number for the local port depends on the entry type.
             */
            j = i ? 5 : 11;
            lp = NULL;
            if (fp[j] && *fp[j] && (len = strlen(fp[j]))) {
                if (sp && sp->lport) {
                    if (isainb(fp[j], sp->lport)) {
                        plen = strlen(sp->lport);
                        lp = (char *)malloc(plen + len + 2);
                        d = 0;
                    } else
                        d = 1;
                } else {
                    plen = (MALLOC_S)0;
                    lp = (char *)malloc(len + 1);
                    d = 0;
                }
                if (!d) {
                    if (!lp) {
                        if (ctx->err)
                            (void)fprintf(
                                ctx->err,
                                "%s: can't allocate %d SCTP LPORT bytes: %s\n",
                                Pn, (int)(len + 1), fp[j]);
                        Error(ctx);
                        goto cleanup;
                    }
                    if (plen)
                        (void)snpf(lp, plen + len + 2, "%s,%s", sp->lport,
                                   fp[j]);
                    else
                        (void)snpf(lp, len + 1, "%s", fp[j]);
                }
            }
            /*
             * The field number for the remote port depends on the entry type.
             */
            rp = NULL;
            if (!i && fp[12] && *fp[12] && (len = strlen(fp[12]))) {
                if (sp && sp->rport) {
                    if (isainb(fp[12], sp->rport)) {
                        plen = strlen(sp->rport);
                        rp = (char *)malloc(plen + len + 2);
                        d = 0;
                    } else
                        d = 1;
                } else {
                    plen = (MALLOC_S)0;
                    rp = (char *)malloc(len + 1);
                    d = 0;
                }
                if (!d) {
                    if (!rp) {
                        if (ctx->err)
                            (void)fprintf(
                                ctx->err,
                                "%s: can't allocate %d SCTP RPORT bytes: %s\n",
                                Pn, (int)(len + 1), fp[12]);
                        Error(ctx);
                        goto cleanup;
                    }
                    if (plen)
                        (void)snpf(rp, plen + len + 2, "%s,%s", sp->rport,
                                   fp[12]);
                    else
                        (void)snpf(rp, len + 1, "%s", fp[12]);
                }
            }
            /*
             * The local addresses begin in a field whose number depends on
             * the entry type.
             */
            j = i ? 8 : 13;
            la = NULL;
            x = 0;
            if (fp[j] && *fp[j] && (len = strlen(fp[j]))) {
                if (!(ta = get_sctpaddrs(fp, j, nf, &x))) {
                    if (ctx->err)
                        (void)fprintf(
                            ctx->err,
                            "%s: can't allocate %d SCTP LADDRS bytes\n", Pn,
                            (int)len);
                    Error(ctx);
                    goto cleanup;
                }
                if (sp && sp->laddrs) {
                    if (isainb(ta, sp->laddrs)) {
                        len = strlen(ta);
                        plen = strlen(sp->laddrs);
                        if (!(la = (char *)malloc(plen + len + 2))) {
                            if (ctx->err)
                                (void)fprintf(ctx->err,
                                              "%s: can't reallocate %d SCTP "
                                              "LADDRS bytes\n",
                                              Pn, (int)len);
                            Error(ctx);
                            goto cleanup;
                        }
                        (void)snpf(la, plen + len + 2, "%s,%s", sp->laddrs, ta);
                    }
                    CLEAN(ta);
                } else {
                    la = ta;
                    ta = NULL;
                }
            }
            /*
             * The remote addresses begin after the local addresses, but only
             * for the ASSOC type.
             */
            ra = NULL;
            if (!i && x && fp[x + 1] && *fp[x + 1] &&
                (len = strlen(fp[x + 1]))) {
                if (!(ta = get_sctpaddrs(fp, x + 1, nf, &x))) {
                    if (ctx->err)
                        (void)fprintf(
                            ctx->err,
                            "%s: can't allocate %d SCTP RADDRS bytes\n", Pn,
                            (int)len);
                    Error(ctx);
                    goto cleanup;
                }
                if (sp && sp->raddrs) {
                    if (isainb(ta, sp->raddrs)) {
                        len = strlen(ta);
                        plen = strlen(sp->raddrs);
                        if (!(ra = (char *)malloc(plen + len + 2))) {
                            if (ctx->err)
                                (void)fprintf(ctx->err,
                                              "%s: can't reallocate %d SCTP "
                                              "RADDRS bytes\n",
                                              Pn, (int)len);
                            Error(ctx);
                            goto cleanup;
                        }
                        (void)snpf(ra, plen + len + 2, "%s,%s", sp->raddrs, ta);
                    }
                    CLEAN(ta);
                } else {
                    ra = ta;
                    ta = NULL;
                }
            }
            /*
             * If no matching sctpsin entry was found for this inode, allocate
             * space for a new sctpsin entry, fill it, and link it to its hash
             * bucket.  Update a matching entry.
             */
            if (!sp) {
                if (!(sp = (struct sctpsin *)malloc(sizeof(struct sctpsin)))) {
                    if (ctx->err)
                        (void)fprintf(
                            ctx->err,
                            "%s: can't allocate %d byte sctpsin structure\n",
                            Pn, (int)sizeof(struct sctpsin));
                    Error(ctx);
                    goto cleanup;
                }
                sp->inode = inode;
                HASH_INSERT_ELEMENT(SCTPsin, INOHASH, sp, inode);
            }
            if (a) {
                CLEAN(sp->addr);
                sp->addr = a;
                a = NULL;
            }
            if (id) {
                CLEAN(sp->assocID);
                sp->assocID = id;
                id = NULL;
            }
            if (lp) {
                CLEAN(sp->lport);
                sp->lport = lp;
                lp = NULL;
            }
            if (rp) {
                CLEAN(sp->rport);
                sp->rport = rp;
                rp = NULL;
            }
            if (la) {
                CLEAN(sp->laddrs);
                sp->laddrs = la;
                la = NULL;
            }
            if (ra) {
                CLEAN(sp->raddrs);
                sp->raddrs = ra;
                ra = NULL;
            }
            sp->type = ty;
        }
        (void)fclose(ss);
        ss = NULL;
    }
cleanup:
    if (ss)
        (void)fclose(ss);
    CLEAN(a);
    CLEAN(id);
    CLEAN(lp);
    CLEAN(rp);
    CLEAN(ta);
    CLEAN(la);
    CLEAN(ra);
}

static char *get_sctpaddrs(char **fp, /* field pointers */
                           int i,     /* first address field index in fp */
                           int nf,    /* number of fields */
                           int *x)    /* index of first "<->" field entry */
{
    MALLOC_S al = (MALLOC_S)0;
    char *cp = (char *)NULL;
    char *temp;
    MALLOC_S tl;

    *x = 0;
    do {
        if ((i >= nf) || !fp[i] || !*fp[i] || !(tl = strlen(fp[i])))
            break;
        if (!strcmp(fp[i], "<->")) {
            *x = i;
            break;
        }
        if (!strchr(fp[i], (int)'.') && !strchr(fp[i], (int)':'))
            break;

        if (cp)
            temp = (char *)realloc((MALLOC_P *)cp, al + tl + 1);
        else
            temp = (char *)malloc(al + tl + 1);
        if (!temp) {
            CLEAN(cp);
            break;
        }
        cp = temp;

        if (al)
            *(cp + al - 1) = ',';
        (void)strncpy(al ? (cp + al) : cp, fp[i], tl);
        al += (tl + 1);
        *(cp + al - 1) = '\0';
    } while (++i < nf);
    return (cp);
}

/*
 * clean_tcpudp() - cleanup tcpudp net info
 */
void clean_tcpudp(struct lsof_context *ctx, int free_array) {
    int h;
    struct tcp_udp *np, *tp;
#if defined(HASEPTOPTS)
    pxinfo_t *pp, *pnp;
#endif /* defined(HASEPTOPTS) */
    if (TcpUdp) {
        for (h = 0; h < TcpUdp_bucks; h++) {
            for (tp = TcpUdp[h]; tp; tp = np) {
                np = tp->next;

#if defined(HASEPTOPTS)
                for (pp = tp->pxinfo; pp; pp = pnp) {
                    pnp = pp->next;
                    (void)free((FREE_P *)pp);
                }
#endif /* defined(HASEPTOPTS) */

                (void)free((FREE_P *)tp);
            }
            TcpUdp[h] = (struct tcp_udp *)NULL;
        }
#if defined(HASEPTOPTS)
        if (FeptE)
            for (h = 0; h < IPCBUCKS; h++)
                TcpUdpIPC[h] = (struct tcp_udp *)NULL;
#endif /* defined(HASEPTOPTS) */
        if (free_array) {
            CLEAN(TcpUdp);
            CLEAN(TcpUdpIPC);
        }
    }
}

/*
 * get_tcpudp() - get IPv4 TCP, UDP or UDPLITE net info
 */
static void get_tcpudp(struct lsof_context *ctx,
                       char *p, /* /proc/net/{tcp,udp} path */
                       int pr,  /* protocol: 0 = TCP, 1 = UDP,
                                 *           2 = UDPLITE */
                       int clr) /* 1 == clear the table */
{
    char buf[MAXPATHLEN], *ep, **fp;
    unsigned long faddr, fport, laddr, lport, rxq, state, txq;
    FILE *fs;
    int h, nf;
    INODETYPE inode;
    struct tcp_udp *np, *tp;
    char *vbuf = (char *)NULL;
    size_t vsz = (size_t)0;

#if defined(HASEPTOPTS)
    pxinfo_t *pp, *pnp;
#endif /* defined(HASEPTOPTS) */

    /*
     * Delete previous table contents.
     */
    if (TcpUdp) {
        if (clr) {
            clean_tcpudp(ctx, 0);
        }
        /*
         * If no hash buckets have been allocated, do so now.
         */
    } else {

        /*
         * Open the /proc/net/sockstat file and establish the hash bucket
         * count from its "sockets: used" line.
         */
        TcpUdp_bucks = INOBUCKS;
        if ((fs = fopen(SockStatPath, "r"))) {
            while (fgets(buf, sizeof(buf) - 1, fs)) {
                if (get_fields(ctx, buf, (char *)NULL, &fp, (int *)NULL, 0) !=
                    3)
                    continue;
                if (!fp[0] || strcmp(fp[0], "sockets:") || !fp[1] ||
                    strcmp(fp[1], "used") || !fp[2] || !*fp[2])
                    continue;
                if ((h = atoi(fp[2])) < 1)
                    h = INOBUCKS;
                while (TcpUdp_bucks < h)
                    TcpUdp_bucks *= 2;
                break;
            }
            (void)fclose(fs);
            fs = NULL;
        }
        if (!(TcpUdp = (struct tcp_udp **)calloc(TcpUdp_bucks,
                                                 sizeof(struct tcp_udp *)))) {
            if (ctx->err)
                (void)fprintf(
                    ctx->err,
                    "%s: can't allocate %d bytes for TCP&UDP hash buckets\n",
                    Pn, (int)(TcpUdp_bucks * sizeof(struct tcp_udp *)));
            Error(ctx);
            goto cleanup;
        }
#if defined(HASEPTOPTS)
        if (FeptE && (!(TcpUdpIPC = (struct tcp_udp **)calloc(
                            IPCBUCKS, sizeof(struct tcp_udp *))))) {
            if (ctx->err)
                (void)fprintf(
                    ctx->err,
                    "%s: can't allocate %d bytes for TCP&UDP local IPC "
                    "hash buckets\n",
                    Pn, (int)(IPCBUCKS * sizeof(struct tcp_udp *)));
            Error(ctx);
            goto cleanup;
        }
#endif /* defined(HASEPTOPTS) */
    }
    /*
     * Open the /proc/net file, assign a page size buffer to the stream, and
     * read it.
     */
    if (!(fs = open_proc_stream(ctx, p, "r", &vbuf, &vsz))) {
        goto cleanup;
    }
    nf = 12;
    while (fgets(buf, sizeof(buf) - 1, fs)) {
        if (get_fields(ctx, buf, (nf == 12) ? (char *)NULL : ":", &fp,
                       (int *)NULL, 0) < nf)
            continue;
        if (nf == 12) {
            if (!fp[1] || strcmp(fp[1], "local_address") || !fp[2] ||
                strcmp(fp[2], "rem_address") || !fp[3] || strcmp(fp[3], "st") ||
                !fp[4] || strcmp(fp[4], "tx_queue") || !fp[5] ||
                strcmp(fp[5], "rx_queue") || !fp[11] ||
                strcmp(fp[11], "inode")) {
                if (ctx->err && !Fwarn) {
                    (void)fprintf(ctx->err,
                                  "%s: WARNING: unsupported format: %s\n", Pn,
                                  p);
                }
                break;
            }
            nf = 14;
            continue;
        }
        /*
         * Get the local and remote addresses.
         */
        ep = (char *)NULL;
        if (!fp[1] || !*fp[1] ||
            (laddr = strtoul(fp[1], &ep, 16)) == ULONG_MAX || !ep || *ep)
            continue;
        ep = (char *)NULL;
        if (!fp[2] || !*fp[2] ||
            (lport = strtoul(fp[2], &ep, 16)) == ULONG_MAX || !ep || *ep)
            continue;
        ep = (char *)NULL;
        if (!fp[3] || !*fp[3] ||
            (faddr = strtoul(fp[3], &ep, 16)) == ULONG_MAX || !ep || *ep)
            continue;
        ep = (char *)NULL;
        if (!fp[4] || !*fp[4] ||
            (fport = strtoul(fp[4], &ep, 16)) == ULONG_MAX || !ep || *ep)
            continue;
        /*
         * Get the state and queue sizes.
         */
        ep = (char *)NULL;
        if (!fp[5] || !*fp[5] ||
            (state = strtoul(fp[5], &ep, 16)) == ULONG_MAX || !ep || *ep)
            continue;
        ep = (char *)NULL;
        if (!fp[6] || !*fp[6] || (txq = strtoul(fp[6], &ep, 16)) == ULONG_MAX ||
            !ep || *ep)
            continue;
        ep = (char *)NULL;
        if (!fp[7] || !*fp[7] || (rxq = strtoul(fp[7], &ep, 16)) == ULONG_MAX ||
            !ep || *ep)
            continue;
        /*
         * Get the inode and use it for hashing and searching.
         */
        ep = (char *)NULL;
        if (!fp[13] || !*fp[13] ||
            (inode = strtoull(fp[13], &ep, 0)) == ULONG_MAX || !ep || *ep)
            continue;
        if (HASH_FIND_ELEMENT(TcpUdp, TCPUDPHASH, struct tcp_udp, inode, inode))
            continue;
        /*
         * Create a new entry and link it to its hash bucket.
         */
        if (!(tp = (struct tcp_udp *)malloc(sizeof(struct tcp_udp)))) {
            if (ctx->err)
                (void)fprintf(
                    ctx->err,
                    "%s: can't allocate %d bytes for tcp_udp struct\n", Pn,
                    (int)sizeof(struct tcp_udp));
            Error(ctx);
            goto cleanup;
        }
        tp->inode = inode;
        tp->faddr = faddr;
        tp->fport = (int)(fport & 0xffff);
        tp->laddr = laddr;
        tp->lport = (int)(lport & 0xffff);
        tp->txq = txq;
        tp->rxq = rxq;
        tp->proto = pr;
        tp->state = (int)state;
        HASH_INSERT_ELEMENT(TcpUdp, TCPUDPHASH, tp, inode);
#if defined(HASEPTOPTS)
        tp->pxinfo = (pxinfo_t *)NULL;
        if (FeptE) {
            tp->ipc_peer = (struct tcp_udp *)NULL;
            if (tp->state == TCP_ESTABLISHED) {
                int i = TCPUDP_IPC_HASH(tp);
                tp->ipc_next = TcpUdpIPC[i];
                TcpUdpIPC[i] = tp;
            }
        }
#endif /* defined(HASEPTOPTS) */
    }

#if defined(HASEPTOPTS)
    /*
     * If endpoint info has been requested, link INET socket peer info.
     */
    if (FeptE)
        get_netpeeri(ctx);
#endif /* defined(HASEPTOPTS) */

cleanup:
    (void)fclose(fs);
    CLEAN(vbuf);
}

#if defined(HASIPv6)
/*
 * clean_raw6() - cleanup raw6 net info
 */
void clean_raw6(struct lsof_context *ctx) {
    struct rawsin *np, *rp;
    int h;
    if (Rawsin6) {
        for (h = 0; h < INOBUCKS; h++) {
            for (rp = Rawsin6[h]; rp; rp = np) {
                np = rp->next;
                CLEAN(rp->la);
                CLEAN(rp->ra);
                CLEAN(rp->sp);
                (void)free((FREE_P *)rp);
            }
            Rawsin6[h] = (struct rawsin *)NULL;
        }
        CLEAN(Rawsin6);
    }
}

/*
 * get_raw6() - get /proc/net/raw6 info
 */
static void get_raw6(struct lsof_context *ctx, char *p) /* /proc/net/raw path */
{
    char buf[MAXPATHLEN], *ep, **fp, *la = NULL, *ra = NULL, *sp = NULL;
    int h;
    INODETYPE inode;
    int nf = 12;
    struct rawsin *np, *rp;
    MALLOC_S lal, ral, spl;
    char *vbuf = (char *)NULL;
    size_t vsz = (size_t)0;
    FILE *xs;
    /*
     * Do second time cleanup or first time setup.
     */
    clean_raw6(ctx);
    Rawsin6 = (struct rawsin **)calloc(INOBUCKS, sizeof(struct rawsin *));
    if (!Rawsin6) {
        if (ctx->err)
            (void)fprintf(ctx->err,
                          "%s: can't allocate %d raw6 hash pointer bytes\n", Pn,
                          (int)(INOBUCKS * sizeof(struct rawsin *)));
        Error(ctx);
        return;
    }
    /*
     * Open the /proc/net/raw6 file, assign a page size buffer to the stream,
     * and read it.  Store raw6 socket info in the Rawsin6[] hash buckets.
     */
    if (!(xs = open_proc_stream(ctx, p, "r", &vbuf, &vsz)))
        return;
    while (fgets(buf, sizeof(buf) - 1, xs)) {
        if (get_fields(ctx, buf, (char *)NULL, &fp, (int *)NULL, 0) < nf)
            continue;
        if (nf == 12) {

            /*
             * Check the column labels in the first line.
             */
            if (!fp[1] || strcmp(fp[1], "local_address") || !fp[2] ||
                strcmp(fp[2], "remote_address") || !fp[3] ||
                strcmp(fp[3], "st") || !fp[11] || strcmp(fp[11], "inode")) {
                if (ctx->err && !Fwarn) {
                    (void)fprintf(ctx->err,
                                  "%s: WARNING: unsupported format: %s\n", Pn,
                                  p);
                }
                break;
            }
            nf = 10;
            continue;
        }
        /*
         * Assemble the inode number and see if the inode is already
         * recorded.
         */
        ep = (char *)NULL;
        if (!fp[9] || !*fp[9] ||
            (inode = strtoull(fp[9], &ep, 0)) == ULONG_MAX || !ep || *ep)
            continue;
        /* Skip if already exists in hash table */
        if (HASH_FIND_ELEMENT(Rawsin6, INOHASH, struct rawsin, inode, inode))
            continue;
        /*
         * Save the local address, remote address, and state.
         */
        if (!fp[1] || !*fp[1] || (lal = strlen(fp[1])) < 1) {
            la = (char *)NULL;
            lal = (MALLOC_S)0;
        } else {
            if (!(la = (char *)malloc(lal + 1))) {
                if (ctx->err)
                    (void)fprintf(
                        ctx->err,
                        "%s: can't allocate %d local raw6 address bytes: %s\n",
                        Pn, (int)(lal + 1), fp[1]);
                Error(ctx);
                goto cleanup;
            }
            (void)snpf(la, lal + 1, "%s", fp[1]);
        }
        if (!fp[2] || !*fp[2] || (ral = strlen(fp[2])) < 1) {
            ra = (char *)NULL;
            ral = (MALLOC_S)0;
        } else {
            if (!(ra = (char *)malloc(ral + 1))) {
                if (ctx->err)
                    (void)fprintf(
                        ctx->err,
                        "%s: can't allocate %d remote raw6 address bytes: %s\n",
                        Pn, (int)(ral + 1), fp[2]);
                Error(ctx);
                goto cleanup;
            }
            (void)snpf(ra, ral + 1, "%s", fp[2]);
        }
        if (!fp[3] || !*fp[3] || (spl = strlen(fp[3])) < 1) {
            sp = (char *)NULL;
            spl = (MALLOC_S)0;
        } else {
            if (!(sp = (char *)malloc(spl + 1))) {
                if (ctx->err)
                    (void)fprintf(
                        ctx->err,
                        "%s: can't allocate %d remote raw6 state bytes: %s\n",
                        Pn, (int)(spl + 1), fp[2]);
                Error(ctx);
                goto cleanup;
            }
            (void)snpf(sp, spl + 1, "%s", fp[3]);
        }
        /*
         * Allocate space for an rawsin entry, fill it, and link it to its
         * hash bucket.
         */
        if (!(rp = (struct rawsin *)malloc(sizeof(struct rawsin)))) {
            if (ctx->err)
                (void)fprintf(
                    ctx->err,
                    "%s: can't allocate %d byte rawsin structure for IPv6\n",
                    Pn, (int)sizeof(struct rawsin));
            Error(ctx);
            goto cleanup;
        }
        rp->inode = inode;
        rp->la = la;
        la = NULL;
        rp->lal = lal;
        rp->ra = ra;
        ra = NULL;
        rp->ral = ral;
        rp->sp = sp;
        sp = NULL;
        rp->spl = spl;
        HASH_INSERT_ELEMENT(Rawsin6, INOHASH, rp, inode);
    }
cleanup:
    (void)fclose(xs);
    CLEAN(vbuf);
    CLEAN(la);
    CLEAN(ra);
    CLEAN(sp);
}

/*
 * clean_tcpudp6() - cleanup tcpudp6 net info
 */
void clean_tcpudp6(struct lsof_context *ctx, int free_array) {
    struct tcp_udp6 *np6, *tp6;
    int h;
#    if defined(HASEPTOPTS)
    pxinfo_t *pp, *pnp;
#    endif /* defined(HASEPTOPTS) */
    if (TcpUdp6) {
        for (h = 0; h < TcpUdp6_bucks; h++) {
            for (tp6 = TcpUdp6[h]; tp6; tp6 = np6) {
                np6 = tp6->next;

#    if defined(HASEPTOPTS)
                for (pp = tp6->pxinfo; pp; pp = pnp) {
                    pnp = pp->next;
                    (void)free((FREE_P *)pp);
                }
#    endif /* defined(HASEPTOPTS) */

                (void)free((FREE_P *)tp6);
            }
            TcpUdp6[h] = (struct tcp_udp6 *)NULL;
        }
#    if defined(HASEPTOPTS)
        if (FeptE)
            for (h = 0; h < IPCBUCKS; h++)
                TcpUdp6IPC[h] = (struct tcp_udp6 *)NULL;
#    endif /* defined(HASEPTOPTS) */

        if (free_array) {
            CLEAN(TcpUdp6);
            CLEAN(TcpUdp6IPC);
        }
    }
}

/*
 * get_tcpudp6() - get IPv6 TCP, UDP or UDPLITE net info
 */
static void get_tcpudp6(struct lsof_context *ctx,
                        char *p, /* /proc/net/{tcp,udp} path */
                        int pr,  /* protocol: 0 = TCP, 1 = UDP */
                        int clr) /* 1 == clear the table */
{
    char buf[MAXPATHLEN], *ep, **fp;
    struct in6_addr faddr, laddr;
    unsigned long fport, lport, rxq, state, txq;
    FILE *fs;
    int h, i, nf;
    INODETYPE inode;
    struct tcp_udp6 *np6, *tp6;
    char *vbuf = (char *)NULL;
    size_t vsz = (size_t)0;

#    if defined(HASEPTOPTS)
    pxinfo_t *pp, *pnp;
#    endif /* defined(HASEPTOPTS) */

    /*
     * Delete previous table contents.  Allocate a table for the first time.
     */
    if (TcpUdp6) {
        if (clr) {
            clean_tcpudp6(ctx, 0);
        }
    } else {

        /*
         * Open the /proc/net/sockstat6 file and establish the hash bucket
         * count from its "TCP6: inuse" and "UDP6: inuse" lines.
         */
        TcpUdp6_bucks = INOBUCKS;
        i = nf = 0;
        if ((fs = fopen(SockStatPath6, "r"))) {
            while (fgets(buf, sizeof(buf) - 1, fs)) {
                if (get_fields(ctx, buf, (char *)NULL, &fp, (int *)NULL, 0) !=
                    3)
                    continue;
                if (!fp[0] || !fp[1] || strcmp(fp[1], "inuse") || !fp[2] ||
                    !*fp[2])
                    continue;
                if (!strcmp(fp[0], "TCP6:")) {
                    nf |= 1;
                    if ((h = atoi(fp[2])) < 1)
                        h = INOBUCKS;
                    i += h;
                } else if (!strcmp(fp[0], "UDP6:")) {
                    nf |= 2;
                    if ((h = atoi(fp[2])) < 1)
                        h = INOBUCKS;
                    i += h;
                } else
                    continue;
                if (nf == 3) {
                    while (TcpUdp6_bucks < i)
                        TcpUdp6_bucks *= 2;
                    break;
                }
            }
            (void)fclose(fs);
            fs = NULL;
        }
        if (!(TcpUdp6 = (struct tcp_udp6 **)calloc(
                  TcpUdp6_bucks, sizeof(struct tcp_udp6 *)))) {
            if (ctx->err)
                (void)fprintf(
                    ctx->err,
                    "%s: can't allocate %d bytes for TCP6&UDP6 hash buckets\n",
                    Pn, (int)(TcpUdp6_bucks * sizeof(struct tcp_udp6 *)));
            Error(ctx);
            goto cleanup;
        }
#    if defined(HASEPTOPTS)
        if (FeptE && (!(TcpUdp6IPC = (struct tcp_udp6 **)calloc(
                            IPCBUCKS, sizeof(struct tcp_udp6 *))))) {
            if (ctx->err)
                (void)fprintf(
                    ctx->err,
                    "%s: can't allocate %d bytes for TCP6&UDP6 local IPC "
                    "hash buckets\n",
                    Pn, (int)(IPCBUCKS * sizeof(struct tcp_udp6 *)));
            Error(ctx);
            goto cleanup;
        }
#    endif /* defined(HASEPTOPTS) */
    }
    /*
     * Open the /proc/net file, assign a page size buffer to the stream,
     * and read it.
     */
    if (!(fs = open_proc_stream(ctx, p, "r", &vbuf, &vsz))) {
        goto cleanup;
    }
    nf = 12;
    while (fgets(buf, sizeof(buf) - 1, fs)) {
        if (get_fields(ctx, buf, (nf == 12) ? (char *)NULL : ":", &fp,
                       (int *)NULL, 0) < nf)
            continue;
        if (nf == 12) {
            if (!fp[1] || strcmp(fp[1], "local_address") || !fp[2] ||
                strcmp(fp[2], "remote_address") || !fp[3] ||
                strcmp(fp[3], "st") || !fp[4] || strcmp(fp[4], "tx_queue") ||
                !fp[5] || strcmp(fp[5], "rx_queue") || !fp[11] ||
                strcmp(fp[11], "inode")) {
                if (ctx->err && !Fwarn) {
                    (void)fprintf(ctx->err,
                                  "%s: WARNING: unsupported format: %s\n", Pn,
                                  p);
                }
                break;
            }
            nf = 14;
            continue;
        }
        /*
         * Get the local and remote addresses.
         */
        if (!fp[1] || !*fp[1] || hex_ipv6_to_in6(fp[1], &laddr))
            continue;
        ep = (char *)NULL;
        if (!fp[2] || !*fp[2] ||
            (lport = strtoul(fp[2], &ep, 16)) == ULONG_MAX || !ep || *ep)
            continue;
        if (!fp[3] || !*fp[3] || hex_ipv6_to_in6(fp[3], &faddr))
            continue;
        ep = (char *)NULL;
        if (!fp[4] || !*fp[4] ||
            (fport = strtoul(fp[4], &ep, 16)) == ULONG_MAX || !ep || *ep)
            continue;
        /*
         * Get the state and queue sizes.
         */
        ep = (char *)NULL;
        if (!fp[5] || !*fp[5] ||
            (state = strtoul(fp[5], &ep, 16)) == ULONG_MAX || !ep || *ep)
            continue;
        ep = (char *)NULL;
        if (!fp[6] || !*fp[6] || (txq = strtoul(fp[6], &ep, 16)) == ULONG_MAX ||
            !ep || *ep)
            continue;
        ep = (char *)NULL;
        if (!fp[7] || !*fp[7] || (rxq = strtoul(fp[7], &ep, 16)) == ULONG_MAX ||
            !ep || *ep)
            continue;
        /*
         * Get the inode and use it for hashing and searching.
         */
        ep = (char *)NULL;
        if (!fp[13] || !*fp[13] ||
            (inode = strtoull(fp[13], &ep, 0)) == ULONG_MAX || !ep || *ep)
            continue;
        if (HASH_FIND_ELEMENT(TcpUdp6, TCPUDP6HASH, struct tcp_udp6, inode,
                              inode))
            continue;
        /*
         * Create a new entry and link it to its hash bucket.
         */
        if (!(tp6 = (struct tcp_udp6 *)malloc(sizeof(struct tcp_udp6)))) {
            if (ctx->err)
                (void)fprintf(
                    ctx->err,
                    "%s: can't allocate %d bytes for tcp_udp6 struct\n", Pn,
                    (int)sizeof(struct tcp_udp6));
            Error(ctx);
            goto cleanup;
        }
        tp6->inode = inode;
        tp6->faddr = faddr;
        tp6->fport = (int)(fport & 0xffff);
        tp6->laddr = laddr;
        tp6->lport = (int)(lport & 0xffff);
        tp6->txq = txq;
        tp6->rxq = rxq;
        tp6->proto = pr;
        tp6->state = (int)state;
        HASH_INSERT_ELEMENT(TcpUdp6, TCPUDP6HASH, tp6, inode);
#    if defined(HASEPTOPTS)
        tp6->pxinfo = (pxinfo_t *)NULL;
        if (FeptE) {
            tp6->ipc_peer = (struct tcp_udp6 *)NULL;
            if (tp6->state == TCP_ESTABLISHED) {
                int i = TCPUDP6_IPC_HASH(tp6);
                tp6->ipc_next = TcpUdp6IPC[i];
                TcpUdp6IPC[i] = tp6;
            }
        }
#    endif /* defined(HASEPTOPTS) */
    }
#    if defined(HASEPTOPTS)
    /*
     * If endpoint info has been requested, link INET6 socket peer info.
     */
    if (FeptE)
        get_net6peeri(ctx);
#    endif /* defined(HASEPTOPTS) */

cleanup:
    if (fs)
        (void)fclose(fs);
    CLEAN(vbuf);
}
#endif /* defined(HASIPv6) */

/*
 * clean_unix() - cleanup raw6 net info
 */
void clean_unix(struct lsof_context *ctx) {
    int h;
    uxsin_t *np, *up;
#if defined(HASEPTOPTS) && defined(HASUXSOCKEPT)
    pxinfo_t *pp, *pnp;
#endif /* defined(HASEPTOPTS) && defined(HASUXSOCKEPT) */
    if (Uxsin) {
        for (h = 0; h < INOBUCKS; h++) {
            for (up = Uxsin[h]; up; up = np) {
                np = up->next;

#if defined(HASEPTOPTS) && defined(HASUXSOCKEPT)
                for (pp = up->pxinfo; pp; pp = pnp) {
                    pnp = pp->next;
                    (void)free((FREE_P *)pp);
                }
#endif /* defined(HASEPTOPTS) && defined(HASUXSOCKEPT) */

                if (up->path)
                    (void)free((FREE_P *)up->path);
                if (up->pcb)
                    (void)free((FREE_P *)up->pcb);
                (void)free((FREE_P *)up);
            }
            Uxsin[h] = (uxsin_t *)NULL;
        }
        CLEAN(Uxsin);
    }
}

/*
 * get_unix() - get UNIX net info
 */
static void get_unix(struct lsof_context *ctx,
                     char *p) /* /proc/net/unix path */
{
    char buf[MAXPATHLEN], *ep, **fp, *path = NULL, *pcb = NULL;
    int fl = 1; /* First line */
    int h, nf;
    INODETYPE inode;
    MALLOC_S len;
    uxsin_t *np, *up;
    FILE *us;
    uint32_t ty;
    char *vbuf = (char *)NULL;
    size_t vsz = (size_t)0;

#if defined(HASEPTOPTS) && defined(HASUXSOCKEPT)
    pxinfo_t *pp, *pnp;
#endif /* defined(HASEPTOPTS) && defined(HASUXSOCKEPT) */

    /*
     * Do second time cleanup or first time setup.
     */
    clean_unix(ctx);
    Uxsin = (uxsin_t **)calloc(INOBUCKS, sizeof(uxsin_t *));
    if (!Uxsin) {
        if (ctx->err)
            (void)fprintf(ctx->err,
                          "%s: can't allocate %d bytes for Unix socket info\n",
                          Pn, (int)(INOBUCKS * sizeof(uxsin_t *)));
        Error(ctx);
        return;
    }
    /*
     * Open the /proc/net/unix file, assign a page size buffer to the stream,
     * read the file's contents, and add them to the Uxsin hash buckets.
     */
    if (!(us = open_proc_stream(ctx, p, "r", &vbuf, &vsz))) {
        goto cleanup;
    }
    while (fgets(buf, sizeof(buf) - 1, us)) {
        if ((nf = get_fields(ctx, buf, ":", &fp, (int *)NULL, 0)) < 7)
            continue;
        if (fl) {

            /*
             * Check the first line for header words.
             */
            if (!fp[0] || strcmp(fp[0], "Num") || !fp[1] ||
                strcmp(fp[1], "RefCount") || !fp[2] ||
                strcmp(fp[2], "Protocol") || !fp[3] || strcmp(fp[3], "Flags") ||
                !fp[4] || strcmp(fp[4], "Type") || !fp[5] ||
                strcmp(fp[5], "St") || !fp[6] || strcmp(fp[6], "Inode") ||
                nf < 8 || !fp[7] || strcmp(fp[7], "Path")) {
                if (ctx->err && !Fwarn) {
                    (void)fprintf(ctx->err,
                                  "%s: WARNING: unsupported format: %s\n", Pn,
                                  p);
                }
                break;
            }
            fl = 0;
            continue;
        }
        /*
         * Assemble PCB address, inode number, and path name.  If this
         * inode is already represented in Uxsin, skip it.
         */
        ep = (char *)NULL;
        if (!fp[6] || !*fp[6] ||
            (inode = strtoull(fp[6], &ep, 0)) == ULONG_MAX || !ep || *ep)
            continue;
        /* Skip if already exists in hash table */
        if (HASH_FIND_ELEMENT(Uxsin, INOHASH, uxsin_t, inode, inode))
            continue;
        if (!fp[0] || !*fp[0])
            pcb = (char *)NULL;
        else {
            len = strlen(fp[0]) + 2;
            if (!(pcb = (char *)malloc(len + 1))) {
                if (ctx->err)
                    (void)fprintf(
                        ctx->err,
                        "%s: can't allocate %d bytes for UNIX PCB: %s\n", Pn,
                        (int)(len + 1), fp[0]);
                Error(ctx);
                goto cleanup;
            }
            (void)snpf(pcb, len + 1, "0x%s", fp[0]);
        }
        if (nf >= 8 && fp[7] && *fp[7] && (len = strlen(fp[7]))) {
            if (!(path = (char *)malloc(len + 1))) {
                if (ctx->err)
                    (void)fprintf(
                        ctx->err,
                        "%s: can't allocate %d bytes for UNIX path \"%s\"\n",
                        Pn, (int)(len + 1), fp[7]);
                Error(ctx);
                goto cleanup;
            }
            (void)snpf(path, len + 1, "%s", fp[7]);
        } else
            path = (char *)NULL;
        /*
         * Assemble socket type.
         */
        ep = (char *)NULL;
        if (!fp[4] || !*fp[4] ||
            (ty = (uint32_t)strtoul(fp[4], &ep, 16)) == (uint32_t)UINT32_MAX ||
            !ep || *ep) {
            ty = (uint32_t)UINT_MAX;
        }
        /*
         * Record socket state.
         */
        unsigned long proc_flags = 0UL;
        ep = (char *)NULL;
        if (fp[3] && *fp[3] &&
            (proc_flags = strtoul(fp[3], &ep, 16)) == ULONG_MAX)
            proc_flags = 0UL;

        unsigned long proc_st = 0UL;
        ep = (char *)NULL;
        if (fp[5] && *fp[5] &&
            ((proc_st = strtoul(fp[5], &ep, 16)) == ULONG_MAX))
            proc_st = 0UL;

        /*
         * Allocate and fill a Unix socket info structure; link it to its
         * hash bucket.
         */
        if (!(up = (uxsin_t *)malloc(sizeof(uxsin_t)))) {
            if (ctx->err)
                (void)fprintf(ctx->err,
                              "%s: can't allocate %d bytes for uxsin struct\n",
                              Pn, (int)sizeof(uxsin_t));
            Error(ctx);
            goto cleanup;
        }
        up->inode = inode;
        up->next = (uxsin_t *)NULL;
        up->pcb = pcb;
        pcb = NULL;
        up->sb_def = 0;
        up->ty = ty;
        up->opt = (unsigned int)proc_flags;
        up->ss = (unsigned int)proc_st;
        if ((up->path = path) && (*path == '/')) {

            /*
             * If an absolute path (i.e., one that begins with a '/') exists
             * for the line, attempt to stat(2) it and save the device and
             * node numbers reported in the stat buffer.
             */
            struct stat sb;
            int sr;

            if (HasNFS)
                sr = statsafely(ctx, path, &sb);
            else
                sr = stat(path, &sb);
            if (sr == 0 && ((sb.st_mode & S_IFMT) == S_IFSOCK)) {
                up->sb_def = 1;
                up->sb_dev = sb.st_dev;
                up->sb_ino = (INODETYPE)sb.st_ino;
                up->sb_rdev = sb.st_rdev;
            }
        }
        path = NULL;

#if defined(HASEPTOPTS) && defined(HASUXSOCKEPT)
        /*
         * Clean UNIX socket endpoint values.
         */
        up->icstat = 0;
        up->pxinfo = (pxinfo_t *)NULL;
        up->peer = up->icons = (uxsin_t *)NULL;
#endif /* defined(HASEPTOPTS) && defined(HASUXSOCKEPT) */

        HASH_INSERT_ELEMENT(Uxsin, INOHASH, up, inode);
    }

#if defined(HASEPTOPTS) && defined(HASUXSOCKEPT)
    /*
     * If endpoint info has been requested, get UNIX socket peer info.
     */
    if (FeptE)
        get_uxpeeri(ctx);
#endif /* defined(HASEPTOPTS) && defined(HASUXSOCKEPT) */

cleanup:
    if (us)
        (void)fclose(us);
    CLEAN(vbuf);
    CLEAN(pcb);
    CLEAN(path);
}

#if defined(HASIPv6)
/*
 * hex_ipv6_to_in6() - convert ASCII IPv6 address in /proc/net/{tcp,udp}6 form
 *to an in6_addr
 */
static int hex_ipv6_to_in6(char *as,            /* address source */
                           struct in6_addr *ad) /* address destination */
{
    char buf[9], *ep;
    int i;
    size_t len;
    /*
     * Assemble four uint32_t's from 4 X 8 hex digits into s6_addr32[].
     */
    for (i = 0, len = strlen(as); (i < 4) && (len >= 8);
         as += 8, i++, len -= 8) {
        (void)strncpy(buf, as, 8);
        buf[8] = '\0';
        ep = (char *)NULL;

        errno = 0;
        unsigned long ul_addr = strtoul(buf, &ep, 16);
        if (!ep || *ep)
            break;
        else if (ul_addr == ULONG_MAX && errno == ERANGE) {
            /* Quoted from strtoul(3)
               ---------------------------------------------------
               The strtoul() function returns either the result of
               the conversion or, if there was a leading minus
               sign, the negation of the result of the conversion
               represented as an unsigned value, unless the
               original (nonnegated) value would overflow; in the
               latter case, strtoul() returns ULONG_MAX and sets
               errno to ERANGE.
               ---------------------------------------------------
               NOTE: even if the value doesn't overflow, a
               negative is not acceptable. */
            break;
        } else if (ul_addr > (unsigned long)UINT32_MAX) {
            /* This will never happen:
               The maximum length of BUF is 8 characters.
               The possible maximum value represented by BUF is
               "FFFFFFFF". This is UINT32_MAX.
               If you agree with what I write here, make a pull
               request for removing this block. */
            break;
        }
        ad->s6_addr32[i] = (uint32_t)ul_addr;
    }
    return ((*as || (i != 4) || len) ? 1 : 0);
}
#endif /* defined(HASIPv6) */

/*
 * isainb(a,b) check if string a is included in b, where b is a comma-separated
 * string
 */
static int isainb(char *a, /*string a */
                  char *b) /* string b */
{
    char *cp, *pp;
    MALLOC_S la, lb, lt;

    if (!a || !b)
        return (1);
    if (!(la = strlen(a)) || !(lb = strlen(b)))
        return (1);
    if (!(cp = strchr(b, (int)','))) {
        if (la != lb)
            return (1);
        return (strcmp(a, b));
    }
    for (pp = b; pp && *pp;) {
        lt = (MALLOC_S)(cp - pp);
        if ((la == lt) && !strncmp(a, pp, lt))
            return (0);
        if (*cp) {
            pp = cp + 1;
            if (!(cp = strchr(pp, (int)',')))
                cp = b + lb;
        } else
            pp = cp;
    }
    return (1);
}

/*
 * print_ax25info() - print AX25 socket info
 */
static void print_ax25info(struct lsof_context *ctx,
                           struct ax25sin *ap) /* AX25 socket info */
{
    char *cp, pbuf[1024];
    int ds;
    MALLOC_S pl = (MALLOC_S)0;

    if (Lf->nma)
        return;
    if (ap->sa) {
        ds = (ap->da && strcmp(ap->da, "*")) ? 1 : 0;
        (void)snpf(&pbuf[pl], sizeof(pbuf) - pl, "%s%s%s ", ap->sa,
                   ds ? "->" : "", ds ? ap->da : "");
        pl = strlen(pbuf);
    }
    if (ap->sqs) {
        (void)snpf(&pbuf[pl], sizeof(pbuf) - pl, "(Sq=%lu ", ap->sq);
        pl = strlen(pbuf);
        cp = "";
    } else
        cp = "(";
    if (ap->rqs) {
        (void)snpf(&pbuf[pl], sizeof(pbuf) - pl, "%sRq=%lu ", cp, ap->rq);
        pl = strlen(pbuf);
        cp = "";
    }
    (void)snpf(&pbuf[pl], sizeof(pbuf) - pl, "%sState=%d", cp, ap->state);
    pl = strlen(pbuf);
    if ((ap->state >= 0) && (ap->state < NAX25ST))
        cp = ax25st[ap->state];
    else
        cp = NULL;
    (void)snpf(&pbuf[pl], sizeof(pbuf) - pl, "%s%s)", cp ? ", " : "",
               cp ? cp : "");
    pl = strlen(pbuf);
    if (!(cp = (char *)malloc(pl + 1))) {
        if (ctx->err)
            (void)fprintf(
                ctx->err,
                "%s: can't allocate %d bytes for AX25 sock state, PID: %d\n",
                Pn, (int)(pl + 1), Lp->pid);
        Error(ctx);
        return;
    }
    (void)snpf(cp, pl + 1, "%s", pbuf);
    Lf->nma = cp;
}

/*
 * print_ipxinfo() - print IPX socket info
 */
static void print_ipxinfo(struct lsof_context *ctx,
                          struct ipxsin *ip) /* IPX socket info */
{
    char *cp, pbuf[256];
    MALLOC_S pl;

    if (Lf->nma)
        return;
    (void)snpf(pbuf, sizeof(pbuf), "(Tx=%lx Rx=%lx State=%02x)", ip->txq,
               ip->rxq, ip->state);
    pl = strlen(pbuf);
    if (!(cp = (char *)malloc(pl + 1))) {
        if (ctx->err)
            (void)fprintf(
                ctx->err,
                "%s: can't allocate %d bytes for IPX sock state, PID: %d\n", Pn,
                (int)(pl + 1), Lp->pid);
        Error(ctx);
        return;
    }
    (void)snpf(cp, pl + 1, "%s", pbuf);
    Lf->nma = cp;
}

/*
 * process_proc_sock() - process /proc-based socket
 */
void process_proc_sock(struct lsof_context *ctx,
                       char *p,        /* node's readlink() path */
                       char *pbr,      /* node's path before readlink() */
                       struct stat *s, /* stat() result for path */
                       int ss,         /* *s status -- i.e, SB_* values */
                       struct stat *l, /* lstat() result for FD (NULL for
                                        * others) */
                       int lss)        /* *l status -- i.e, SB_* values */
{
    struct ax25sin *ap;
    char *cp, *path = (char *)NULL, tbuf[64];
    enum lsof_protocol proto;
    unsigned char *fa, *la;
    struct in_addr fs, ls;
    struct icmpin *icmpp;
    struct ipxsin *ip;
    int i, len, nl, rf;
    struct nlksin *np;
    struct packin *pp;
    char *pr;
    char *prp = (char *)NULL;
    struct rawsin *rp;
    struct sctpsin *sp;
    ssize_t sz;
    struct tcp_udp *tp;
    uxsin_t *up;

#if defined(HASIPv6)
    int af;
    struct tcp_udp6 *tp6;
#endif /* defined(HASIPv6) */

    /*
     * Enter offset, if possible.
     */
    if (l && (lss & SB_SIZE) && OffType != OFFSET_UNKNOWN) {
        Lf->off = (SZOFFTYPE)l->st_size;
        Lf->off_def = 1;
    }

    /*
     * Check for socket's inode presence in the protocol info caches.
     */
    if (!ctxd.ax25_valid) {
        (void)get_ax25(ctx, PROCFS "/net/ax25");
        ctxd.ax25_valid = 1;
    }
    if ((ss & SB_INO) && (ap = check_ax25(ctx, (INODETYPE)s->st_ino))) {

        /*
         * The inode is connected to an AX25 /proc record.
         *
         * Set the type to "ax25"; save the device name; save the inode number;
         * save the destination and source addresses; save the send and receive
         * queue sizes; and save the connection state.
         */
        Lf->type = LSOF_FILE_AX25;
        if (ap->dev_ch)
            (void)enter_dev_ch(ctx, ap->dev_ch);
        Lf->inode = ap->inode;
        Lf->inode_def = 1;
        print_ax25info(ctx, ap);
        return;
    }

    if (!ctxd.ipx_valid) {
        (void)get_ipx(ctx, PROCFS "/net/ipx");
        ctxd.ipx_valid = 1;
    }
    if ((ss & SB_INO) && (ip = check_ipx(ctx, (INODETYPE)s->st_ino))) {
        /*
         * The inode is connected to an IPX /proc record.
         *
         * Set the type to "ipx"; enter the inode and device numbers; store
         * the addresses, queue sizes, and state in the NAME column.
         */
        Lf->type = LSOF_FILE_IPX;
        Lf->inode = (INODETYPE)s->st_ino;
        Lf->inode_def = 1;

        if (ss & SB_DEV) {
            Lf->dev = s->st_dev;
            Lf->dev_def = 1;
        }

        cp = Namech;
        nl = Namechl;
        *cp = '\0';
        if (ip->la && nl) {
            /*
             * Store the local IPX address.
             */
            len = strlen(ip->la);
            if (len > nl - 1)
                len = nl - 1;
            (void)strncpy(cp, ip->la, len);
            cp += len;
            *cp = '\0';
            nl -= len;
        }
        if (ip->ra && nl) {
            /*
             * Store the remote IPX address, prefixed with "->".
             */
            if (nl > 2) {
                (void)snpf(cp, nl, "->");
                cp += 2;
                nl -= 2;
            }
            if (nl) {
                (void)snpf(cp, nl, "%s", ip->ra);
                len = strlen(ip->ra);
                cp += len;
                nl -= len;
            }
        }
        (void)print_ipxinfo(ctx, ip);
        if (Namech[0])
            enter_nm(ctx, Namech);
        return;
    }

    if (!ctxd.raw_valid) {
        (void)get_raw(ctx, PROCFS "/net/raw");
        ctxd.raw_valid = 1;
    }
    if ((ss & SB_INO) && (rp = check_raw(ctx, (INODETYPE)s->st_ino))) {
        /*
         * The inode is connected to a raw /proc record.
         *
         * Set the type to "raw"; enter the inode number; store the local
         * address, remote address, and state in the NAME column.
         */
        Lf->type = LSOF_FILE_RAW;
        Lf->inode = (INODETYPE)s->st_ino;
        Lf->inode_def = 1;

        cp = Namech;
        nl = Namechl - 2;
        *cp = '\0';
        if (rp->la && rp->lal) {
            /*
             * Store the local raw address.
             */
            if (nl > rp->lal) {
                (void)snpf(cp, nl, "%s", rp->la);
                cp += rp->lal;
                *cp = '\0';
                nl -= rp->lal;
            }
        }
        if (rp->ra && rp->ral) {
            /*
             * Store the remote raw address, prefixed with "->".
             */
            if (nl > (rp->ral + 2)) {
                (void)snpf(cp, nl, "->%s", rp->ra);
                cp += (rp->ral + 2);
                *cp = '\0';
                nl -= (rp->ral + 2);
            }
        }
        if (rp->sp && rp->spl) {
            /*
             * Store the state, optionally prefixed by a space, in the
             * form "st=x...x".
             */

            if (nl > (len = ((cp == Namech) ? 0 : 1) + 3 + rp->spl)) {
                (void)snpf(cp, nl, "%sst=%s", (cp == Namech) ? "" : " ",
                           rp->sp);
                cp += len;
                *cp = '\0';
                nl -= len;
            }
        }
        if (Namech[0])
            enter_nm(ctx, Namech);
        return;
    }

    if (!ctxd.netlink_valid) {
        (void)get_netlink(ctx, PROCFS "/net/netlink");
        ctxd.netlink_valid = 1;
    }
    if ((ss & SB_INO) && (np = check_netlink(ctx, (INODETYPE)s->st_ino))) {
        /*
         * The inode is connected to a Netlink /proc record.
         *
         * Set the type to "netlink" and store the protocol in the NAME
         * column.  Save the inode number.
         */

        Lf->type = LSOF_FILE_NETLINK;
        cp = netlink_proto_to_str(np->pr);
        if (cp)
            (void)snpf(Namech, Namechl, "%s", cp);
        else
            (void)snpf(Namech, Namechl, "unknown protocol: %d", np->pr);

        Lf->inode = (INODETYPE)s->st_ino;
        Lf->inode_def = 1;
        if (Namech[0])
            enter_nm(ctx, Namech);
        return;
    }

    if (!ctxd.packet_valid) {
        (void)get_pack(ctx, PROCFS "/net/packet");
        ctxd.packet_valid = 1;
    }
    if ((ss & SB_INO) && (pp = check_pack(ctx, (INODETYPE)s->st_ino))) {
        /*
         * The inode is connected to a packet /proc record.
         *
         * Set the type to "pack" and store the socket type in the NAME
         * column.  Put the protocol name in the NODE column and the inode
         * number in the DEVICE column.
         */
        Lf->type = LSOF_FILE_PACKET;
        cp = socket_type_to_str(pp->ty, &rf);
        (void)snpf(Namech, Namechl, "type=%s%s", rf ? "" : "SOCK_", cp);
        proto = ethernet_proto_convert(pp->pr);
        Lf->iproto = proto;
        if (Lf->iproto == LSOF_PROTOCOL_UNKNOWN) {
            Lf->unknown_proto_number = pp->pr;
        }
        if (ss & SB_INO) {
            (void)snpf(tbuf, sizeof(tbuf), "%" INODEPSPEC "d",
                       (INODETYPE)s->st_ino);
            tbuf[sizeof(tbuf) - 1] = '\0';
            enter_dev_ch(ctx, tbuf);
        }
        if (Namech[0])
            enter_nm(ctx, Namech);
        return;
    }

    if (!ctxd.unix_valid) {
        (void)get_unix(ctx, PROCFS "/net/unix");
        ctxd.unix_valid = 1;
    }
    if ((ss & SB_INO) && (up = check_unix(ctx, (INODETYPE)s->st_ino))) {

        /*
         * The inode is connected to a UNIX /proc record.
         *
         * Set the type to "unix"; enter the PCB address in the DEVICE column;
         * enter the inode number; and save the optional path.
         */
        if (Funix)
            Lf->sf |= SELUNX;
        Lf->type = LSOF_FILE_UNIX;
        if (up->pcb)
            enter_dev_ch(ctx, up->pcb);
        Lf->inode = (INODETYPE)s->st_ino;
        Lf->inode_def = 1;

        Lf->lts.type = up->ty;
#if defined(HASSOOPT)
        Lf->lts.opt = up->opt;
#endif /* defined(HASSOOPT) */
#if defined(HASSOSTATE)
        Lf->lts.ss = up->ss;
#endif /* defined(HASSOSTATE) */
#if defined(HASEPTOPTS) && defined(HASUXSOCKEPT)
        if (FeptE) {
            (void)enter_uxsinfo(ctx, up);
            Lf->sf |= SELUXSINFO;
        }
#endif /* defined(HASEPTOPTS) && defined(HASUXSOCKEPT) */

        cp = socket_type_to_str(up->ty, &rf);
        (void)snpf(Namech, Namechl - 1, "%s%stype=%s", up->path ? up->path : "",
                   up->path ? " " : "", cp);
        Namech[Namechl - 1] = '\0';
        (void)enter_nm(ctx, Namech);
        if (Sfile) {
            /*
             * See if this UNIX domain socket was specified as a search
             * argument.
             *
             * Search first by device and node numbers, if that is possible;
             * then search by name.
             */
            unsigned char f = 0; /* file-found flag */

            if (up->sb_def) {

                /*
                 * If the UNIX socket information includes stat(2) results, do
                 * a device and node number search.
                 *
                 * Note: that requires the saving, temporary modification and
                 *	 restoration of some *Lf values.
                 */
                unsigned char sv_dev_def;   /* saved dev_def */
                unsigned char sv_inode_def; /* saved inode_def */
                unsigned char sv_rdev_def;  /* saved rdev_def */
                dev_t sv_dev;               /* saved dev */
                INODETYPE sv_inode;         /* saved inode */
                dev_t sv_rdev;              /* saved rdev */

                sv_dev_def = Lf->dev_def;
                sv_dev = Lf->dev;
                sv_inode = Lf->inode;
                sv_inode_def = Lf->inode_def;
                sv_rdev_def = Lf->rdev_def;
                sv_rdev = Lf->rdev;
                Lf->dev_def = Lf->inode_def = Lf->rdev_def = 1;
                Lf->dev = up->sb_dev;
                Lf->inode = up->sb_ino;
                Lf->rdev = up->sb_rdev;
                if (is_file_named(ctx, 0, path, (struct mounts *)NULL, 0)) {
                    f = 1;
                    Lf->sf |= SELNM;
                }
                Lf->dev_def = sv_dev_def;
                Lf->dev = sv_dev;
                Lf->inode = sv_inode;
                Lf->inode_def = sv_inode_def;
                Lf->rdev_def = sv_rdev_def;
                Lf->rdev = sv_rdev;
            }
            if (!f && (ss & SB_MODE)) {

                /*
                 * If the file has not yet been found and the stat buffer has
                 * st_mode, search for the file by full path.
                 */
                if (is_file_named(ctx, 2, up->path ? up->path : p,
                                  (struct mounts *)NULL,
                                  ((s->st_mode & S_IFMT) == S_IFCHR))
                        ? 1
                        : 0) {
                    Lf->sf |= SELNM;
                }
            }
        }
        return;
    }

#if defined(HASIPv6)
    if (!ctxd.raw6_valid) {
        if (!Fxopt)
            (void)get_raw6(ctx, PROCFS "/net/raw6");
        ctxd.raw6_valid = 1;
    }
    if (!Fxopt && (ss & SB_INO) &&
        (rp = check_raw6(ctx, (INODETYPE)s->st_ino))) {

        /*
         * The inode is connected to a raw IPv6 /proc record.
         *
         * Set the type to "raw6"; enter the inode number; store the local
         * address, remote address, and state in the NAME column.
         */
        Lf->type = LSOF_FILE_RAW6;
        if (ss & SB_INO) {
            Lf->inode = (INODETYPE)s->st_ino;
            Lf->inode_def = 1;
        }
        cp = Namech;
        nl = MAXPATHLEN - 2;
        if (rp->la && rp->lal) {

            /*
             * Store the local raw IPv6 address.
             */
            if (nl > rp->lal) {
                (void)snpf(cp, nl, "%s", rp->la);
                cp += rp->lal;
                *cp = '\0';
                nl -= rp->lal;
            }
        }
        if (rp->ra && rp->ral) {
            /*
             * Store the remote raw address, prefixed with "->".
             */
            if (nl > (rp->ral + 2)) {
                (void)snpf(cp, nl, "->%s", rp->ra);
                cp += (rp->ral + 2);
                nl -= (rp->ral + 2);
            }
        }
        if (rp->sp && rp->spl) {

            /*
             * Store the state, optionally prefixed by a space, in the
             * form "st=x...x".
             */

            if (nl > (len = ((cp == Namech) ? 0 : 1) + 3 + rp->spl)) {
                (void)snpf(cp, nl, "%sst=%s", (cp == Namech) ? "" : " ",
                           rp->sp);
                cp += len;
                *cp = '\0';
                nl -= len;
            }
        }
        if (Namech[0])
            enter_nm(ctx, Namech);
        return;
    }

    if (!ctxd.tcp6_valid) {
        if (!Fxopt)
            (void)get_tcpudp6(ctx, PROCFS "/net/tcp6", 0, 1);
        ctxd.tcp6_valid = 1;
    }

    if (!ctxd.udp6_valid) {
        if (!Fxopt)
            (void)get_tcpudp6(ctx, PROCFS "/net/udp6", 1, 0);
        ctxd.udp6_valid = 1;
    }

    if (!ctxd.udplite6_valid) {
        if (!Fxopt)
            (void)get_tcpudp6(ctx, PROCFS "/net/udplite6", 2, 0);
        ctxd.udplite6_valid = 1;
    }

    if (!Fxopt && (ss & SB_INO) &&
        (tp6 = check_tcpudp6(ctx, (INODETYPE)s->st_ino, &proto))) {

        /*
         * The inode is connected to an IPv6 TCP or UDP /proc record.
         *
         * Set the type to "IPv6"; enter the protocol; put the inode number
         * in the DEVICE column in lieu of the PCB address; save the local
         * and foreign IPv6 addresses; save the type and protocol; and
         * (optionally) save the queue sizes.
         */
        i = tp6->state + TcpStOff;
        if (TcpStXn) {

            /*
             * Check for state exclusion.
             */
            if (i >= 0 && i < TcpNstates) {
                if (TcpStX[i]) {
                    Lf->sf |= SELEXCLF;
                    return;
                }
            }
        }
        if (TcpStIn) {

            /*
             * Check for state inclusion.
             */
            if (i >= 0 && i < TcpNstates) {
                if (TcpStI[i])
                    TcpStI[i] = 2;
                else {
                    Lf->sf |= SELEXCLF;
                    return;
                }
            }
        }
        if (Fnet && FnetTy != AF_INET)
            Lf->sf |= SELNET;
        Lf->type = LSOF_FILE_IPV6;
        Lf->iproto = proto;
        if (ss & SB_INO) {
            (void)snpf(tbuf, sizeof(tbuf), "%" INODEPSPEC "d",
                       (INODETYPE)s->st_ino);
            tbuf[sizeof(tbuf) - 1] = '\0';
            enter_dev_ch(ctx, tbuf);
            Lf->inode = (INODETYPE)s->st_ino;
        }
        af = AF_INET6;
        if (!IN6_IS_ADDR_UNSPECIFIED(&tp6->faddr) || tp6->fport)
            fa = (unsigned char *)&tp6->faddr;
        else
            fa = (unsigned char *)NULL;
        if (!IN6_IS_ADDR_UNSPECIFIED(&tp6->laddr) || tp6->lport)
            la = (unsigned char *)&tp6->laddr;
        else
            la = (unsigned char *)NULL;
        if ((fa && IN6_IS_ADDR_V4MAPPED(&tp6->faddr)) ||
            (la && IN6_IS_ADDR_V4MAPPED(&tp6->laddr))) {
            af = AF_INET;
            if (fa)
                fa += 12;
            if (la)
                la += 12;
        }
        ent_inaddr(ctx, la, tp6->lport, fa, tp6->fport, af);
        Lf->lts.type = tp6->proto;
        Lf->lts.state.i = tp6->state;

#    if defined(HASTCPTPIQ)
        Lf->lts.rq = tp6->rxq;
        Lf->lts.sq = tp6->txq;
        Lf->lts.rqs = Lf->lts.sqs = 1;
#    endif /* defined(HASTCPTPIQ) */

#    if defined(HASEPTOPTS)
        if (FeptE && tp6->ipc_peer) {
            (void)enter_nets6info(ctx, tp6);
            Lf->sf |= SELNETS6INFO;
        }
#    endif /* defined(HASEPTOPTS) */
        return;
    }
#endif /* defined(HASIPv6) */

    if (!ctxd.tcp_valid) {
        if (!Fxopt)
            (void)get_tcpudp(ctx, PROCFS "/net/tcp", 0, 1);
        ctxd.tcp_valid = 1;
    }

    if (!ctxd.udp_valid) {
        if (!Fxopt)
            (void)get_tcpudp(ctx, PROCFS "/net/udp", 1, 0);
        ctxd.udp_valid = 1;
    }

    if (!ctxd.udplite_valid) {
        if (!Fxopt)
            (void)get_tcpudp(ctx, PROCFS "/net/udplite", 2, 0);
        ctxd.udplite_valid = 1;
    }

    if (!Fxopt && (ss & SB_INO) &&
        (tp = check_tcpudp(ctx, (INODETYPE)s->st_ino, &proto))) {

        /*
         * The inode is connected to an IPv4 TCP or UDP /proc record.
         *
         * Set the type to "inet" or "IPv4"; enter the protocol; put the
         * inode number in the DEVICE column in lieu of the PCB address;
         * save the local and foreign IPv4 addresses; save the type and
         * protocol; and (optionally) save the queue sizes.
         */
        i = tp->state + TcpStOff;
        if (TcpStXn) {

            /*
             * Check for state exclusion.
             */
            if (i >= 0 && i < TcpNstates) {
                if (TcpStX[i]) {
                    Lf->sf |= SELEXCLF;
                    return;
                }
            }
        }
        if (TcpStIn) {

            /*
             * Check for state inclusion.
             */
            if (i >= 0 && i < TcpNstates) {
                if (TcpStI[i])
                    TcpStI[i] = 2;
                else {
                    Lf->sf |= SELEXCLF;
                    return;
                }
            }
        }
        if (Fnet && (FnetTy != AF_INET6))
            Lf->sf |= SELNET;

#if defined(HASIPv6)
        Lf->type = LSOF_FILE_IPV4;
#else  /* !defined(HASIPv6) */
        Lf->type = LSOF_FILE_INET;
#endif /* defined(HASIPv6) */

        Lf->iproto = proto;
        if (ss & SB_INO) {
            (void)snpf(tbuf, sizeof(tbuf), "%" INODEPSPEC "d",
                       (INODETYPE)s->st_ino);
            tbuf[sizeof(tbuf) - 1] = '\0';
            enter_dev_ch(ctx, tbuf);
            Lf->inode = (INODETYPE)s->st_ino;
        }
        if (tp->faddr || tp->fport) {
            fs.s_addr = tp->faddr;
            fa = (unsigned char *)&fs;
        } else
            fa = (unsigned char *)NULL;
        if (tp->laddr || tp->lport) {
            ls.s_addr = tp->laddr;
            la = (unsigned char *)&ls;
        } else
            la = (unsigned char *)NULL;
        ent_inaddr(ctx, la, tp->lport, fa, tp->fport, AF_INET);
        Lf->lts.type = tp->proto;
        Lf->lts.state.i = tp->state;

#if defined(HASTCPTPIQ)
        Lf->lts.rq = tp->rxq;
        Lf->lts.sq = tp->txq;
        Lf->lts.rqs = Lf->lts.sqs = 1;
#endif /* defined(HASTCPTPIQ) */

#if defined(HASEPTOPTS)
        if (FeptE && tp->ipc_peer) {
            (void)enter_netsinfo(ctx, tp);
            Lf->sf |= SELNETSINFO;
        }
#endif /* defined(HASEPTOPTS) */

        return;
    }

    if (!ctxd.sctp_valid) {
        (void)get_sctp(ctx);
        ctxd.sctp_valid = 1;
    }
    if ((ss & SB_INO) && (sp = check_sctp(ctx, (INODETYPE)s->st_ino))) {

        /*
         * The inode is connected to an SCTP /proc record.
         *
         * Set the type to "sock"; enter the inode number in the DEVICE
         * column; set the protocol to SCTP; and fill in the NAME column
         * with ASSOC, ASSOC-ID, ENDPT, LADDRS, LPORT, RADDRS and RPORT.
         */
        Lf->type = LSOF_FILE_SOCKET;
        Lf->iproto = LSOF_PROTOCOL_SCTP;
        (void)snpf(tbuf, sizeof(tbuf), "%" INODEPSPEC "d",
                   (INODETYPE)s->st_ino);
        tbuf[sizeof(tbuf) - 1] = '\0';
        enter_dev_ch(ctx, tbuf);
        Namech[0] = '\0';
        if (sp->type == 1) {

            /*
             * This is an ENDPT SCTP file.
             */
            (void)snpf(Namech, Namechl, "ENDPT: %s%s%s%s%s%s",
                       sp->addr ? sp->addr : "",
                       (sp->laddrs || sp->lport) ? " " : "",
                       sp->laddrs ? sp->laddrs : "", sp->lport ? "[" : "",
                       sp->lport ? sp->lport : "", sp->lport ? "]" : "");
        } else {

            /*
             * This is an ASSOC, or ASSOC and ENDPT socket file.
             */
            (void)snpf(
                Namech, Namechl, "%s: %s%s%s %s%s%s%s%s%s%s%s%s",
                sp->type ? "ASSOC+ENDPT" : "ASSOC", sp->addr ? sp->addr : "",
                (sp->addr && sp->assocID) ? "," : "",
                sp->assocID ? sp->assocID : "", sp->laddrs ? sp->laddrs : "",
                sp->lport ? "[" : "", sp->lport ? sp->lport : "",
                sp->lport ? "]" : "",
                ((sp->laddrs || sp->lport) && (sp->raddrs || sp->rport)) ? "<->"
                                                                         : "",
                sp->raddrs ? sp->raddrs : "", sp->rport ? "[" : "",
                sp->rport ? sp->rport : "", sp->rport ? "]" : "");
        }
        if (Namech[0])
            enter_nm(ctx, Namech);
        return;
    }

    if (!ctxd.icmp_valid) {
        (void)get_icmp(ctx, PROCFS "/net/icmp");
        ctxd.icmp_valid = 1;
    }
    if ((ss & SB_INO) && (icmpp = check_icmp(ctx, (INODETYPE)s->st_ino))) {

        /*
         * The inode is connected to an ICMP /proc record.
         *
         * Set the type to "icmp" and store the type in the NAME
         * column.  Save the inode number.
         */
        Lf->type = LSOF_FILE_ICMP;
        Lf->inode = (INODETYPE)s->st_ino;
        Lf->inode_def = 1;
        cp = Namech;
        nl = Namechl - 2;
        *cp = '\0';
        if (icmpp->la && icmpp->lal) {

            /*
             * Store the local raw address.
             */
            if (nl > icmpp->lal) {
                (void)snpf(cp, nl, "%s", icmpp->la);
                cp += icmpp->lal;
                *cp = '\0';
                nl -= icmpp->lal;
            }
        }
        if (icmpp->ra && icmpp->ral) {

            /*
             * Store the remote raw address, prefixed with "->".
             */
            if (nl > (icmpp->ral + 2)) {
                (void)snpf(cp, nl, "->%s", icmpp->ra);
                cp += (icmpp->ral + 2);
                *cp = '\0';
                nl -= (icmpp->ral + 2);
            }
        }
        if (Namech[0])
            enter_nm(ctx, Namech);
        return;
    }

    /*
     * The socket's protocol can't be identified.
     */
    Lf->type = LSOF_FILE_SOCKET;
    if (ss & SB_INO) {
        Lf->inode = (INODETYPE)s->st_ino;
        Lf->inode_def = 1;
    }
    if (ss & SB_DEV) {
        Lf->dev = s->st_dev;
        Lf->dev_def = 1;
    }
    if (Fxopt)
        enter_nm(ctx, "can't identify protocol (-X specified)");
    else {
        (void)snpf(Namech, Namechl, "protocol: ");
        if (!prp) {
            i = (int)strlen(Namech);
            prp = &Namech[i];
            sz = (ssize_t)(Namechl - i - 1);
        }
        if ((getxattr(pbr, "system.sockprotoname", prp, sz)) < 0)
            enter_nm(ctx, "can't identify protocol");
        else
            enter_nm(ctx, Namech);
    }
}

/*
 * refresh_socket_info() - refresh socket info
 */
void refresh_socket_info(struct lsof_context *ctx) {
    /* Refresh socket info */
    ctxd.ax25_valid = 0;
    ctxd.icmp_valid = 0;
    ctxd.ipx_valid = 0;
    ctxd.netlink_valid = 0;
    ctxd.packet_valid = 0;
    ctxd.raw_valid = 0;
    ctxd.sctp_valid = 0;
    ctxd.tcp_valid = 0;
    ctxd.udp_valid = 0;
    ctxd.udplite_valid = 0;

#if defined(HASIPv6)
    ctxd.raw6_valid = 0;
    ctxd.tcp6_valid = 0;
    ctxd.udp6_valid = 0;
    ctxd.udplite6_valid = 0;
#endif /* defined(HASIPv6) */

    ctxd.unix_valid = 0;
}

/*
 * socket_type_to_str() -- convert socket type number to a string
 */

static char *socket_type_to_str(uint32_t ty, /* socket type number */
                                int *rf)     /* result flag: 0 == known
                                              *		1 = unknown */
{
    int f = 0; /* result flag */
    char *sr;  /* string result */

    switch (ty) {

#if defined(SOCK_STREAM)
    case SOCK_STREAM:
        sr = "STREAM";
        break;
#endif /* defined(SOCK_STREAM) */

#if defined(SOCK_DGRAM)
    case SOCK_DGRAM:
        sr = "DGRAM";
        break;
#endif /* defined(SOCK_DGRAM) */

#if defined(SOCK_RAW)
    case SOCK_RAW:
        sr = "RAW";
        break;
#endif /* defined(SOCK_RAW) */

#if defined(SOCK_RDM)
    case SOCK_RDM:
        sr = "RDM";
        break;
#endif /* defined(SOCK_RDM) */

#if defined(SOCK_SEQPACKET)
    case SOCK_SEQPACKET:
        sr = "SEQPACKET";
        break;
#endif /* defined(SOCK_SEQPACKET) */

#if defined(SOCK_DCCP)
    case SOCK_DCCP:
        sr = "DCCP";
        break;
#endif /* defined(SOCK_DCCP) */

#if defined(SOCK_PACKET)
    case SOCK_PACKET:
        sr = "PACKET";
        break;
#endif /* defined(SOCK_PACKET) */

    default:
        f = 1;
        sr = "unknown";
    }
    *rf = f;
    return (sr);
}

/*
 * netlink_proto_to_str() -- convert netlink protocol number to a string
 *
 * return NULL if the number is unknown.
 */
static char *netlink_proto_to_str(unsigned int pr) {
    char *cp = NULL;
    switch (pr) {
#if defined(NETLINK_ROUTE)
    case NETLINK_ROUTE:
        cp = "ROUTE";
        break;
#endif /* defined(NETLINK_ROUTE) */

#if defined(NETLINK_UNUSED)
    case NETLINK_UNUSED:
        cp = "UNUSED";
        break;
#endif /* defined(NETLINK_UNUSED) */

#if defined(NETLINK_USERSOCK)
    case NETLINK_USERSOCK:
        cp = "USERSOCK";
        break;
#endif /* defined(NETLINK_USERSOCK) */

#if defined(NETLINK_FIREWALL)
    case NETLINK_FIREWALL:
        cp = "FIREWALL";
        break;
#endif /* defined(NETLINK_FIREWALL) */

#if defined(NETLINK_INET_DIAG)
    case NETLINK_INET_DIAG:
        cp = "INET_DIAG";
        break;
#endif /* defined(NETLINK_INET_DIAG) */

#if defined(NETLINK_NFLOG)
    case NETLINK_NFLOG:
        cp = "NFLOG";
        break;
#endif /* defined(NETLINK_NFLOG) */

#if defined(NETLINK_XFRM)
    case NETLINK_XFRM:
        cp = "XFRM";
        break;
#endif /* defined(NETLINK_XFRM) */

#if defined(NETLINK_SELINUX)
    case NETLINK_SELINUX:
        cp = "SELINUX";
        break;
#endif /* defined(NETLINK_SELINUX) */

#if defined(NETLINK_ISCSI)
    case NETLINK_ISCSI:
        cp = "ISCSI";
        break;
#endif /* defined(NETLINK_ISCSI) */

#if defined(NETLINK_AUDIT)
    case NETLINK_AUDIT:
        cp = "AUDIT";
        break;
#endif /* defined(NETLINK_AUDIT) */

#if defined(NETLINK_FIB_LOOKUP)
    case NETLINK_FIB_LOOKUP:
        cp = "FIB_LOOKUP";
        break;
#endif /* defined(NETLINK_FIB_LOOKUP) */

#if defined(NETLINK_CONNECTOR)
    case NETLINK_CONNECTOR:
        cp = "CONNECTOR";
        break;
#endif /* defined(NETLINK_CONNECTOR) */

#if defined(NETLINK_NETFILTER)
    case NETLINK_NETFILTER:
        cp = "NETFILTER";
        break;
#endif /* defined(NETLINK_NETFILTER) */

#if defined(NETLINK_IP6_FW)
    case NETLINK_IP6_FW:
        cp = "IP6_FW";
        break;
#endif /* defined(NETLINK_IP6_FW) */

#if defined(NETLINK_DNRTMSG)
    case NETLINK_DNRTMSG:
        cp = "DNRTMSG";
        break;
#endif /* defined(NETLINK_DNRTMSG) */

#if defined(NETLINK_KOBJECT_UEVENT)
    case NETLINK_KOBJECT_UEVENT:
        cp = "KOBJECT_UEVENT";
        break;
#endif /* defined(NETLINK_KOBJECT_UEVENT) */

#if defined(NETLINK_GENERIC)
    case NETLINK_GENERIC:
        cp = "GENERIC";
        break;
#endif /* defined(NETLINK_GENERIC) */

#if defined(NETLINK_SCSITRANSPORT)
    case NETLINK_SCSITRANSPORT:
        cp = "SCSITRANSPORT";
        break;
#endif /* defined(NETLINK_SCSITRANSPORT) */

#if defined(NETLINK_ECRYPTFS)
    case NETLINK_ECRYPTFS:
        cp = "ECRYPTFS";
        break;
#endif /* defined(NETLINK_ECRYPTFS) */

#if defined(NETLINK_RDMA)
    case NETLINK_RDMA:
        cp = "RDMA";
        break;
#endif /* defined(NETLINK_RDMA) */

#if defined(NETLINK_CRYPTO)
    case NETLINK_CRYPTO:
        cp = "CRYPTO";
        break;
#endif /* defined(NETLINK_CRYPTO) */

#if defined(NETLINK_SMC)
    case NETLINK_SMC:
        cp = "SMC";
        break;
#endif /* defined(NETLINK_SMC) */
    }

    return cp;
}

/*
 * ethernet_proto_convert() -- convert ethernet protocol number to enum
 * lsof_protocol
 */
static enum lsof_protocol ethernet_proto_convert(unsigned int pr) {
    enum lsof_protocol cp = LSOF_PROTOCOL_UNKNOWN;
    switch (pr) {
#if defined(ETH_P_LOOP)
    case ETH_P_LOOP:
        cp = LSOF_PROTOCOL_LOOP;
        break;
#endif /* defined(ETH_P_LOOP) */

#if defined(ETH_P_PUP)
    case ETH_P_PUP:
        cp = LSOF_PROTOCOL_PUP;
        break;
#endif /* defined(ETH_P_PUP) */

#if defined(ETH_P_PUPAT)
    case ETH_P_PUPAT:
        cp = LSOF_PROTOCOL_PUPAT;
        break;
#endif /* defined(ETH_P_PUPAT) */

#if defined(ETH_P_TSN)
    case ETH_P_TSN:
        cp = LSOF_PROTOCOL_TSN;
        break;
#endif /* defined(ETH_P_TSN) */

#if defined(ETH_P_ERSPAN2)
    case ETH_P_ERSPAN2:
        cp = LSOF_PROTOCOL_ERSPAN2;
        break;
#endif /* defined(ETH_P_ERSPAN2) */

#if defined(ETH_P_IP)
    case ETH_P_IP:
        cp = LSOF_PROTOCOL_IP;
        break;
#endif /* defined(ETH_P_IP) */

#if defined(ETH_P_X25)
    case ETH_P_X25:
        cp = LSOF_PROTOCOL_X25;
        break;
#endif /* defined(ETH_P_X25) */

#if defined(ETH_P_ARP)
    case ETH_P_ARP:
        cp = LSOF_PROTOCOL_ARP;
        break;
#endif /* defined(ETH_P_ARP) */

#if defined(ETH_P_BPQ)
    case ETH_P_BPQ:
        cp = LSOF_PROTOCOL_BPQ;
        break;
#endif /* defined(ETH_P_BPQ) */

#if defined(ETH_P_IEEEPUP)
    case ETH_P_IEEEPUP:
        cp = LSOF_PROTOCOL_IEEEPUP;
        break;
#endif /* defined(ETH_P_IEEEPUP) */

#if defined(ETH_P_IEEEPUPAT)
    case ETH_P_IEEEPUPAT:
        cp = LSOF_PROTOCOL_IEEEPUPAT;
        break;
#endif /* defined(ETH_P_IEEEPUPAT) */

#if defined(ETH_P_BATMAN)
    case ETH_P_BATMAN:
        cp = LSOF_PROTOCOL_BATMAN;
        break;
#endif /* defined(ETH_P_BATMAN) */

#if defined(ETH_P_DEC)
    case ETH_P_DEC:
        cp = LSOF_PROTOCOL_DEC;
        break;
#endif /* defined(ETH_P_DEC) */

#if defined(ETH_P_DNA_DL)
    case ETH_P_DNA_DL:
        cp = LSOF_PROTOCOL_DNA_DL;
        break;
#endif /* defined(ETH_P_DNA_DL) */

#if defined(ETH_P_DNA_RC)
    case ETH_P_DNA_RC:
        cp = LSOF_PROTOCOL_DNA_RC;
        break;
#endif /* defined(ETH_P_DNA_RC) */

#if defined(ETH_P_DNA_RT)
    case ETH_P_DNA_RT:
        cp = LSOF_PROTOCOL_DNA_RT;
        break;
#endif /* defined(ETH_P_DNA_RT) */

#if defined(ETH_P_LAT)
    case ETH_P_LAT:
        cp = LSOF_PROTOCOL_LAT;
        break;
#endif /* defined(ETH_P_LAT) */

#if defined(ETH_P_DIAG)
    case ETH_P_DIAG:
        cp = LSOF_PROTOCOL_DIAG;
        break;
#endif /* defined(ETH_P_DIAG) */

#if defined(ETH_P_CUST)
    case ETH_P_CUST:
        cp = LSOF_PROTOCOL_CUST;
        break;
#endif /* defined(ETH_P_CUST) */

#if defined(ETH_P_SCA)
    case ETH_P_SCA:
        cp = LSOF_PROTOCOL_SCA;
        break;
#endif /* defined(ETH_P_SCA) */

#if defined(ETH_P_TEB)
    case ETH_P_TEB:
        cp = LSOF_PROTOCOL_TEB;
        break;
#endif /* defined(ETH_P_TEB) */

#if defined(ETH_P_RARP)
    case ETH_P_RARP:
        cp = LSOF_PROTOCOL_RARP;
        break;
#endif /* defined(ETH_P_RARP) */

#if defined(ETH_P_ATALK)
    case ETH_P_ATALK:
        cp = LSOF_PROTOCOL_ATALK;
        break;
#endif /* defined(ETH_P_ATALK) */

#if defined(ETH_P_AARP)
    case ETH_P_AARP:
        cp = LSOF_PROTOCOL_AARP;
        break;
#endif /* defined(ETH_P_AARP) */

#if defined(ETH_P_8021Q)
    case ETH_P_8021Q:
        cp = LSOF_PROTOCOL_8021Q;
        break;
#endif /* defined(ETH_P_8021Q) */

#if defined(ETH_P_ERSPAN)
    case ETH_P_ERSPAN:
        cp = LSOF_PROTOCOL_ERSPAN;
        break;
#endif /* defined(ETH_P_ERSPAN) */

#if defined(ETH_P_IPX)
    case ETH_P_IPX:
        cp = LSOF_PROTOCOL_IPX;
        break;
#endif /* defined(ETH_P_IPX) */

#if defined(ETH_P_IPV6)
    case ETH_P_IPV6:
        cp = LSOF_PROTOCOL_IPV6;
        break;
#endif /* defined(ETH_P_IPV6) */

#if defined(ETH_P_PAUSE)
    case ETH_P_PAUSE:
        cp = LSOF_PROTOCOL_PAUSE;
        break;
#endif /* defined(ETH_P_PAUSE) */

#if defined(ETH_P_SLOW)
    case ETH_P_SLOW:
        cp = LSOF_PROTOCOL_SLOW;
        break;
#endif /* defined(ETH_P_SLOW) */

#if defined(ETH_P_WCCP)
    case ETH_P_WCCP:
        cp = LSOF_PROTOCOL_WCCP;
        break;
#endif /* defined(ETH_P_WCCP) */

#if defined(ETH_P_MPLS_UC)
    case ETH_P_MPLS_UC:
        cp = LSOF_PROTOCOL_MPLS_UC;
        break;
#endif /* defined(ETH_P_MPLS_UC) */

#if defined(ETH_P_MPLS_MC)
    case ETH_P_MPLS_MC:
        cp = LSOF_PROTOCOL_MPLS_MC;
        break;
#endif /* defined(ETH_P_MPLS_MC) */

#if defined(ETH_P_ATMMPOA)
    case ETH_P_ATMMPOA:
        cp = LSOF_PROTOCOL_ATMMPOA;
        break;
#endif /* defined(ETH_P_ATMMPOA) */

#if defined(ETH_P_PPP_DISC)
    case ETH_P_PPP_DISC:
        cp = LSOF_PROTOCOL_PPP_DISC;
        break;
#endif /* defined(ETH_P_PPP_DISC) */

#if defined(ETH_P_PPP_SES)
    case ETH_P_PPP_SES:
        cp = LSOF_PROTOCOL_PPP_SES;
        break;
#endif /* defined(ETH_P_PPP_SES) */

#if defined(ETH_P_LINK_CTL)
    case ETH_P_LINK_CTL:
        cp = LSOF_PROTOCOL_LINK_CTL;
        break;
#endif /* defined(ETH_P_LINK_CTL) */

#if defined(ETH_P_ATMFATE)
    case ETH_P_ATMFATE:
        cp = LSOF_PROTOCOL_ATMFATE;
        break;
#endif /* defined(ETH_P_ATMFATE) */

#if defined(ETH_P_PAE)
    case ETH_P_PAE:
        cp = LSOF_PROTOCOL_PAE;
        break;
#endif /* defined(ETH_P_PAE) */

#if defined(ETH_P_AOE)
    case ETH_P_AOE:
        cp = LSOF_PROTOCOL_AOE;
        break;
#endif /* defined(ETH_P_AOE) */

#if defined(ETH_P_8021AD)
    case ETH_P_8021AD:
        cp = LSOF_PROTOCOL_8021AD;
        break;
#endif /* defined(ETH_P_8021AD) */

#if defined(ETH_P_802_EX1)
    case ETH_P_802_EX1:
        cp = LSOF_PROTOCOL_802_EX1;
        break;
#endif /* defined(ETH_P_802_EX1) */

#if defined(ETH_P_PREAUTH)
    case ETH_P_PREAUTH:
        cp = LSOF_PROTOCOL_PREAUTH;
        break;
#endif /* defined(ETH_P_PREAUTH) */

#if defined(ETH_P_TIPC)
    case ETH_P_TIPC:
        cp = LSOF_PROTOCOL_TIPC;
        break;
#endif /* defined(ETH_P_TIPC) */

#if defined(ETH_P_LLDP)
    case ETH_P_LLDP:
        cp = LSOF_PROTOCOL_LLDP;
        break;
#endif /* defined(ETH_P_LLDP) */

#if defined(ETH_P_MRP)
    case ETH_P_MRP:
        cp = LSOF_PROTOCOL_MRP;
        break;
#endif /* defined(ETH_P_MRP) */

#if defined(ETH_P_MACSEC)
    case ETH_P_MACSEC:
        cp = LSOF_PROTOCOL_MACSEC;
        break;
#endif /* defined(ETH_P_MACSEC) */

#if defined(ETH_P_8021AH)
    case ETH_P_8021AH:
        cp = LSOF_PROTOCOL_8021AH;
        break;
#endif /* defined(ETH_P_8021AH) */

#if defined(ETH_P_MVRP)
    case ETH_P_MVRP:
        cp = LSOF_PROTOCOL_MVRP;
        break;
#endif /* defined(ETH_P_MVRP) */

#if defined(ETH_P_1588)
    case ETH_P_1588:
        cp = LSOF_PROTOCOL_1588;
        break;
#endif /* defined(ETH_P_1588) */

#if defined(ETH_P_NCSI)
    case ETH_P_NCSI:
        cp = LSOF_PROTOCOL_NCSI;
        break;
#endif /* defined(ETH_P_NCSI) */

#if defined(ETH_P_PRP)
    case ETH_P_PRP:
        cp = LSOF_PROTOCOL_PRP;
        break;
#endif /* defined(ETH_P_PRP) */

#if defined(ETH_P_FCOE)
    case ETH_P_FCOE:
        cp = LSOF_PROTOCOL_FCOE;
        break;
#endif /* defined(ETH_P_FCOE) */

#if defined(ETH_P_IBOE)
    case ETH_P_IBOE:
        cp = LSOF_PROTOCOL_IBOE;
        break;
#endif /* defined(ETH_P_IBOE) */

#if defined(ETH_P_TDLS)
    case ETH_P_TDLS:
        cp = LSOF_PROTOCOL_TDLS;
        break;
#endif /* defined(ETH_P_TDLS) */

#if defined(ETH_P_FIP)
    case ETH_P_FIP:
        cp = LSOF_PROTOCOL_FIP;
        break;
#endif /* defined(ETH_P_FIP) */

#if defined(ETH_P_80221)
    case ETH_P_80221:
        cp = LSOF_PROTOCOL_80221;
        break;
#endif /* defined(ETH_P_80221) */

#if defined(ETH_P_HSR)
    case ETH_P_HSR:
        cp = LSOF_PROTOCOL_HSR;
        break;
#endif /* defined(ETH_P_HSR) */

#if defined(ETH_P_NSH)
    case ETH_P_NSH:
        cp = LSOF_PROTOCOL_NSH;
        break;
#endif /* defined(ETH_P_NSH) */

#if defined(ETH_P_LOOPBACK)
    case ETH_P_LOOPBACK:
        cp = LSOF_PROTOCOL_LOOPBACK;
        break;
#endif /* defined(ETH_P_LOOPBACK) */

#if defined(ETH_P_QINQ1)
    case ETH_P_QINQ1:
        cp = LSOF_PROTOCOL_QINQ1;
        break;
#endif /* defined(ETH_P_QINQ1) */

#if defined(ETH_P_QINQ2)
    case ETH_P_QINQ2:
        cp = LSOF_PROTOCOL_QINQ2;
        break;
#endif /* defined(ETH_P_QINQ2) */

#if defined(ETH_P_QINQ3)
    case ETH_P_QINQ3:
        cp = LSOF_PROTOCOL_QINQ3;
        break;
#endif /* defined(ETH_P_QINQ3) */

#if defined(ETH_P_EDSA)
    case ETH_P_EDSA:
        cp = LSOF_PROTOCOL_EDSA;
        break;
#endif /* defined(ETH_P_EDSA) */

#if defined(ETH_P_DSA_8021Q)
    case ETH_P_DSA_8021Q:
        cp = LSOF_PROTOCOL_DSA_8021Q;
        break;
#endif /* defined(ETH_P_DSA_8021Q) */

#if defined(ETH_P_IFE)
    case ETH_P_IFE:
        cp = LSOF_PROTOCOL_IFE;
        break;
#endif /* defined(ETH_P_IFE) */

#if defined(ETH_P_AF_IUCV)
    case ETH_P_AF_IUCV:
        cp = LSOF_PROTOCOL_AF_IUCV;
        break;
#endif /* defined(ETH_P_AF_IUCV) */

#if defined(ETH_P_802_3)
    case ETH_P_802_3:
        cp = LSOF_PROTOCOL_802_3;
        break;
#endif /* defined(ETH_P_802_3) */

#if defined(ETH_P_AX25)
    case ETH_P_AX25:
        cp = LSOF_PROTOCOL_AX25;
        break;
#endif /* defined(ETH_P_AX25) */

#if defined(ETH_P_ALL)
    case ETH_P_ALL:
        cp = LSOF_PROTOCOL_ALL;
        break;
#endif /* defined(ETH_P_ALL) */

#if defined(ETH_P_802_2)
    case ETH_P_802_2:
        cp = LSOF_PROTOCOL_802_2;
        break;
#endif /* defined(ETH_P_802_2) */

#if defined(ETH_P_SNAP)
    case ETH_P_SNAP:
        cp = LSOF_PROTOCOL_SNAP;
        break;
#endif /* defined(ETH_P_SNAP) */

#if defined(ETH_P_DDCMP)
    case ETH_P_DDCMP:
        cp = LSOF_PROTOCOL_DDCMP;
        break;
#endif /* defined(ETH_P_DDCMP) */

#if defined(ETH_P_WAN_PPP)
    case ETH_P_WAN_PPP:
        cp = LSOF_PROTOCOL_WAN_PPP;
        break;
#endif /* defined(ETH_P_WAN_PPP) */

#if defined(ETH_P_PPP_MP)
    case ETH_P_PPP_MP:
        cp = LSOF_PROTOCOL_PPP_MP;
        break;
#endif /* defined(ETH_P_PPP_MP) */

#if defined(ETH_P_LOCALTALK)
    case ETH_P_LOCALTALK:
        cp = LSOF_PROTOCOL_LOCALTALK;
        break;
#endif /* defined(ETH_P_LOCALTALK) */

#if defined(ETH_P_CAN)
    case ETH_P_CAN:
        cp = LSOF_PROTOCOL_CAN;
        break;
#endif /* defined(ETH_P_CAN) */

#if defined(ETH_P_CANFD)
    case ETH_P_CANFD:
        cp = LSOF_PROTOCOL_CANFD;
        break;
#endif /* defined(ETH_P_CANFD) */

#if defined(ETH_P_PPPTALK)
    case ETH_P_PPPTALK:
        cp = LSOF_PROTOCOL_PPPTALK;
        break;
#endif /* defined(ETH_P_PPPTALK) */

#if defined(ETH_P_TR_802_2)
    case ETH_P_TR_802_2:
        cp = LSOF_PROTOCOL_TR_802_2;
        break;
#endif /* defined(ETH_P_TR_802_2) */

#if defined(ETH_P_MOBITEX)
    case ETH_P_MOBITEX:
        cp = LSOF_PROTOCOL_MOBITEX;
        break;
#endif /* defined(ETH_P_MOBITEX) */

#if defined(ETH_P_CONTROL)
    case ETH_P_CONTROL:
        cp = LSOF_PROTOCOL_CONTROL;
        break;
#endif /* defined(ETH_P_CONTROL) */

#if defined(ETH_P_IRDA)
    case ETH_P_IRDA:
        cp = LSOF_PROTOCOL_IRDA;
        break;
#endif /* defined(ETH_P_IRDA) */

#if defined(ETH_P_ECONET)
    case ETH_P_ECONET:
        cp = LSOF_PROTOCOL_ECONET;
        break;
#endif /* defined(ETH_P_ECONET) */

#if defined(ETH_P_HDLC)
    case ETH_P_HDLC:
        cp = LSOF_PROTOCOL_HDLC;
        break;
#endif /* defined(ETH_P_HDLC) */

#if defined(ETH_P_ARCNET)
    case ETH_P_ARCNET:
        cp = LSOF_PROTOCOL_ARCNET;
        break;
#endif /* defined(ETH_P_ARCNET) */

#if defined(ETH_P_DSA)
    case ETH_P_DSA:
        cp = LSOF_PROTOCOL_DSA;
        break;
#endif /* defined(ETH_P_DSA) */

#if defined(ETH_P_TRAILER)
    case ETH_P_TRAILER:
        cp = LSOF_PROTOCOL_TRAILER;
        break;
#endif /* defined(ETH_P_TRAILER) */

#if defined(ETH_P_PHONET)
    case ETH_P_PHONET:
        cp = LSOF_PROTOCOL_PHONET;
        break;
#endif /* defined(ETH_P_PHONET) */

#if defined(ETH_P_IEEE802154)
    case ETH_P_IEEE802154:
        cp = LSOF_PROTOCOL_IEEE802154;
        break;
#endif /* defined(ETH_P_IEEE802154) */

#if defined(ETH_P_CAIF)
    case ETH_P_CAIF:
        cp = LSOF_PROTOCOL_CAIF;
        break;
#endif /* defined(ETH_P_CAIF) */

#if defined(ETH_P_XDSA)
    case ETH_P_XDSA:
        cp = LSOF_PROTOCOL_XDSA;
        break;
#endif /* defined(ETH_P_XDSA) */

#if defined(ETH_P_MAP)
    case ETH_P_MAP:
        cp = LSOF_PROTOCOL_MAP;
        break;
#endif /* defined(ETH_P_MAP) */

    default:
        cp = LSOF_PROTOCOL_UNKNOWN;
        break;
    }
    return cp;
}
