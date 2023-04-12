/*
 * print.c - common print support functions for lsof
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

#include "common.h"
#include "lsof.h"

/*
 * Local definitions, structures and function prototypes
 */

/*
 * endnm() - locate end of Namech
 */

char *endnm(struct lsof_context *ctx, size_t *sz) /* returned remaining size */
{
    register char *s;
    register size_t tsz;

    for (s = Namech, tsz = Namechl; *s; s++, tsz--)
        ;
    *sz = tsz;
    return (s);
}

/*
 * print_kptr() - print kernel pointer
 */

char *print_kptr(KA_T kp,     /* kernel pointer address */
                 char *buf,   /* optional destination buffer */
                 size_t bufl) /* size of buf[] */
{
    static char dbuf[32];

    (void)snpf(buf ? buf : dbuf, buf ? bufl : sizeof(dbuf), KA_T_FMT_X, kp);
    return (buf ? buf : dbuf);
}

/*
 * enter_ip_proto() - set iproto from IP protocol
 */

void enter_ip_proto(struct lsof_context *ctx, int p) /* protocol number */
{
    switch (p) {

#if defined(IPPROTO_TCP)
    case IPPROTO_TCP:
        Lf->iproto = LSOF_PROTOCOL_TCP;
        break;
#endif /* defined(IPPROTO_TCP) */

#if defined(IPPROTO_UDP)
    case IPPROTO_UDP:
        Lf->iproto = LSOF_PROTOCOL_UDP;
        break;
#endif /* defined(IPPROTO_UDP) */

#if defined(IPPROTO_IP)
#    if !defined(IPPROTO_HOPOPTS) || IPPROTO_IP != IPPROTO_HOPOPTS
    case IPPROTO_IP:
        Lf->iproto = LSOF_PROTOCOL_IP;
        break;
#    endif /* !defined(IPPROTO_HOPOPTS) || IPPROTO_IP!=IPPROTO_HOPOPTS */
#endif     /* defined(IPPROTO_IP) */

#if defined(IPPROTO_ICMP)
    case IPPROTO_ICMP:
        Lf->iproto = LSOF_PROTOCOL_ICMP;
        break;
#endif /* defined(IPPROTO_ICMP) */

#if defined(IPPROTO_ICMPV6)
    case IPPROTO_ICMPV6:
        Lf->iproto = LSOF_PROTOCOL_ICMPV6;
        break;
#endif /* defined(IPPROTO_ICMPV6) */

#if defined(IPPROTO_IGMP)
    case IPPROTO_IGMP:
        Lf->iproto = LSOF_PROTOCOL_IGMP;
        break;
#endif /* defined(IPPROTO_IGMP) */

#if defined(IPPROTO_GGP)
    case IPPROTO_GGP:
        Lf->iproto = LSOF_PROTOCOL_GGP;
        break;
#endif /* defined(IPPROTO_GGP) */

#if defined(IPPROTO_EGP)
    case IPPROTO_EGP:
        Lf->iproto = LSOF_PROTOCOL_EGP;
        break;
#endif /* defined(IPPROTO_EGP) */

#if defined(IPPROTO_PUP)
    case IPPROTO_PUP:
        Lf->iproto = LSOF_PROTOCOL_PUP;
        break;
#endif /* defined(IPPROTO_PUP) */

#if defined(IPPROTO_IDP)
    case IPPROTO_IDP:
        Lf->iproto = LSOF_PROTOCOL_IDP;
        break;
#endif /* defined(IPPROTO_IDP) */

#if defined(IPPROTO_ND)
    case IPPROTO_ND:
        Lf->iproto = LSOF_PROTOCOL_ND;
        break;
#endif /* defined(IPPROTO_ND) */

#if defined(IPPROTO_RAW)
    case IPPROTO_RAW:
        Lf->iproto = LSOF_PROTOCOL_RAW;
        break;
#endif /* defined(IPPROTO_RAW) */

#if defined(IPPROTO_HELLO)
    case IPPROTO_HELLO:
        Lf->iproto = LSOF_PROTOCOL_HELLO;
        break;
#endif /* defined(IPPROTO_HELLO) */

#if defined(IPPROTO_PXP)
    case IPPROTO_PXP:
        Lf->iproto = LSOF_PROTOCOL_PXP;
        break;
#endif /* defined(IPPROTO_PXP) */

#if defined(IPPROTO_RAWIP)
    case IPPROTO_RAWIP:
        Lf->iproto = LSOF_PROTOCOL_RAWIP;
        break;
#endif /* defined(IPPROTO_RAWIP) */

#if defined(IPPROTO_RAWIF)
    case IPPROTO_RAWIF:
        Lf->iproto = LSOF_PROTOCOL_RAWIF;
        break;
#endif /* defined(IPPROTO_RAWIF) */

#if defined(IPPROTO_HOPOPTS)
    case IPPROTO_HOPOPTS:
        Lf->iproto = LSOF_PROTOCOL_HOPOPTS;
        break;
#endif /* defined(IPPROTO_HOPOPTS) */

#if defined(IPPROTO_IPIP)
    case IPPROTO_IPIP:
        Lf->iproto = LSOF_PROTOCOL_IPIP;
        break;
#endif /* defined(IPPROTO_IPIP) */

#if defined(IPPROTO_ST)
    case IPPROTO_ST:
        Lf->iproto = LSOF_PROTOCOL_ST;
        break;
#endif /* defined(IPPROTO_ST) */

#if defined(IPPROTO_PIGP)
    case IPPROTO_PIGP:
        Lf->iproto = LSOF_PROTOCOL_PIGP;
        break;
#endif /* defined(IPPROTO_PIGP) */

#if defined(IPPROTO_RCCMON)
    case IPPROTO_RCCMON:
        Lf->iproto = LSOF_PROTOCOL_RCCMON;
        break;
#endif /* defined(IPPROTO_RCCMON) */

#if defined(IPPROTO_NVPII)
    case IPPROTO_NVPII:
        Lf->iproto = LSOF_PROTOCOL_NVPII;
        break;
#endif /* defined(IPPROTO_NVPII) */

#if defined(IPPROTO_ARGUS)
    case IPPROTO_ARGUS:
        Lf->iproto = LSOF_PROTOCOL_ARGUS;
        break;
#endif /* defined(IPPROTO_ARGUS) */

#if defined(IPPROTO_EMCON)
    case IPPROTO_EMCON:
        Lf->iproto = LSOF_PROTOCOL_EMCON;
        break;
#endif /* defined(IPPROTO_EMCON) */

#if defined(IPPROTO_XNET)
    case IPPROTO_XNET:
        Lf->iproto = LSOF_PROTOCOL_XNET;
        break;
#endif /* defined(IPPROTO_XNET) */

#if defined(IPPROTO_CHAOS)
    case IPPROTO_CHAOS:
        Lf->iproto = LSOF_PROTOCOL_CHAOS;
        break;
#endif /* defined(IPPROTO_CHAOS) */

#if defined(IPPROTO_MUX)
    case IPPROTO_MUX:
        Lf->iproto = LSOF_PROTOCOL_MUX;
        break;
#endif /* defined(IPPROTO_MUX) */

#if defined(IPPROTO_MEAS)
    case IPPROTO_MEAS:
        Lf->iproto = LSOF_PROTOCOL_MEAS;
        break;
#endif /* defined(IPPROTO_MEAS) */

#if defined(IPPROTO_HMP)
    case IPPROTO_HMP:
        Lf->iproto = LSOF_PROTOCOL_HMP;
        break;
#endif /* defined(IPPROTO_HMP) */

#if defined(IPPROTO_PRM)
    case IPPROTO_PRM:
        Lf->iproto = LSOF_PROTOCOL_PRM;
        break;
#endif /* defined(IPPROTO_PRM) */

#if defined(IPPROTO_TRUNK1)
    case IPPROTO_TRUNK1:
        Lf->iproto = LSOF_PROTOCOL_TRUNK1;
        break;
#endif /* defined(IPPROTO_TRUNK1) */

#if defined(IPPROTO_TRUNK2)
    case IPPROTO_TRUNK2:
        Lf->iproto = LSOF_PROTOCOL_TRUNK2;
        break;
#endif /* defined(IPPROTO_TRUNK2) */

#if defined(IPPROTO_LEAF1)
    case IPPROTO_LEAF1:
        Lf->iproto = LSOF_PROTOCOL_LEAF1;
        break;
#endif /* defined(IPPROTO_LEAF1) */

#if defined(IPPROTO_LEAF2)
    case IPPROTO_LEAF2:
        Lf->iproto = LSOF_PROTOCOL_LEAF2;
        break;
#endif /* defined(IPPROTO_LEAF2) */

#if defined(IPPROTO_RDP)
    case IPPROTO_RDP:
        Lf->iproto = LSOF_PROTOCOL_RDP;
        break;
#endif /* defined(IPPROTO_RDP) */

#if defined(IPPROTO_IRTP)
    case IPPROTO_IRTP:
        Lf->iproto = LSOF_PROTOCOL_IRTP;
        break;
#endif /* defined(IPPROTO_IRTP) */

#if defined(IPPROTO_TP)
    case IPPROTO_TP:
        Lf->iproto = LSOF_PROTOCOL_TP;
        break;
#endif /* defined(IPPROTO_TP) */

#if defined(IPPROTO_BLT)
    case IPPROTO_BLT:
        Lf->iproto = LSOF_PROTOCOL_BLT;
        break;
#endif /* defined(IPPROTO_BLT) */

#if defined(IPPROTO_NSP)
    case IPPROTO_NSP:
        Lf->iproto = LSOF_PROTOCOL_NSP;
        break;
#endif /* defined(IPPROTO_NSP) */

#if defined(IPPROTO_INP)
    case IPPROTO_INP:
        Lf->iproto = LSOF_PROTOCOL_INP;
        break;
#endif /* defined(IPPROTO_INP) */

#if defined(IPPROTO_SEP)
    case IPPROTO_SEP:
        Lf->iproto = LSOF_PROTOCOL_SEP;
        break;
#endif /* defined(IPPROTO_SEP) */

#if defined(IPPROTO_3PC)
    case IPPROTO_3PC:
        Lf->iproto = LSOF_PROTOCOL_3PC;
        break;
#endif /* defined(IPPROTO_3PC) */

#if defined(IPPROTO_IDPR)
    case IPPROTO_IDPR:
        Lf->iproto = LSOF_PROTOCOL_IDPR;
        break;
#endif /* defined(IPPROTO_IDPR) */

#if defined(IPPROTO_XTP)
    case IPPROTO_XTP:
        Lf->iproto = LSOF_PROTOCOL_XTP;
        break;
#endif /* defined(IPPROTO_XTP) */

#if defined(IPPROTO_DDP)
    case IPPROTO_DDP:
        Lf->iproto = LSOF_PROTOCOL_DDP;
        break;
#endif /* defined(IPPROTO_DDP) */

#if defined(IPPROTO_CMTP)
    case IPPROTO_CMTP:
        Lf->iproto = LSOF_PROTOCOL_CMTP;
        break;
#endif /* defined(IPPROTO_CMTP) */

#if defined(IPPROTO_TPXX)
    case IPPROTO_TPXX:
        Lf->iproto = LSOF_PROTOCOL_TPXX;
        break;
#endif /* defined(IPPROTO_TPXX) */

#if defined(IPPROTO_IL)
    case IPPROTO_IL:
        Lf->iproto = LSOF_PROTOCOL_IL;
        break;
#endif /* defined(IPPROTO_IL) */

#if defined(IPPROTO_IPV6)
    case IPPROTO_IPV6:
        Lf->iproto = LSOF_PROTOCOL_IPV6;
        break;
#endif /* defined(IPPROTO_IPV6) */

#if defined(IPPROTO_SDRP)
    case IPPROTO_SDRP:
        Lf->iproto = LSOF_PROTOCOL_SDRP;
        break;
#endif /* defined(IPPROTO_SDRP) */

#if defined(IPPROTO_ROUTING)
    case IPPROTO_ROUTING:
        Lf->iproto = LSOF_PROTOCOL_ROUTING;
        break;
#endif /* defined(IPPROTO_ROUTING) */

#if defined(IPPROTO_FRAGMENT)
    case IPPROTO_FRAGMENT:
        Lf->iproto = LSOF_PROTOCOL_FRAGMENT;
        break;
#endif /* defined(IPPROTO_FRAGMENT) */

#if defined(IPPROTO_IDRP)
    case IPPROTO_IDRP:
        Lf->iproto = LSOF_PROTOCOL_IDRP;
        break;
#endif /* defined(IPPROTO_IDRP) */

#if defined(IPPROTO_RSVP)
    case IPPROTO_RSVP:
        Lf->iproto = LSOF_PROTOCOL_RSVP;
        break;
#endif /* defined(IPPROTO_RSVP) */

#if defined(IPPROTO_GRE)
    case IPPROTO_GRE:
        Lf->iproto = LSOF_PROTOCOL_GRE;
        break;
#endif /* defined(IPPROTO_GRE) */

#if defined(IPPROTO_MHRP)
    case IPPROTO_MHRP:
        Lf->iproto = LSOF_PROTOCOL_MHRP;
        break;
#endif /* defined(IPPROTO_MHRP) */

#if defined(IPPROTO_BHA)
    case IPPROTO_BHA:
        Lf->iproto = LSOF_PROTOCOL_BHA;
        break;
#endif /* defined(IPPROTO_BHA) */

#if defined(IPPROTO_ESP)
    case IPPROTO_ESP:
        Lf->iproto = LSOF_PROTOCOL_ESP;
        break;
#endif /* defined(IPPROTO_ESP) */

#if defined(IPPROTO_AH)
    case IPPROTO_AH:
        Lf->iproto = LSOF_PROTOCOL_AH;
        break;
#endif /* defined(IPPROTO_AH) */

#if defined(IPPROTO_INLSP)
    case IPPROTO_INLSP:
        Lf->iproto = LSOF_PROTOCOL_INLSP;
        break;
#endif /* defined(IPPROTO_INLSP) */

#if defined(IPPROTO_SWIPE)
    case IPPROTO_SWIPE:
        Lf->iproto = LSOF_PROTOCOL_SWIPE;
        break;
#endif /* defined(IPPROTO_SWIPE) */

#if defined(IPPROTO_NHRP)
    case IPPROTO_NHRP:
        Lf->iproto = LSOF_PROTOCOL_NHRP;
        break;
#endif /* defined(IPPROTO_NHRP) */

#if defined(IPPROTO_NONE)
    case IPPROTO_NONE:
        Lf->iproto = LSOF_PROTOCOL_NONE;
        break;
#endif /* defined(IPPROTO_NONE) */

#if defined(IPPROTO_DSTOPTS)
    case IPPROTO_DSTOPTS:
        Lf->iproto = LSOF_PROTOCOL_DSTOPTS;
        break;
#endif /* defined(IPPROTO_DSTOPTS) */

#if defined(IPPROTO_AHIP)
    case IPPROTO_AHIP:
        Lf->iproto = LSOF_PROTOCOL_AHIP;
        break;
#endif /* defined(IPPROTO_AHIP) */

#if defined(IPPROTO_CFTP)
    case IPPROTO_CFTP:
        Lf->iproto = LSOF_PROTOCOL_CFTP;
        break;
#endif /* defined(IPPROTO_CFTP) */

#if defined(IPPROTO_SATEXPAK)
    case IPPROTO_SATEXPAK:
        Lf->iproto = LSOF_PROTOCOL_SATEXPAK;
        break;
#endif /* defined(IPPROTO_SATEXPAK) */

#if defined(IPPROTO_KRYPTOLAN)
    case IPPROTO_KRYPTOLAN:
        Lf->iproto = LSOF_PROTOCOL_KRYPTOLAN;
        break;
#endif /* defined(IPPROTO_KRYPTOLAN) */

#if defined(IPPROTO_RVD)
    case IPPROTO_RVD:
        Lf->iproto = LSOF_PROTOCOL_RVD;
        break;
#endif /* defined(IPPROTO_RVD) */

#if defined(IPPROTO_IPPC)
    case IPPROTO_IPPC:
        Lf->iproto = LSOF_PROTOCOL_IPPC;
        break;
#endif /* defined(IPPROTO_IPPC) */

#if defined(IPPROTO_ADFS)
    case IPPROTO_ADFS:
        Lf->iproto = LSOF_PROTOCOL_ADFS;
        break;
#endif /* defined(IPPROTO_ADFS) */

#if defined(IPPROTO_SATMON)
    case IPPROTO_SATMON:
        Lf->iproto = LSOF_PROTOCOL_SATMON;
        break;
#endif /* defined(IPPROTO_SATMON) */

#if defined(IPPROTO_VISA)
    case IPPROTO_VISA:
        Lf->iproto = LSOF_PROTOCOL_VISA;
        break;
#endif /* defined(IPPROTO_VISA) */

#if defined(IPPROTO_IPCV)
    case IPPROTO_IPCV:
        Lf->iproto = LSOF_PROTOCOL_IPCV;
        break;
#endif /* defined(IPPROTO_IPCV) */

#if defined(IPPROTO_CPNX)
    case IPPROTO_CPNX:
        Lf->iproto = LSOF_PROTOCOL_CPNX;
        break;
#endif /* defined(IPPROTO_CPNX) */

#if defined(IPPROTO_CPHB)
    case IPPROTO_CPHB:
        Lf->iproto = LSOF_PROTOCOL_CPHB;
        break;
#endif /* defined(IPPROTO_CPHB) */

#if defined(IPPROTO_WSN)
    case IPPROTO_WSN:
        Lf->iproto = LSOF_PROTOCOL_WSN;
        break;
#endif /* defined(IPPROTO_WSN) */

#if defined(IPPROTO_PVP)
    case IPPROTO_PVP:
        Lf->iproto = LSOF_PROTOCOL_PVP;
        break;
#endif /* defined(IPPROTO_PVP) */

#if defined(IPPROTO_BRSATMON)
    case IPPROTO_BRSATMON:
        Lf->iproto = LSOF_PROTOCOL_BRSATMON;
        break;
#endif /* defined(IPPROTO_BRSATMON) */

#if defined(IPPROTO_WBMON)
    case IPPROTO_WBMON:
        Lf->iproto = LSOF_PROTOCOL_WBMON;
        break;
#endif /* defined(IPPROTO_WBMON) */

#if defined(IPPROTO_WBEXPAK)
    case IPPROTO_WBEXPAK:
        Lf->iproto = LSOF_PROTOCOL_WBEXPAK;
        break;
#endif /* defined(IPPROTO_WBEXPAK) */

#if defined(IPPROTO_EON)
    case IPPROTO_EON:
        Lf->iproto = LSOF_PROTOCOL_EON;
        break;
#endif /* defined(IPPROTO_EON) */

#if defined(IPPROTO_VMTP)
    case IPPROTO_VMTP:
        Lf->iproto = LSOF_PROTOCOL_VMTP;
        break;
#endif /* defined(IPPROTO_VMTP) */

#if defined(IPPROTO_SVMTP)
    case IPPROTO_SVMTP:
        Lf->iproto = LSOF_PROTOCOL_SVMTP;
        break;
#endif /* defined(IPPROTO_SVMTP) */

#if defined(IPPROTO_VINES)
    case IPPROTO_VINES:
        Lf->iproto = LSOF_PROTOCOL_VINES;
        break;
#endif /* defined(IPPROTO_VINES) */

#if defined(IPPROTO_TTP)
    case IPPROTO_TTP:
        Lf->iproto = LSOF_PROTOCOL_TTP;
        break;
#endif /* defined(IPPROTO_TTP) */

#if defined(IPPROTO_IGP)
    case IPPROTO_IGP:
        Lf->iproto = LSOF_PROTOCOL_IGP;
        break;
#endif /* defined(IPPROTO_IGP) */

#if defined(IPPROTO_DGP)
    case IPPROTO_DGP:
        Lf->iproto = LSOF_PROTOCOL_DGP;
        break;
#endif /* defined(IPPROTO_DGP) */

#if defined(IPPROTO_TCF)
    case IPPROTO_TCF:
        Lf->iproto = LSOF_PROTOCOL_TCF;
        break;
#endif /* defined(IPPROTO_TCF) */

#if defined(IPPROTO_IGRP)
    case IPPROTO_IGRP:
        Lf->iproto = LSOF_PROTOCOL_IGRP;
        break;
#endif /* defined(IPPROTO_IGRP) */

#if defined(IPPROTO_OSPFIGP)
    case IPPROTO_OSPFIGP:
        Lf->iproto = LSOF_PROTOCOL_OSPFIGP;
        break;
#endif /* defined(IPPROTO_OSPFIGP) */

#if defined(IPPROTO_SRPC)
    case IPPROTO_SRPC:
        Lf->iproto = LSOF_PROTOCOL_SRPC;
        break;
#endif /* defined(IPPROTO_SRPC) */

#if defined(IPPROTO_LARP)
    case IPPROTO_LARP:
        Lf->iproto = LSOF_PROTOCOL_LARP;
        break;
#endif /* defined(IPPROTO_LARP) */

#if defined(IPPROTO_MTP)
    case IPPROTO_MTP:
        Lf->iproto = LSOF_PROTOCOL_MTP;
        break;
#endif /* defined(IPPROTO_MTP) */

#if defined(IPPROTO_AX25)
    case IPPROTO_AX25:
        Lf->iproto = LSOF_PROTOCOL_AX25;
        break;
#endif /* defined(IPPROTO_AX25) */

#if defined(IPPROTO_IPEIP)
    case IPPROTO_IPEIP:
        Lf->iproto = LSOF_PROTOCOL_IPEIP;
        break;
#endif /* defined(IPPROTO_IPEIP) */

#if defined(IPPROTO_MICP)
    case IPPROTO_MICP:
        Lf->iproto = LSOF_PROTOCOL_MICP;
        break;
#endif /* defined(IPPROTO_MICP) */

#if defined(IPPROTO_SCCSP)
    case IPPROTO_SCCSP:
        Lf->iproto = LSOF_PROTOCOL_SCCSP;
        break;
#endif /* defined(IPPROTO_SCCSP) */

#if defined(IPPROTO_ETHERIP)
    case IPPROTO_ETHERIP:
        Lf->iproto = LSOF_PROTOCOL_ETHERIP;
        break;
#endif /* defined(IPPROTO_ETHERIP) */

#if defined(IPPROTO_ENCAP)
#    if !defined(IPPROTO_IPIP) || IPPROTO_IPIP != IPPROTO_ENCAP
    case IPPROTO_ENCAP:
        Lf->iproto = LSOF_PROTOCOL_ENCAP;
        break;
#    endif /* !defined(IPPROTO_IPIP) || IPPROTO_IPIP!=IPPROTO_ENCAP */
#endif     /* defined(IPPROTO_ENCAP) */

#if defined(IPPROTO_APES)
    case IPPROTO_APES:
        Lf->iproto = LSOF_PROTOCOL_APES;
        break;
#endif /* defined(IPPROTO_APES) */

#if defined(IPPROTO_GMTP)
    case IPPROTO_GMTP:
        Lf->iproto = LSOF_PROTOCOL_GMTP;
        break;
#endif /* defined(IPPROTO_GMTP) */

    default:
        Lf->iproto = LSOF_PROTOCOL_UNKNOWN;
        Lf->unknown_proto_number = p;
        break;
    }
}

