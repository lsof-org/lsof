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
static char copyright[] =
"@(#) Copyright 2005 Apple Computer, Inc. and Purdue Research Foundation.\nAll rights reserved.\n";
static char *rcsid = "$Id: dsock.c,v 1.2 2005/11/01 20:10:09 abe Exp abe $";
#endif


#include "lsof.h"


/*
 * IPv6_2_IPv4()  -- macro to define the address of an IPv4 address contained
 *                 in an IPv6 address
 */

#define	IPv6_2_IPv4(v6)	(((uint8_t *)((struct in6_addr *)v6)->s6_addr)+12)


void
process_socket(int pid, int32_t fd)
{
	unsigned char		*fa	= (unsigned char *)NULL;
	int			fp;
	int			fam;
	unsigned char		*la	= (unsigned char *)NULL;
	int			lp;
	int			size;
	struct socket_fdinfo	socketi;
	int			unl;

	(void) snpf(Lf->type, sizeof(Lf->type), "sock");
	Lf->inp_ty = 2;

	size = proc_pidfdinfo(pid, fd, PROC_PIDFDSOCKETINFO, &socketi, sizeof(socketi));
	if (size <= 0) {
	    switch (errno) {
	    case ESRCH :
		// if the process is no longer available
		break;
	    case EBADF :
		// if the file descriptor is no longer available
		break;
	    default :
		(void) fprintf(stderr,
			       "%s: can't get socket information (%s)\n",
			       Pn,
			       strerror(errno));
		Exit(1);
	    }
	    return;
	} else if (size < sizeof(socketi)) {
	    (void) fprintf(stderr,
			   "%s: not enough data for proc_pidfdinfo(PROC_PIDFDSOCKETINFO) (%d < %ld)\n",
			   Pn,
			   size,
			   sizeof(socketi));
	    Exit(1);
	}

	enter_file_info(&socketi.pfi);

	/*
	 * Save size information
	 */
	if (Fsize) {
	    if (Lf->access == 'r')
		Lf->sz = (SZOFFTYPE)socketi.psi.soi_rcv.sbi_cc;
	    else if (Lf->access == 'w')
		Lf->sz = (SZOFFTYPE)socketi.psi.soi_snd.sbi_cc;
	    else
		Lf->sz = (SZOFFTYPE)(socketi.psi.soi_rcv.sbi_cc + socketi.psi.soi_snd.sbi_cc);
	    Lf->sz_def = 1;
	} else {
	    Lf->off_def = 1;
	}

	Lf->lts.rq = socketi.psi.soi_rcv.sbi_cc;
	Lf->lts.sq = socketi.psi.soi_snd.sbi_cc;
	Lf->lts.rqs = 1;
	Lf->lts.sqs = 1;

	Lf->lts.ltm = (unsigned int)(socketi.psi.soi_linger & 0xffff);
	Lf->lts.opt = (unsigned int)(socketi.psi.soi_options & 0xffff);
	Lf->lts.pqlen = (unsigned int)socketi.psi.soi_incqlen;
	Lf->lts.qlen = (unsigned int)socketi.psi.soi_qlen;
	Lf->lts.qlim = (unsigned int)socketi.psi.soi_qlimit;
	Lf->lts.rbsz = (unsigned long)socketi.psi.soi_rcv.sbi_mbmax;	// use sbi_hiwat?
	Lf->lts.sbsz = (unsigned long)socketi.psi.soi_snd.sbi_mbmax;	// use sbi_hiwat?
	Lf->lts.pqlens = 1;
	Lf->lts.qlens = 1;
	Lf->lts.qlims = 1;
	Lf->lts.rbszs = 1;
	Lf->lts.sbszs = 1;

	Lf->lts.ss = (unsigned int)socketi.psi.soi_state;

	/*
	 * Process socket by the associated domain family
	 */
	switch ((fam = socketi.psi.soi_family)) {

	/*
	 * Process IPv[46] sockets.
	 */
	case AF_INET:
	case AF_INET6:
	    (void) snpf(Lf->type, sizeof(Lf->type),
			(fam == AF_INET) ? "IPv4" : "IPv6");

	    if ((socketi.psi.soi_kind != SOCKINFO_IN) &&
		(socketi.psi.soi_kind != SOCKINFO_TCP)) {
		break;
	    }

	    /*
	     * Process an Internet domain socket
	     */
	    if (Fnet) {
		if (!FnetTy
		||  ((FnetTy == 4) && (fam == AF_INET))
		||  ((FnetTy == 6) && (fam == AF_INET6))
		)
		    Lf->sf |= SELNET;
	    }

	    printiproto(socketi.psi.soi_protocol);

	    if ((socketi.psi.soi_kind == SOCKINFO_TCP) && socketi.psi.soi_proto.pri_tcp.tcpsi_tp) {
		    enter_dev_ch(print_kptr((KA_T)socketi.psi.soi_proto.pri_tcp.tcpsi_tp, (char *)NULL, 0));
	    } else {
		enter_dev_ch(print_kptr((KA_T)socketi.psi.soi_pcb, (char *)NULL, 0));
	    }

	    if (fam == AF_INET) {
		/*
		 * Save IPv4 address information
		 */
		if (socketi.psi.soi_kind == SOCKINFO_TCP) {
		    // if TCP
		    la = (unsigned char *)&socketi.psi.soi_proto.pri_tcp.tcpsi_ini.insi_laddr.ina_46.i46a_addr4;
		    lp = (int)ntohs(socketi.psi.soi_proto.pri_tcp.tcpsi_ini.insi_lport);
		    fa = (unsigned char *)&socketi.psi.soi_proto.pri_tcp.tcpsi_ini.insi_faddr.ina_46.i46a_addr4;
		    fp = (int)ntohs(socketi.psi.soi_proto.pri_tcp.tcpsi_ini.insi_fport);
		} else {
		    // if not TCP
		    la = (unsigned char *)&socketi.psi.soi_proto.pri_in.insi_laddr.ina_46.i46a_addr4;
		    lp = (int)ntohs(socketi.psi.soi_proto.pri_in.insi_lport);
		    fa = (unsigned char *)&socketi.psi.soi_proto.pri_in.insi_faddr.ina_46.i46a_addr4;
		    fp = (int)ntohs(socketi.psi.soi_proto.pri_in.insi_fport);
		}
		if (((fa != NULL) && (*fa == INADDR_ANY)) && !fp) {
		    fa = (unsigned char *)NULL;
		    fp = 0;
		}
	    } else {
		/*
		 * Save IPv6 address information
		 */
		if (socketi.psi.soi_kind == SOCKINFO_TCP) {
		    // if TCP
		    la = (unsigned char *)&socketi.psi.soi_proto.pri_tcp.tcpsi_ini.insi_laddr.ina_6;
		    lp = (int)ntohs(socketi.psi.soi_proto.pri_tcp.tcpsi_ini.insi_lport);
		    fa = (unsigned char *)&socketi.psi.soi_proto.pri_tcp.tcpsi_ini.insi_faddr.ina_6;
		    fp = (int)ntohs(socketi.psi.soi_proto.pri_tcp.tcpsi_ini.insi_fport);
		} else {
		    // if not TCP
		    la = (unsigned char *)&socketi.psi.soi_proto.pri_in.insi_laddr.ina_6;
		    lp = (int)ntohs(socketi.psi.soi_proto.pri_in.insi_lport);
		    fa = (unsigned char *)&socketi.psi.soi_proto.pri_in.insi_faddr.ina_6;
		    fp = (int)ntohs(socketi.psi.soi_proto.pri_in.insi_fport);
		}
		if (IN6_IS_ADDR_UNSPECIFIED((struct in6_addr *)fa) && !fp) {
		    fa = (unsigned char *)NULL;
		    fp = 0;
		}
		if ((la && IN6_IS_ADDR_V4MAPPED((struct in6_addr *)la))
		||  (fa && IN6_IS_ADDR_V4MAPPED((struct in6_addr *)fa))) {
		    /*
		     * Adjust for IPv4 addresses mapped in IPv6 addresses.
		     */
		    fam = AF_INET;
		    if (la)
			la = (unsigned char *)IPv6_2_IPv4(la);
		    if (fa)
			fa = (unsigned char *)IPv6_2_IPv4(fa);
		}
	    }

	    /*
	     * Enter local and remote addresses by address family
	     */
	    if (fa || la)
		(void) ent_inaddr(la, lp, fa, fp, fam);

	    if (socketi.psi.soi_kind == SOCKINFO_TCP) {
		// if TCP
		Lf->lts.type = 0;
		Lf->lts.state.i = (int)socketi.psi.soi_proto.pri_tcp.tcpsi_state;
		Lf->lts.kai = (unsigned int)socketi.psi.soi_proto.pri_tcp.tcpsi_timer[TCPT_KEEP];
		Lf->lts.mss = (unsigned long)socketi.psi.soi_proto.pri_tcp.tcpsi_mss;
		Lf->lts.msss = (unsigned char)1;
		Lf->lts.topt = (unsigned int)socketi.psi.soi_proto.pri_tcp.tcpsi_flags;
	    }

	    break;

	/*
	 * Process a Unix domain socket.
	 */
	case AF_UNIX:
	    (void) snpf(Lf->type, sizeof(Lf->type), "unix");

	    if (socketi.psi.soi_kind != SOCKINFO_UN) {
		break;
	    }

	    if (Funix)
		Lf->sf |= SELUNX;

	    enter_dev_ch(print_kptr((KA_T)socketi.psi.soi_pcb, (char *)NULL, 0));

	    /*
	     * Print information on Unix socket that has no address bound
	     * to it, although it may be connected to another Unix domain
	     * socket as a pipe.
	     */
	    if (socketi.psi.soi_proto.pri_un.unsi_addr.ua_sun.sun_family != AF_UNIX) {
		if (socketi.psi.soi_proto.pri_un.unsi_addr.ua_sun.sun_family == AF_UNSPEC) {
		    if (socketi.psi.soi_proto.pri_un.unsi_conn_pcb) {
			    (void) snpf(Namech, Namechl, "->%s",
				print_kptr((KA_T) socketi.psi.soi_proto.pri_un.unsi_conn_pcb,(char *)NULL,0));
		    } else
			(void) snpf(Namech, Namechl, "->(none)");
		} else
		    (void) snpf(Namech, Namechl, "unknown sun_family (%d)",
			socketi.psi.soi_proto.pri_un.unsi_addr.ua_sun.sun_family);
		break;
	    }
	    if (socketi.psi.soi_proto.pri_un.unsi_addr.ua_sun.sun_path[0]) {
		unl = socketi.psi.soi_proto.pri_un.unsi_addr.ua_sun.sun_len - offsetof(struct sockaddr_un, sun_path);
		if ((unl < 0) || (unl >= sizeof(socketi.psi.soi_proto.pri_un.unsi_addr.ua_sun.sun_path)))
		    unl = sizeof(socketi.psi.soi_proto.pri_un.unsi_addr.ua_sun.sun_path) - 1;
		socketi.psi.soi_proto.pri_un.unsi_addr.ua_sun.sun_path[unl] = '\0';
		if (socketi.psi.soi_proto.pri_un.unsi_addr.ua_sun.sun_path[0]
		&&  Sfile
		&&  is_file_named(socketi.psi.soi_proto.pri_un.unsi_addr.ua_sun.sun_path, 0))
		    Lf->sf |= SELNM;
		if (socketi.psi.soi_proto.pri_un.unsi_addr.ua_sun.sun_path[0] && !Namech[0])
		    (void) snpf(Namech, Namechl, "%s", socketi.psi.soi_proto.pri_un.unsi_addr.ua_sun.sun_path);
	    } else
		(void) snpf(Namech, Namechl, "no address");
	    break;

	/*
	 * Process a ROUTE domain socket.
	 */
	case AF_ROUTE:
	    (void) snpf(Lf->type, sizeof(Lf->type), "rte");
//??	    if (s.so_pcb)
//??		enter_dev_ch(print_kptr((KA_T)(s.so_pcb), (char *)NULL, 0));
//??	    else
//??		(void) snpf(Namech, Namechl, "no protocol control block");
	    if (!Fsize)
		Lf->off_def = 1;
	    break;

	/*
	 * Process an NDRV domain socket.
	 */
	case AF_NDRV:
	    (void) snpf(Lf->type, sizeof(Lf->type), "ndrv");

	    if (socketi.psi.soi_kind != SOCKINFO_NDRV) {
		break;
	    }

	    enter_dev_ch(print_kptr((KA_T)socketi.psi.soi_pcb, (char *)NULL, 0));

	    socketi.psi.soi_proto.pri_ndrv.ndrvsi_if_name[sizeof(socketi.psi.soi_proto.pri_ndrv.ndrvsi_if_name) - 1] = '\0';
	    (void) snpf(Namech, Namechl, "-> %s%d",
			socketi.psi.soi_proto.pri_ndrv.ndrvsi_if_name,
			socketi.psi.soi_proto.pri_ndrv.ndrvsi_if_unit);
	    break;

	/*
	 * Process an [internal] key-management function socket
	 */
	case pseudo_AF_KEY:
	    (void) snpf(Lf->type, sizeof(Lf->type), "key");

	    enter_dev_ch(print_kptr((KA_T)socketi.psi.soi_pcb, (char *)NULL, 0));

	    break;

	/*
	 * Process a SYSTEM domain socket
	 */
	case AF_SYSTEM:
	    (void) snpf(Lf->type, sizeof(Lf->type), "systm");

	    if (socketi.psi.soi_kind != SOCKINFO_KERN_EVENT) {
		break;
	    }

	    enter_dev_ch(print_kptr((KA_T)socketi.psi.soi_pcb, (char *)NULL, 0));

	    (void) snpf(Namech, Namechl, "[%x:%x:%x]",
			socketi.psi.soi_proto.pri_kern_event.kesi_vendor_code_filter,
			socketi.psi.soi_proto.pri_kern_event.kesi_class_filter,
			socketi.psi.soi_proto.pri_kern_event.kesi_subclass_filter);
	    break;

	/*
	 * Process a PPP domain socket
	 */
	case AF_PPP:
	    (void) snpf(Lf->type, sizeof(Lf->type), "ppp");

	    enter_dev_ch(print_kptr((KA_T)socketi.psi.soi_pcb, (char *)NULL, 0));

	    break;

	default:
	    printunkaf(fam, 1);

	}

	if (Namech[0])
	    enter_nm(Namech);

	return;
}