/*
 * printunkaf() - print unknown address family
 */

void printunkaf(struct lsof_context *ctx, int fam, /* unknown address family */
                int ty) /* output type: 0 = terse; 1 = full */
{
    char *p, *s;

    p = "";
    switch (fam) {

#if defined(AF_UNSPEC)
    case AF_UNSPEC:
        s = "UNSPEC";
        break;
#endif /* defined(AF_UNSPEC) */

#if defined(AF_UNIX)
    case AF_UNIX:
        s = "UNIX";
        break;
#endif /* defined(AF_UNIX) */

#if defined(AF_INET)
    case AF_INET:
        s = "INET";
        break;
#endif /* defined(AF_INET) */

#if defined(AF_INET6)
    case AF_INET6:
        s = "INET6";
        break;
#endif /* defined(AF_INET6) */

#if defined(AF_IMPLINK)
    case AF_IMPLINK:
        s = "IMPLINK";
        break;
#endif /* defined(AF_IMPLINK) */

#if defined(AF_PUP)
    case AF_PUP:
        s = "PUP";
        break;
#endif /* defined(AF_PUP) */

#if defined(AF_CHAOS)
    case AF_CHAOS:
        s = "CHAOS";
        break;
#endif /* defined(AF_CHAOS) */

#if defined(AF_NS)
    case AF_NS:
        s = "NS";
        break;
#endif /* defined(AF_NS) */

#if defined(AF_ISO)
    case AF_ISO:
        s = "ISO";
        break;
#endif /* defined(AF_ISO) */

#if defined(AF_NBS)
#    if !defined(AF_ISO) || AF_NBS != AF_ISO
    case AF_NBS:
        s = "NBS";
        break;
#    endif /* !defined(AF_ISO) || AF_NBS!=AF_ISO */
#endif     /* defined(AF_NBS) */

#if defined(AF_ECMA)
    case AF_ECMA:
        s = "ECMA";
        break;
#endif /* defined(AF_ECMA) */

#if defined(AF_DATAKIT)
    case AF_DATAKIT:
        s = "DATAKIT";
        break;
#endif /* defined(AF_DATAKIT) */

#if defined(AF_CCITT)
    case AF_CCITT:
        s = "CCITT";
        break;
#endif /* defined(AF_CCITT) */

#if defined(AF_SNA)
    case AF_SNA:
        s = "SNA";
        break;
#endif /* defined(AF_SNA) */

#if defined(AF_DECnet)
    case AF_DECnet:
        s = "DECnet";
        break;
#endif /* defined(AF_DECnet) */

#if defined(AF_DLI)
    case AF_DLI:
        s = "DLI";
        break;
#endif /* defined(AF_DLI) */

#if defined(AF_LAT)
    case AF_LAT:
        s = "LAT";
        break;
#endif /* defined(AF_LAT) */

#if defined(AF_HYLINK)
    case AF_HYLINK:
        s = "HYLINK";
        break;
#endif /* defined(AF_HYLINK) */

#if defined(AF_APPLETALK)
    case AF_APPLETALK:
        s = "APPLETALK";
        break;
#endif /* defined(AF_APPLETALK) */

#if defined(AF_BSC)
    case AF_BSC:
        s = "BSC";
        break;
#endif /* defined(AF_BSC) */

#if defined(AF_DSS)
    case AF_DSS:
        s = "DSS";
        break;
#endif /* defined(AF_DSS) */

#if defined(AF_ROUTE)
    case AF_ROUTE:
        s = "ROUTE";
        break;
#endif /* defined(AF_ROUTE) */

#if defined(AF_RAW)
    case AF_RAW:
        s = "RAW";
        break;
#endif /* defined(AF_RAW) */

#if defined(AF_LINK)
    case AF_LINK:
        s = "LINK";
        break;
#endif /* defined(AF_LINK) */

#if defined(pseudo_AF_XTP)
    case pseudo_AF_XTP:
        p = "pseudo_";
        s = "XTP";
        break;
#endif /* defined(pseudo_AF_XTP) */

#if defined(AF_RMP)
    case AF_RMP:
        s = "RMP";
        break;
#endif /* defined(AF_RMP) */

#if defined(AF_COIP)
    case AF_COIP:
        s = "COIP";
        break;
#endif /* defined(AF_COIP) */

#if defined(AF_CNT)
    case AF_CNT:
        s = "CNT";
        break;
#endif /* defined(AF_CNT) */

#if defined(pseudo_AF_RTIP)
    case pseudo_AF_RTIP:
        p = "pseudo_";
        s = "RTIP";
        break;
#endif /* defined(pseudo_AF_RTIP) */

#if defined(AF_NETMAN)
    case AF_NETMAN:
        s = "NETMAN";
        break;
#endif /* defined(AF_NETMAN) */

#if defined(AF_INTF)
    case AF_INTF:
        s = "INTF";
        break;
#endif /* defined(AF_INTF) */

#if defined(AF_NETWARE)
    case AF_NETWARE:
        s = "NETWARE";
        break;
#endif /* defined(AF_NETWARE) */

#if defined(AF_NDD)
    case AF_NDD:
        s = "NDD";
        break;
#endif /* defined(AF_NDD) */

#if defined(AF_NIT)
#    if !defined(AF_ROUTE) || AF_ROUTE != AF_NIT
    case AF_NIT:
        s = "NIT";
        break;
#    endif /* !defined(AF_ROUTE) || AF_ROUTE!=AF_NIT */
#endif     /* defined(AF_NIT) */

#if defined(AF_802)
#    if !defined(AF_RAW) || AF_RAW != AF_802
    case AF_802:
        s = "802";
        break;
#    endif /* !defined(AF_RAW) || AF_RAW!=AF_802 */
#endif     /* defined(AF_802) */

#if defined(AF_X25)
    case AF_X25:
        s = "X25";
        break;
#endif /* defined(AF_X25) */

#if defined(AF_CTF)
    case AF_CTF:
        s = "CTF";
        break;
#endif /* defined(AF_CTF) */

#if defined(AF_WAN)
    case AF_WAN:
        s = "WAN";
        break;
#endif /* defined(AF_WAN) */

#if defined(AF_OSINET)
#    if defined(AF_INET) && AF_INET != AF_OSINET
    case AF_OSINET:
        s = "OSINET";
        break;
#    endif /* defined(AF_INET) && AF_INET!=AF_OSINET */
#endif     /* defined(AF_OSINET) */

#if defined(AF_GOSIP)
    case AF_GOSIP:
        s = "GOSIP";
        break;
#endif /* defined(AF_GOSIP) */

#if defined(AF_SDL)
    case AF_SDL:
        s = "SDL";
        break;
#endif /* defined(AF_SDL) */

#if defined(AF_IPX)
    case AF_IPX:
        s = "IPX";
        break;
#endif /* defined(AF_IPX) */

#if defined(AF_SIP)
    case AF_SIP:
        s = "SIP";
        break;
#endif /* defined(AF_SIP) */

#if defined(psuedo_AF_PIP)
    case psuedo_AF_PIP:
        p = "pseudo_";
        s = "PIP";
        break;
#endif /* defined(psuedo_AF_PIP) */

#if defined(AF_OTS)
    case AF_OTS:
        s = "OTS";
        break;
#endif /* defined(AF_OTS) */

#if defined(pseudo_AF_BLUE)
    case pseudo_AF_BLUE: /* packets for Blue box */
        p = "pseudo_";
        s = "BLUE";
        break;
#endif /* defined(pseudo_AF_BLUE) */

#if defined(AF_NDRV) /* network driver raw access */
    case AF_NDRV:
        s = "NDRV";
        break;
#endif /* defined(AF_NDRV) */

#if defined(AF_SYSTEM) /* kernel event messages */
    case AF_SYSTEM:
        s = "SYSTEM";
        break;
#endif /* defined(AF_SYSTEM) */

#if defined(AF_USER)
    case AF_USER:
        s = "USER";
        break;
#endif /* defined(AF_USER) */

#if defined(pseudo_AF_KEY)
    case pseudo_AF_KEY:
        p = "pseudo_";
        s = "KEY";
        break;
#endif /* defined(pseudo_AF_KEY) */

#if defined(AF_KEY) /* Security Association DB socket */
    case AF_KEY:
        s = "KEY";
        break;
#endif /* defined(AF_KEY) */

#if defined(AF_NCA) /* NCA socket */
    case AF_NCA:
        s = "NCA";
        break;
#endif /* defined(AF_NCA) */

#if defined(AF_POLICY) /* Security Policy DB socket */
    case AF_POLICY:
        s = "POLICY";
        break;
#endif /* defined(AF_POLICY) */

#if defined(AF_PPP) /* PPP socket */
    case AF_PPP:
        s = "PPP";
        break;
#endif /* defined(AF_PPP) */

    default:
        if (!ty)
            (void)snpf(Namech, Namechl, "%#x", fam);
        else
            (void)snpf(Namech, Namechl, "no further information on family %#x",
                       fam);
        return;
    }
    if (!ty)
        (void)snpf(Namech, Namechl, "%sAF_%s", p, s);
    else
        (void)snpf(Namech, Namechl, "no further information on %sAF_%s", p, s);
    return;
}

/*
 * print_access() - print enum lsof_file_access_mode
 */
char print_access(enum lsof_file_access_mode access) {
    switch (access) {
    default:
    case LSOF_FILE_ACCESS_NONE:
        return ' ';
    case LSOF_FILE_ACCESS_READ:
        return 'r';
    case LSOF_FILE_ACCESS_WRITE:
        return 'w';
    case LSOF_FILE_ACCESS_READ_WRITE:
        return 'u';
    }
}

/*
 * print_lock() - print enum lsof_lock_mode
 */
char print_lock(enum lsof_lock_mode lock) {
    switch (lock) {
    default:
    case LSOF_LOCK_NONE:
        return ' ';
    case LSOF_LOCK_UNKNOWN:
        return 'U';
    case LSOF_LOCK_READ_PARTIAL:
        return 'r';
    case LSOF_LOCK_READ_FULL:
        return 'R';
    case LSOF_LOCK_WRITE_PARTIAL:
        return 'w';
    case LSOF_LOCK_WRITE_FULL:
        return 'W';
    case LSOF_LOCK_READ_WRITE:
        return 'u';
    case LSOF_LOCK_SOLARIS_NFS:
        return 'N';
    case LSOF_LOCK_SCO_PARTIAL:
        return 'x';
    case LSOF_LOCK_SCO_FULL:
        return 'X';
    }
}

/*
 * print_file_type() - print enum lsof_file_type
 */
void print_file_type(enum lsof_file_type type,
                     uint32_t unknown_file_type_number, char *buf,
                     size_t buf_len) {
    switch (type) {
    default:
    case LSOF_FILE_UNKNOWN:
        (void)snpf(buf, buf_len, "%04o", (unknown_file_type_number & 0xfff));
        break;
    case LSOF_FILE_FIFO:
        (void)snpf(buf, buf_len, "FIFO");
        break;
    case LSOF_FILE_CHAR:
        (void)snpf(buf, buf_len, "CHR");
        break;
    case LSOF_FILE_DIR:
        (void)snpf(buf, buf_len, "DIR");
        break;
    case LSOF_FILE_BLOCK:
        (void)snpf(buf, buf_len, "BLK");
        break;
    case LSOF_FILE_REGULAR:
        (void)snpf(buf, buf_len, "REG");
        break;
    case LSOF_FILE_LINK:
        (void)snpf(buf, buf_len, "LNK");
        break;
    /* Use lower case for network-related files except IPv4/IPv6 for
     * compatibility */
    case LSOF_FILE_SOCKET:
        (void)snpf(buf, buf_len, "sock");
        break;
    case LSOF_FILE_IPV4:
        (void)snpf(buf, buf_len, "IPv4");
        break;
    case LSOF_FILE_IPV6:
        (void)snpf(buf, buf_len, "IPv6");
        break;
    case LSOF_FILE_AX25:
        (void)snpf(buf, buf_len, "ax25");
        break;
    case LSOF_FILE_INET:
        (void)snpf(buf, buf_len, "inet");
        break;
    case LSOF_FILE_LINK_LEVEL_ACCESS:
        (void)snpf(buf, buf_len, "lla");
        break;
    case LSOF_FILE_ROUTE:
        (void)snpf(buf, buf_len, "rte");
        break;
    case LSOF_FILE_UNIX:
        (void)snpf(buf, buf_len, "unix");
        break;
    case LSOF_FILE_X25:
        (void)snpf(buf, buf_len, "x.25");
        break;
    case LSOF_FILE_APPLETALK:
        (void)snpf(buf, buf_len, "atalk");
        break;
    case LSOF_FILE_NET_DRIVER:
        (void)snpf(buf, buf_len, "ndrv");
        break;
    case LSOF_FILE_INTERNAL_KEY:
        (void)snpf(buf, buf_len, "key");
        break;
    case LSOF_FILE_SYSTEM:
        (void)snpf(buf, buf_len, "systm");
        break;
    case LSOF_FILE_PPP:
        (void)snpf(buf, buf_len, "ppp");
        break;
    case LSOF_FILE_IPX:
        (void)snpf(buf, buf_len, "ipx");
        break;
    case LSOF_FILE_RAW:
        (void)snpf(buf, buf_len, "raw");
        break;
    case LSOF_FILE_RAW6:
        (void)snpf(buf, buf_len, "raw6");
        break;
    case LSOF_FILE_NETLINK:
        (void)snpf(buf, buf_len, "netlink");
        break;
    case LSOF_FILE_PACKET:
        (void)snpf(buf, buf_len, "pack");
        break;
    case LSOF_FILE_ICMP:
        (void)snpf(buf, buf_len, "icmp");
        break;

    case LSOF_FILE_PROC_AS:
        (void)snpf(buf, buf_len, "PAS");
        break;
    case LSOF_FILE_PROC_AUXV:
        (void)snpf(buf, buf_len, "PAXV");
        break;
    case LSOF_FILE_PROC_CRED:
        (void)snpf(buf, buf_len, "PCRE");
        break;
    case LSOF_FILE_PROC_CTRL:
        (void)snpf(buf, buf_len, "PCTL");
        break;
    case LSOF_FILE_PROC_CUR_PROC:
        (void)snpf(buf, buf_len, "PCUR");
        break;
    case LSOF_FILE_PROC_CWD:
        (void)snpf(buf, buf_len, "PCWD");
        break;
    case LSOF_FILE_PROC_DIR:
        (void)snpf(buf, buf_len, "PDIR");
        break;
    case LSOF_FILE_PROC_PID:
        (void)snpf(buf, buf_len, "PPID");
        break;
    case LSOF_FILE_PROC_EXEC_TYPE:
        (void)snpf(buf, buf_len, "PETY");
        break;
    case LSOF_FILE_PROC_FD:
        (void)snpf(buf, buf_len, "PFD");
        break;
    case LSOF_FILE_PROC_FD_DIR:
        (void)snpf(buf, buf_len, "PFDR");
        break;
    case LSOF_FILE_PROC_FILE:
        (void)snpf(buf, buf_len, "PFIL");
        break;
    case LSOF_FILE_PROC_FP_REGS:
        (void)snpf(buf, buf_len, "PFPR");
        break;
    case LSOF_FILE_PROC_PAGE_DATA:
        (void)snpf(buf, buf_len, "PGD");
        break;
    case LSOF_FILE_PROC_GROUP_NOTIFIER:
        (void)snpf(buf, buf_len, "PGID");
        break;
    case LSOF_FILE_PROC_LDT:
        (void)snpf(buf, buf_len, "PLDT");
        break;
    case LSOF_FILE_PROC_LPS_INFO:
        (void)snpf(buf, buf_len, "PLPI");
        break;
    case LSOF_FILE_PROC_LSTATUS:
        (void)snpf(buf, buf_len, "PLST");
        break;
    case LSOF_FILE_PROC_LUSAGE:
        (void)snpf(buf, buf_len, "PLU");
        break;
    case LSOF_FILE_PROC_LWP_GWINDOWS:
        (void)snpf(buf, buf_len, "PLWG");
        break;
    case LSOF_FILE_PROC_LWP_CTL:
        (void)snpf(buf, buf_len, "PLC");
        break;
    case LSOF_FILE_PROC_LWP_DIR:
        (void)snpf(buf, buf_len, "PLDR");
        break;
    case LSOF_FILE_PROC_LWP_SINFO:
        (void)snpf(buf, buf_len, "PLWI");
        break;
    case LSOF_FILE_PROC_LWP_STATUS:
        (void)snpf(buf, buf_len, "PLWS");
        break;
    case LSOF_FILE_PROC_LWP_USAGE:
        (void)snpf(buf, buf_len, "PLWU");
        break;
    case LSOF_FILE_PROC_LWP_XREGS:
        (void)snpf(buf, buf_len, "PLWX");
        break;
    case LSOF_FILE_PROC_MAP:
        (void)snpf(buf, buf_len, "PMAP");
        break;
    case LSOF_FILE_PROC_MAPS:
        (void)snpf(buf, buf_len, "PMPS");
        break;
    case LSOF_FILE_PROC_MEMORY:
        (void)snpf(buf, buf_len, "PMEM");
        break;
    case LSOF_FILE_PROC_PROC_NOTIFIER:
        (void)snpf(buf, buf_len, "PNTF");
        break;
    case LSOF_FILE_PROC_OBJ:
        (void)snpf(buf, buf_len, "POBJ");
        break;
    case LSOF_FILE_PROC_OBJ_DIR:
        (void)snpf(buf, buf_len, "PODR");
        break;
    case LSOF_FILE_PROC_OLD_LWP:
        (void)snpf(buf, buf_len, "POLP");
        break;
    case LSOF_FILE_PROC_OLD_PID:
        (void)snpf(buf, buf_len, "POPF");
        break;
    case LSOF_FILE_PROC_OLD_PAGE:
        (void)snpf(buf, buf_len, "POPG");
        break;
    case LSOF_FILE_PROC_REGS:
        (void)snpf(buf, buf_len, "PREG");
        break;
    case LSOF_FILE_PROC_RMAP:
        (void)snpf(buf, buf_len, "PRMP");
        break;
    case LSOF_FILE_PROC_ROOT:
        (void)snpf(buf, buf_len, "PRTD");
        break;
    case LSOF_FILE_PROC_SIGACT:
        (void)snpf(buf, buf_len, "PSGA");
        break;
    case LSOF_FILE_PROC_PSINFO:
        (void)snpf(buf, buf_len, "PSIN");
        break;
    case LSOF_FILE_PROC_STATUS:
        (void)snpf(buf, buf_len, "PSTA");
        break;
    case LSOF_FILE_PROC_USAGE:
        (void)snpf(buf, buf_len, "PUSG");
        break;
    case LSOF_FILE_PROC_WATCH:
        (void)snpf(buf, buf_len, "PW");
        break;
    case LSOF_FILE_PROC_XMAP:
        (void)snpf(buf, buf_len, "PXMP");
        break;

    /* Others */
    case LSOF_FILE_ANON_INODE:
        (void)snpf(buf, buf_len, "a_inode");
        break;
    case LSOF_FILE_DEL:
        (void)snpf(buf, buf_len, "DEL");
        break;
    case LSOF_FILE_DOOR:
        (void)snpf(buf, buf_len, "DOOR");
        break;
    case LSOF_FILE_KQUEUE:
        (void)snpf(buf, buf_len, "KQUEUE");
        break;
    case LSOF_FILE_FSEVENTS:
        (void)snpf(buf, buf_len, "FSEVENTS");
        break;
    case LSOF_FILE_EVENTFD:
        (void)snpf(buf, buf_len, "EVENTFD");
        break;
    case LSOF_FILE_PROCDESC:
        (void)snpf(buf, buf_len, "PROCDSC");
        break;
    case LSOF_FILE_MULTIPLEXED_BLOCK:
        (void)snpf(buf, buf_len, "MPB");
        break;
    case LSOF_FILE_MULTIPLEXED_CHAR:
        (void)snpf(buf, buf_len, "MPC");
        break;
    case LSOF_FILE_UNKNOWN_DELETED:
        (void)snpf(buf, buf_len, "UNKNdel");
        break;
    case LSOF_FILE_UNKNOWN_MEMORY:
        (void)snpf(buf, buf_len, "UNKNmem");
        break;
    case LSOF_FILE_UNKNOWN_FD:
        (void)snpf(buf, buf_len, "UNKNfd");
        break;
    case LSOF_FILE_UNKNOWN_CWD:
        (void)snpf(buf, buf_len, "UNKNcwd");
        break;
    case LSOF_FILE_UNKNOWN_ROOT_DIR:
        (void)snpf(buf, buf_len, "UNKNrtd");
        break;
    case LSOF_FILE_UNKNOWN_PROGRAM_TEXT:
        (void)snpf(buf, buf_len, "UNKNtxt");
        break;
    case LSOF_FILE_PIPE:
        (void)snpf(buf, buf_len, "PIPE");
        break;
    case LSOF_FILE_PORT:
        (void)snpf(buf, buf_len, "PORT");
        break;
    case LSOF_FILE_POSIX_MQ:
        (void)snpf(buf, buf_len, "PSXMQ");
        break;
    case LSOF_FILE_POSIX_SEMA:
        (void)snpf(buf, buf_len, "PSXSEM");
        break;
    case LSOF_FILE_POSIX_SHM:
        (void)snpf(buf, buf_len, "PSXSHM");
        break;
    case LSOF_FILE_SHM:
        (void)snpf(buf, buf_len, "SHM");
        break;
    case LSOF_FILE_PTS:
        (void)snpf(buf, buf_len, "PTS");
        break;
    case LSOF_FILE_SHARED_MEM_TRANSPORT:
        (void)snpf(buf, buf_len, "SMT");
        break;
    case LSOF_FILE_STREAM:
        (void)snpf(buf, buf_len, "STR");
        break;
    case LSOF_FILE_STREAM_SOCKET:
        (void)snpf(buf, buf_len, "STSO");
        break;
    case LSOF_FILE_SCO_UNKNOWN:
        (void)snpf(buf, buf_len, "XNAM");
        break;
    case LSOF_FILE_SCO_SEMA:
        (void)snpf(buf, buf_len, "XSEM");
        break;
    case LSOF_FILE_SCO_SHARED:
        (void)snpf(buf, buf_len, "XSD");
        break;
    case LSOF_FILE_UNSUPPORTED:
        (void)snpf(buf, buf_len, "UNSP");
        break;

    /* vnode */
    case LSOF_FILE_VNODE_VNON:
        (void)snpf(buf, buf_len, "VNON");
        break;
    case LSOF_FILE_VNODE_VREG:
        (void)snpf(buf, buf_len, "VREG");
        break;
    case LSOF_FILE_VNODE_VDIR:
        (void)snpf(buf, buf_len, "VDIR");
        break;
    case LSOF_FILE_VNODE_VBLK:
        (void)snpf(buf, buf_len, "VBLK");
        break;
    case LSOF_FILE_VNODE_VCHR:
        (void)snpf(buf, buf_len, "VCHR");
        break;
    case LSOF_FILE_VNODE_VLNK:
        (void)snpf(buf, buf_len, "VLNK");
        break;
    case LSOF_FILE_VNODE_VSOCK:
        (void)snpf(buf, buf_len, "VSOCK");
        break;
    case LSOF_FILE_VNODE_VFIFO:
        (void)snpf(buf, buf_len, "VFIFO");
        break;
    case LSOF_FILE_VNODE_VBAD:
        (void)snpf(buf, buf_len, "VBAD");
        break;
    case LSOF_FILE_VNODE_VMPC:
        (void)snpf(buf, buf_len, "VMPC");
        break;
    case LSOF_FILE_VNODE_VUNNAMED:
        (void)snpf(buf, buf_len, "VUNNM");
        break;
    }
}

/*
 * print_file_type() - print enum lsof_protocol
 */
void print_iproto(enum lsof_protocol proto, uint32_t unknown_proto_number,
                  char *buf, size_t buf_len) {
    int i, m;
    switch (proto) {
    default:
    case LSOF_PROTOCOL_UNKNOWN:
        for (i = 0, m = 1; i < IPROTOL - 2; i++)
            m *= 10;
        if (m > unknown_proto_number)
            (void)snpf(buf, buf_len, "%d?", unknown_proto_number);
        else
            (void)snpf(buf, buf_len, "*%d?", unknown_proto_number % (m / 10));
        break;
    case LSOF_PROTOCOL_INVALID:
        (void)snpf(buf, buf_len, "");
        break;

    /* Copied from scripts/lsof_proto.c */
    case LSOF_PROTOCOL_IP:
        (void)snpf(buf, buf_len, "IP");
        break;
    case LSOF_PROTOCOL_HOPOPTS:
        (void)snpf(buf, buf_len, "HOPOPTS");
        break;
    case LSOF_PROTOCOL_ICMP:
        (void)snpf(buf, buf_len, "ICMP");
        break;
    case LSOF_PROTOCOL_IGMP:
        (void)snpf(buf, buf_len, "IGMP");
        break;
    case LSOF_PROTOCOL_GGP:
        (void)snpf(buf, buf_len, "GGP");
        break;
    case LSOF_PROTOCOL_IPIP:
        (void)snpf(buf, buf_len, "IPIP");
        break;
    case LSOF_PROTOCOL_IPV4:
        (void)snpf(buf, buf_len, "IPV4");
        break;
    case LSOF_PROTOCOL_TCP:
        (void)snpf(buf, buf_len, "TCP");
        break;
    case LSOF_PROTOCOL_ST:
        (void)snpf(buf, buf_len, "ST");
        break;
    case LSOF_PROTOCOL_EGP:
        (void)snpf(buf, buf_len, "EGP");
        break;
    case LSOF_PROTOCOL_PIGP:
        (void)snpf(buf, buf_len, "PIGP");
        break;
    case LSOF_PROTOCOL_RCCMON:
        (void)snpf(buf, buf_len, "RCCMON");
        break;
    case LSOF_PROTOCOL_NVPII:
        (void)snpf(buf, buf_len, "NVPII");
        break;
    case LSOF_PROTOCOL_PUP:
        (void)snpf(buf, buf_len, "PUP");
        break;
    case LSOF_PROTOCOL_ARGUS:
        (void)snpf(buf, buf_len, "ARGUS");
        break;
    case LSOF_PROTOCOL_EMCON:
        (void)snpf(buf, buf_len, "EMCON");
        break;
    case LSOF_PROTOCOL_XNET:
        (void)snpf(buf, buf_len, "XNET");
        break;
    case LSOF_PROTOCOL_CHAOS:
        (void)snpf(buf, buf_len, "CHAOS");
        break;
    case LSOF_PROTOCOL_UDP:
        (void)snpf(buf, buf_len, "UDP");
        break;
    case LSOF_PROTOCOL_MUX:
        (void)snpf(buf, buf_len, "MUX");
        break;
    case LSOF_PROTOCOL_MEAS:
        (void)snpf(buf, buf_len, "MEAS");
        break;
    case LSOF_PROTOCOL_HMP:
        (void)snpf(buf, buf_len, "HMP");
        break;
    case LSOF_PROTOCOL_PRM:
        (void)snpf(buf, buf_len, "PRM");
        break;
    case LSOF_PROTOCOL_IDP:
        (void)snpf(buf, buf_len, "IDP");
        break;
    case LSOF_PROTOCOL_TRUNK1:
        (void)snpf(buf, buf_len, "TRUNK1");
        break;
    case LSOF_PROTOCOL_TRUNK2:
        (void)snpf(buf, buf_len, "TRUNK2");
        break;
    case LSOF_PROTOCOL_LEAF1:
        (void)snpf(buf, buf_len, "LEAF1");
        break;
    case LSOF_PROTOCOL_LEAF2:
        (void)snpf(buf, buf_len, "LEAF2");
        break;
    case LSOF_PROTOCOL_RDP:
        (void)snpf(buf, buf_len, "RDP");
        break;
    case LSOF_PROTOCOL_IRTP:
        (void)snpf(buf, buf_len, "IRTP");
        break;
    case LSOF_PROTOCOL_TP:
        (void)snpf(buf, buf_len, "TP");
        break;
    case LSOF_PROTOCOL_BLT:
        (void)snpf(buf, buf_len, "BLT");
        break;
    case LSOF_PROTOCOL_NSP:
        (void)snpf(buf, buf_len, "NSP");
        break;
    case LSOF_PROTOCOL_INP:
        (void)snpf(buf, buf_len, "INP");
        break;
    case LSOF_PROTOCOL_DCCP:
        (void)snpf(buf, buf_len, "DCCP");
        break;
    case LSOF_PROTOCOL_3PC:
        (void)snpf(buf, buf_len, "3PC");
        break;
    case LSOF_PROTOCOL_IDPR:
        (void)snpf(buf, buf_len, "IDPR");
        break;
    case LSOF_PROTOCOL_XTP:
        (void)snpf(buf, buf_len, "XTP");
        break;
    case LSOF_PROTOCOL_DDP:
        (void)snpf(buf, buf_len, "DDP");
        break;
    case LSOF_PROTOCOL_CMTP:
        (void)snpf(buf, buf_len, "CMTP");
        break;
    case LSOF_PROTOCOL_TPXX:
        (void)snpf(buf, buf_len, "TPXX");
        break;
    case LSOF_PROTOCOL_IL:
        (void)snpf(buf, buf_len, "IL");
        break;
    case LSOF_PROTOCOL_IPV6:
        (void)snpf(buf, buf_len, "IPV6");
        break;
    case LSOF_PROTOCOL_SDRP:
        (void)snpf(buf, buf_len, "SDRP");
        break;
    case LSOF_PROTOCOL_ROUTING:
        (void)snpf(buf, buf_len, "ROUTING");
        break;
    case LSOF_PROTOCOL_FRAGMENT:
        (void)snpf(buf, buf_len, "FRAGMNT");
        break;
    case LSOF_PROTOCOL_IDRP:
        (void)snpf(buf, buf_len, "IDRP");
        break;
    case LSOF_PROTOCOL_RSVP:
        (void)snpf(buf, buf_len, "RSVP");
        break;
    case LSOF_PROTOCOL_GRE:
        (void)snpf(buf, buf_len, "GRE");
        break;
    case LSOF_PROTOCOL_MHRP:
        (void)snpf(buf, buf_len, "MHRP");
        break;
    case LSOF_PROTOCOL_BHA:
        (void)snpf(buf, buf_len, "BHA");
        break;
    case LSOF_PROTOCOL_ESP:
        (void)snpf(buf, buf_len, "ESP");
        break;
    case LSOF_PROTOCOL_AH:
        (void)snpf(buf, buf_len, "AH");
        break;
    case LSOF_PROTOCOL_INLSP:
        (void)snpf(buf, buf_len, "INLSP");
        break;
    case LSOF_PROTOCOL_SWIPE:
        (void)snpf(buf, buf_len, "SWIPE");
        break;
    case LSOF_PROTOCOL_NHRP:
        (void)snpf(buf, buf_len, "NHRP");
        break;
    case LSOF_PROTOCOL_MOBILE:
        (void)snpf(buf, buf_len, "MOBILE");
        break;
    case LSOF_PROTOCOL_TLSP:
        (void)snpf(buf, buf_len, "TLSP");
        break;
    case LSOF_PROTOCOL_SKIP:
        (void)snpf(buf, buf_len, "SKIP");
        break;
    case LSOF_PROTOCOL_ICMPV6:
        (void)snpf(buf, buf_len, "ICMPV6");
        break;
    case LSOF_PROTOCOL_NONE:
        (void)snpf(buf, buf_len, "NONE");
        break;
    case LSOF_PROTOCOL_DSTOPTS:
        (void)snpf(buf, buf_len, "DSTOPTS");
        break;
    case LSOF_PROTOCOL_AHIP:
        (void)snpf(buf, buf_len, "AHIP");
        break;
    case LSOF_PROTOCOL_CFTP:
        (void)snpf(buf, buf_len, "CFTP");
        break;
    case LSOF_PROTOCOL_HELLO:
        (void)snpf(buf, buf_len, "HELLO");
        break;
    case LSOF_PROTOCOL_SATEXPAK:
        (void)snpf(buf, buf_len, "STEXPAK");
        break;
    case LSOF_PROTOCOL_KRYPTOLAN:
        (void)snpf(buf, buf_len, "KRYPTLN");
        break;
    case LSOF_PROTOCOL_RVD:
        (void)snpf(buf, buf_len, "RVD");
        break;
    case LSOF_PROTOCOL_IPPC:
        (void)snpf(buf, buf_len, "IPPC");
        break;
    case LSOF_PROTOCOL_ADFS:
        (void)snpf(buf, buf_len, "ADFS");
        break;
    case LSOF_PROTOCOL_SATMON:
        (void)snpf(buf, buf_len, "SATMON");
        break;
    case LSOF_PROTOCOL_VISA:
        (void)snpf(buf, buf_len, "VISA");
        break;
    case LSOF_PROTOCOL_IPCV:
        (void)snpf(buf, buf_len, "IPCV");
        break;
    case LSOF_PROTOCOL_CPNX:
        (void)snpf(buf, buf_len, "CPNX");
        break;
    case LSOF_PROTOCOL_CPHB:
        (void)snpf(buf, buf_len, "CPHB");
        break;
    case LSOF_PROTOCOL_WSN:
        (void)snpf(buf, buf_len, "WSN");
        break;
    case LSOF_PROTOCOL_PVP:
        (void)snpf(buf, buf_len, "PVP");
        break;
    case LSOF_PROTOCOL_BRSATMON:
        (void)snpf(buf, buf_len, "BRSTMON");
        break;
    case LSOF_PROTOCOL_ND:
        (void)snpf(buf, buf_len, "ND");
        break;
    case LSOF_PROTOCOL_WBMON:
        (void)snpf(buf, buf_len, "WBMON");
        break;
    case LSOF_PROTOCOL_WBEXPAK:
        (void)snpf(buf, buf_len, "WBEXPAK");
        break;
    case LSOF_PROTOCOL_EON:
        (void)snpf(buf, buf_len, "EON");
        break;
    case LSOF_PROTOCOL_VMTP:
        (void)snpf(buf, buf_len, "VMTP");
        break;
    case LSOF_PROTOCOL_SVMTP:
        (void)snpf(buf, buf_len, "SVMTP");
        break;
    case LSOF_PROTOCOL_VINES:
        (void)snpf(buf, buf_len, "VINES");
        break;
    case LSOF_PROTOCOL_TTP:
        (void)snpf(buf, buf_len, "TTP");
        break;
    case LSOF_PROTOCOL_IGP:
        (void)snpf(buf, buf_len, "IGP");
        break;
    case LSOF_PROTOCOL_DGP:
        (void)snpf(buf, buf_len, "DGP");
        break;
    case LSOF_PROTOCOL_TCF:
        (void)snpf(buf, buf_len, "TCF");
        break;
    case LSOF_PROTOCOL_IGRP:
        (void)snpf(buf, buf_len, "IGRP");
        break;
    case LSOF_PROTOCOL_OSPFIGP:
        (void)snpf(buf, buf_len, "OSPFIGP");
        break;
    case LSOF_PROTOCOL_SRPC:
        (void)snpf(buf, buf_len, "SRPC");
        break;
    case LSOF_PROTOCOL_LARP:
        (void)snpf(buf, buf_len, "LARP");
        break;
    case LSOF_PROTOCOL_MTP:
        (void)snpf(buf, buf_len, "MTP");
        break;
    case LSOF_PROTOCOL_AX25:
        (void)snpf(buf, buf_len, "AX25");
        break;
    case LSOF_PROTOCOL_BEETPH:
        (void)snpf(buf, buf_len, "BEETPH");
        break;
    case LSOF_PROTOCOL_IPEIP:
        (void)snpf(buf, buf_len, "IPEIP");
        break;
    case LSOF_PROTOCOL_MICP:
        (void)snpf(buf, buf_len, "MICP");
        break;
    case LSOF_PROTOCOL_SCCSP:
        (void)snpf(buf, buf_len, "SCCSP");
        break;
    case LSOF_PROTOCOL_ETHERIP:
        (void)snpf(buf, buf_len, "ETHERIP");
        break;
    case LSOF_PROTOCOL_ENCAP:
        (void)snpf(buf, buf_len, "ENCAP");
        break;
    case LSOF_PROTOCOL_APES:
        (void)snpf(buf, buf_len, "APES");
        break;
    case LSOF_PROTOCOL_GMTP:
        (void)snpf(buf, buf_len, "GMTP");
        break;
    case LSOF_PROTOCOL_PIM:
        (void)snpf(buf, buf_len, "PIM");
        break;
    case LSOF_PROTOCOL_COMP:
        (void)snpf(buf, buf_len, "COMP");
        break;
    case LSOF_PROTOCOL_IPCOMP:
        (void)snpf(buf, buf_len, "IPCOMP");
        break;
    case LSOF_PROTOCOL_CARP:
        (void)snpf(buf, buf_len, "CARP");
        break;
    case LSOF_PROTOCOL_PGM:
        (void)snpf(buf, buf_len, "PGM");
        break;
    case LSOF_PROTOCOL_SCTP:
        (void)snpf(buf, buf_len, "SCTP");
        break;
    case LSOF_PROTOCOL_MH:
        (void)snpf(buf, buf_len, "MH");
        break;
    case LSOF_PROTOCOL_UDPLITE:
        (void)snpf(buf, buf_len, "UDPLITE");
        break;
    case LSOF_PROTOCOL_MPLS:
        (void)snpf(buf, buf_len, "MPLS");
        break;
    case LSOF_PROTOCOL_HIP:
        (void)snpf(buf, buf_len, "HIP");
        break;
    case LSOF_PROTOCOL_SHIM6:
        (void)snpf(buf, buf_len, "SHIM6");
        break;
    case LSOF_PROTOCOL_ETHERNET:
        (void)snpf(buf, buf_len, "ETHER");
        break;
    case LSOF_PROTOCOL_PFSYNC:
        (void)snpf(buf, buf_len, "PFSYNC");
        break;
    case LSOF_PROTOCOL_RESERVED_253:
        (void)snpf(buf, buf_len, "RSVD253");
        break;
    case LSOF_PROTOCOL_RESERVED_254:
        (void)snpf(buf, buf_len, "RSVD254");
        break;
    case LSOF_PROTOCOL_OLD_DIVERT:
        (void)snpf(buf, buf_len, "ODIVERT");
        break;
    case LSOF_PROTOCOL_RAW:
        (void)snpf(buf, buf_len, "RAW");
        break;
    case LSOF_PROTOCOL_MAX:
        (void)snpf(buf, buf_len, "MAX");
        break;
    case LSOF_PROTOCOL_DONE:
        (void)snpf(buf, buf_len, "DONE");
        break;
    case LSOF_PROTOCOL_SEND:
        (void)snpf(buf, buf_len, "SEND");
        break;
    case LSOF_PROTOCOL_MPTCP:
        (void)snpf(buf, buf_len, "MPTCP");
        break;
    case LSOF_PROTOCOL_SPACER:
        (void)snpf(buf, buf_len, "SPACER");
        break;
    case LSOF_PROTOCOL_802_3:
        (void)snpf(buf, buf_len, "802_3");
        break;
    case LSOF_PROTOCOL_ALL:
        (void)snpf(buf, buf_len, "ALL");
        break;
    case LSOF_PROTOCOL_802_2:
        (void)snpf(buf, buf_len, "802_2");
        break;
    case LSOF_PROTOCOL_SNAP:
        (void)snpf(buf, buf_len, "SNAP");
        break;
    case LSOF_PROTOCOL_DDCMP:
        (void)snpf(buf, buf_len, "DDCMP");
        break;
    case LSOF_PROTOCOL_WAN_PPP:
        (void)snpf(buf, buf_len, "WAN_PPP");
        break;
    case LSOF_PROTOCOL_PPP_MP:
        (void)snpf(buf, buf_len, "PPP_MP");
        break;
    case LSOF_PROTOCOL_LOCALTALK:
        (void)snpf(buf, buf_len, "LCLTALK");
        break;
    case LSOF_PROTOCOL_CAN:
        (void)snpf(buf, buf_len, "CAN");
        break;
    case LSOF_PROTOCOL_CANFD:
        (void)snpf(buf, buf_len, "CANFD");
        break;
    case LSOF_PROTOCOL_CANXL:
        (void)snpf(buf, buf_len, "CANXL");
        break;
    case LSOF_PROTOCOL_PPPTALK:
        (void)snpf(buf, buf_len, "PPPTALK");
        break;
    case LSOF_PROTOCOL_TR_802_2:
        (void)snpf(buf, buf_len, "802.2");
        break;
    case LSOF_PROTOCOL_MOBITEX:
        (void)snpf(buf, buf_len, "MOBITEX");
        break;
    case LSOF_PROTOCOL_CONTROL:
        (void)snpf(buf, buf_len, "CONTROL");
        break;
    case LSOF_PROTOCOL_IRDA:
        (void)snpf(buf, buf_len, "IRDA");
        break;
    case LSOF_PROTOCOL_ECONET:
        (void)snpf(buf, buf_len, "ECONET");
        break;
    case LSOF_PROTOCOL_HDLC:
        (void)snpf(buf, buf_len, "HDLC");
        break;
    case LSOF_PROTOCOL_ARCNET:
        (void)snpf(buf, buf_len, "ARCNET");
        break;
    case LSOF_PROTOCOL_DSA:
        (void)snpf(buf, buf_len, "DSA");
        break;
    case LSOF_PROTOCOL_TRAILER:
        (void)snpf(buf, buf_len, "TRAILER");
        break;
    case LSOF_PROTOCOL_LOOP:
        (void)snpf(buf, buf_len, "LOOP");
        break;
    case LSOF_PROTOCOL_PHONET:
        (void)snpf(buf, buf_len, "PHONET");
        break;
    case LSOF_PROTOCOL_IEEE802154:
        (void)snpf(buf, buf_len, "802154");
        break;
    case LSOF_PROTOCOL_CAIF:
        (void)snpf(buf, buf_len, "CAIF");
        break;
    case LSOF_PROTOCOL_XDSA:
        (void)snpf(buf, buf_len, "XDSA");
        break;
    case LSOF_PROTOCOL_MAP:
        (void)snpf(buf, buf_len, "MAP");
        break;
    case LSOF_PROTOCOL_MCTP:
        (void)snpf(buf, buf_len, "MCTP");
        break;
    case LSOF_PROTOCOL_PUPAT:
        (void)snpf(buf, buf_len, "PUPAT");
        break;
    case LSOF_PROTOCOL_802_3_MIN:
        (void)snpf(buf, buf_len, "802.3");
        break;
    case LSOF_PROTOCOL_X25:
        (void)snpf(buf, buf_len, "X25");
        break;
    case LSOF_PROTOCOL_ARP:
        (void)snpf(buf, buf_len, "ARP");
        break;
    case LSOF_PROTOCOL_BPQ:
        (void)snpf(buf, buf_len, "BPQ");
        break;
    case LSOF_PROTOCOL_IEEEPUP:
        (void)snpf(buf, buf_len, "IEEEPUP");
        break;
    case LSOF_PROTOCOL_IEEEPUPAT:
        (void)snpf(buf, buf_len, "I3EPUPAT");
        break;
    case LSOF_PROTOCOL_ERSPAN2:
        (void)snpf(buf, buf_len, "ERSPAN2");
        break;
    case LSOF_PROTOCOL_TSN:
        (void)snpf(buf, buf_len, "TSN");
        break;
    case LSOF_PROTOCOL_BATMAN:
        (void)snpf(buf, buf_len, "BATMAN");
        break;
    case LSOF_PROTOCOL_DEC:
        (void)snpf(buf, buf_len, "DEC");
        break;
    case LSOF_PROTOCOL_DNA_DL:
        (void)snpf(buf, buf_len, "DNA_DL");
        break;
    case LSOF_PROTOCOL_DNA_RC:
        (void)snpf(buf, buf_len, "DNA_RC");
        break;
    case LSOF_PROTOCOL_DNA_RT:
        (void)snpf(buf, buf_len, "DNA_RT");
        break;
    case LSOF_PROTOCOL_LAT:
        (void)snpf(buf, buf_len, "LAT");
        break;
    case LSOF_PROTOCOL_DIAG:
        (void)snpf(buf, buf_len, "DIAG");
        break;
    case LSOF_PROTOCOL_CUST:
        (void)snpf(buf, buf_len, "CUST");
        break;
    case LSOF_PROTOCOL_SCA:
        (void)snpf(buf, buf_len, "SCA");
        break;
    case LSOF_PROTOCOL_TEB:
        (void)snpf(buf, buf_len, "TEB");
        break;
    case LSOF_PROTOCOL_RARP:
        (void)snpf(buf, buf_len, "RARP");
        break;
    case LSOF_PROTOCOL_ATALK:
        (void)snpf(buf, buf_len, "ATALK");
        break;
    case LSOF_PROTOCOL_AARP:
        (void)snpf(buf, buf_len, "AARP");
        break;
    case LSOF_PROTOCOL_8021Q:
        (void)snpf(buf, buf_len, "8021Q");
        break;
    case LSOF_PROTOCOL_IPX:
        (void)snpf(buf, buf_len, "IPX");
        break;
    case LSOF_PROTOCOL_PAUSE:
        (void)snpf(buf, buf_len, "PAUSE");
        break;
    case LSOF_PROTOCOL_SLOW:
        (void)snpf(buf, buf_len, "SLOW");
        break;
    case LSOF_PROTOCOL_WCCP:
        (void)snpf(buf, buf_len, "WCCP");
        break;
    case LSOF_PROTOCOL_MPLS_UC:
        (void)snpf(buf, buf_len, "MPLS_UC");
        break;
    case LSOF_PROTOCOL_MPLS_MC:
        (void)snpf(buf, buf_len, "MPLS_MC");
        break;
    case LSOF_PROTOCOL_ATMMPOA:
        (void)snpf(buf, buf_len, "ATMMPOA");
        break;
    case LSOF_PROTOCOL_PPP_DISC:
        (void)snpf(buf, buf_len, "PPPDISC");
        break;
    case LSOF_PROTOCOL_PPP_SES:
        (void)snpf(buf, buf_len, "PPP_SES");
        break;
    case LSOF_PROTOCOL_LINK_CTL:
        (void)snpf(buf, buf_len, "LINKCTL");
        break;
    case LSOF_PROTOCOL_ATMFATE:
        (void)snpf(buf, buf_len, "ATMFATE");
        break;
    case LSOF_PROTOCOL_PAE:
        (void)snpf(buf, buf_len, "PAE");
        break;
    case LSOF_PROTOCOL_PROFINET:
        (void)snpf(buf, buf_len, "PROFINT");
        break;
    case LSOF_PROTOCOL_REALTEK:
        (void)snpf(buf, buf_len, "REALTEK");
        break;
    case LSOF_PROTOCOL_AOE:
        (void)snpf(buf, buf_len, "AOE");
        break;
    case LSOF_PROTOCOL_ETHERCAT:
        (void)snpf(buf, buf_len, "ETHCAT");
        break;
    case LSOF_PROTOCOL_8021AD:
        (void)snpf(buf, buf_len, "8021AD");
        break;
    case LSOF_PROTOCOL_802_EX1:
        (void)snpf(buf, buf_len, "802_EX1");
        break;
    case LSOF_PROTOCOL_ERSPAN:
        (void)snpf(buf, buf_len, "ERSPAN");
        break;
    case LSOF_PROTOCOL_PREAUTH:
        (void)snpf(buf, buf_len, "PREAUTH");
        break;
    case LSOF_PROTOCOL_TIPC:
        (void)snpf(buf, buf_len, "TIPC");
        break;
    case LSOF_PROTOCOL_LLDP:
        (void)snpf(buf, buf_len, "LLDP");
        break;
    case LSOF_PROTOCOL_MRP:
        (void)snpf(buf, buf_len, "MRP");
        break;
    case LSOF_PROTOCOL_MACSEC:
        (void)snpf(buf, buf_len, "MACSEC");
        break;
    case LSOF_PROTOCOL_8021AH:
        (void)snpf(buf, buf_len, "8021AH");
        break;
    case LSOF_PROTOCOL_MVRP:
        (void)snpf(buf, buf_len, "MVRP");
        break;
    case LSOF_PROTOCOL_1588:
        (void)snpf(buf, buf_len, "1588");
        break;
    case LSOF_PROTOCOL_NCSI:
        (void)snpf(buf, buf_len, "NCSI");
        break;
    case LSOF_PROTOCOL_PRP:
        (void)snpf(buf, buf_len, "PRP");
        break;
    case LSOF_PROTOCOL_CFM:
        (void)snpf(buf, buf_len, "CFM");
        break;
    case LSOF_PROTOCOL_FCOE:
        (void)snpf(buf, buf_len, "FCOE");
        break;
    case LSOF_PROTOCOL_TDLS:
        (void)snpf(buf, buf_len, "TDLS");
        break;
    case LSOF_PROTOCOL_FIP:
        (void)snpf(buf, buf_len, "FIP");
        break;
    case LSOF_PROTOCOL_IBOE:
        (void)snpf(buf, buf_len, "IBOE");
        break;
    case LSOF_PROTOCOL_80221:
        (void)snpf(buf, buf_len, "80221");
        break;
    case LSOF_PROTOCOL_HSR:
        (void)snpf(buf, buf_len, "HSR");
        break;
    case LSOF_PROTOCOL_NSH:
        (void)snpf(buf, buf_len, "NSH");
        break;
    case LSOF_PROTOCOL_LOOPBACK:
        (void)snpf(buf, buf_len, "LOOPBAK");
        break;
    case LSOF_PROTOCOL_QINQ1:
        (void)snpf(buf, buf_len, "QINQ1");
        break;
    case LSOF_PROTOCOL_QINQ2:
        (void)snpf(buf, buf_len, "QINQ2");
        break;
    case LSOF_PROTOCOL_QINQ3:
        (void)snpf(buf, buf_len, "QINQ3");
        break;
    case LSOF_PROTOCOL_EDSA:
        (void)snpf(buf, buf_len, "EDSA");
        break;
    case LSOF_PROTOCOL_DSA_8021Q:
        (void)snpf(buf, buf_len, "DSAD1Q");
        break;
    case LSOF_PROTOCOL_DSA_A5PSW:
        (void)snpf(buf, buf_len, "DSA5PSW");
        break;
    case LSOF_PROTOCOL_IFE:
        (void)snpf(buf, buf_len, "IFE");
        break;
    case LSOF_PROTOCOL_AF_IUCV:
        (void)snpf(buf, buf_len, "AF_IUCV");
        break;

    /* Others */
    case LSOF_PROTOCOL_8025:
        (void)snpf(buf, buf_len, "802.5");
        break;
    case LSOF_PROTOCOL_CCITT:
        (void)snpf(buf, buf_len, "CCITT");
        break;
    case LSOF_PROTOCOL_STR:
        (void)snpf(buf, buf_len, "STR");
        break;
    case LSOF_PROTOCOL_SHARED:
        (void)snpf(buf, buf_len, "SHARED");
        break;
    }
}

/*
 * printsockty() - print socket type
 */
char *printsockty(int ty) /* socket type -- e.g., from so_type */
{
    static char buf[64];
    char *cp;

    switch (ty) {

#if defined(SOCK_STREAM)
    case SOCK_STREAM:
        cp = "STREAM";
        break;
#endif /* defined(SOCK_STREAM) */

#if defined(SOCK_STREAM)
    case SOCK_DGRAM:
        cp = "DGRAM";
        break;
#endif /* defined(SOCK_DGRAM) */

#if defined(SOCK_RAW)
    case SOCK_RAW:
        cp = "RAW";
        break;
#endif /* defined(SOCK_RAW) */

#if defined(SOCK_RDM)
    case SOCK_RDM:
        cp = "RDM";
        break;
#endif /* defined(SOCK_RDM) */

#if defined(SOCK_SEQPACKET)
    case SOCK_SEQPACKET:
        cp = "SEQPACKET";
        break;
#endif /* defined(SOCK_SEQPACKET) */

    default:
        (void)snpf(buf, sizeof(buf), "SOCK_%#x", ty);
        return (buf);
    }
    (void)snpf(buf, sizeof(buf), "SOCK_%s", cp);
    return (buf);
}
