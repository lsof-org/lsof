/*
 * dsock.c - FreeBSD socket processing functions for lsof
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


#include "lsof.h"


#if	defined(HASIPv6)

/*
 * IPv6_2_IPv4()  -- macro to define the address of an IPv4 address contained
 *		     in an IPv6 address
 */

#define IPv6_2_IPv4(v6)	(((uint8_t *)((struct in6_addr *)v6)->s6_addr)+12)

# if	defined(HAS_NO_6PORT)
/*
 * If the in_pcb structure no longer has the KAME accommodations of
 * in6p_[fl]port, redefine them to inp_[fl]port.
 */

#define	in6p_fport	inp_fport
#define	in6p_lport	inp_lport
# endif	/* defined(HAS_NO_6PORT) */

# if	defined(HAS_NO_6PPCB)
/*
 * If the in_pcb structure no longer has the KAME accommodation of in6p_pcb,
 * redefine it to inp_ppcb.
 */

#define	in6p_ppcb	inp_ppcb
# endif	/* defined(HAS_NO_6PPCB) */
#endif	/* defined(HASIPv6) */

#if	defined(HAS_SB_CCC)
#define	SOCK_CC	sb_ccc
#else	/* !defined(HAS_SB_CCC) */
#define	SOCK_CC	sb_cc
#endif	/* defined(HAS_SB_CCC) */


/*
 * Local function prototypes
 */

_PROTOTYPE(static int ckstate,(KA_T pcb, KA_T ta, struct tcpcb *t, int fam));


static int
cmp_xunpcb_sock_pcb(const void *a, const void *b) {
	const struct xunpcb *pcb1 = (const struct xunpcb *)a;
	const struct xunpcb *pcb2 = (const struct xunpcb *)b;

	if (pcb1->xu_socket.so_pcb < pcb2->xu_socket.so_pcb)
	    return -1;
	else if (pcb1->xu_socket.so_pcb > pcb2->xu_socket.so_pcb)
	    return 1;
	else
	    return 0;
}


static int
cmp_xtcpcb_sock_pcb(const void *a, const void *b) {
	const struct xtcpcb *pcb1 = (const struct xtcpcb *)a;
	const struct xtcpcb *pcb2 = (const struct xtcpcb *)b;

	if (pcb1->xt_inp.xi_socket.so_pcb < pcb2->xt_inp.xi_socket.so_pcb)
	    return -1;
	else if (pcb1->xt_inp.xi_socket.so_pcb > pcb2->xt_inp.xi_socket.so_pcb)
	    return 1;
	else
	    return 0;
}


static int
cmp_xinpcb_sock_pcb(const void *a, const void *b) {
	const struct xinpcb *pcb1 = (const struct xinpcb *)a;
	const struct xinpcb *pcb2 = (const struct xinpcb *)b;

	if (pcb1->xi_socket.so_pcb < pcb2->xi_socket.so_pcb)
	    return -1;
	else if (pcb1->xi_socket.so_pcb > pcb2->xi_socket.so_pcb)
	    return 1;
	else
	    return 0;
}


static int
get_unix_pcbs(const char *name, struct xunpcb **pcbs, size_t *n_pcbs)
{
	size_t len;
	char *buffer = NULL;
	struct xunpgen *ug = (struct xunpgen *)buffer;
	int count = 0;
	int ret = 1;

	if (sysctlbyname(name, NULL, &len, NULL, 0))
	    goto end;
	if ((buffer = malloc(len)) == NULL)
	    goto end;
	if (sysctlbyname(name, buffer, &len, NULL, 0))
	    goto end;
	for (ug = (struct xunpgen *)((char *)ug + ug->xug_len);
	     ug->xug_len == sizeof(struct xunpcb);
	     ug = (struct xunpgen *)((char *)ug + ug->xug_len)) {
	    ++count;
	}
	memmove(buffer, buffer + ((struct xunpgen *)buffer)->xug_len, count * sizeof(struct xunpcb));
	qsort(buffer, count, sizeof(struct xunpcb), cmp_xunpcb_sock_pcb);
	ret = 0;

end:
	*pcbs = (struct xunpcb *)buffer;
	*n_pcbs = count;
	if (ret)
	    free(buffer);
	return ret;
}


static int
get_tcp_pcbs(struct xtcpcb **pcbs, size_t *n_pcbs)
{
	size_t len;
	char *buffer = NULL;
	struct xinpgen *ig = (struct xinpgen *)buffer;
	int count = 0;
	int ret = 1;

	if (sysctlbyname("net.inet.tcp.pcblist", NULL, &len, NULL, 0))
	    goto end;
	if ((buffer = malloc(len)) == NULL)
	    goto end;
	if (sysctlbyname("net.inet.tcp.pcblist", buffer, &len, NULL, 0))
	    goto end;
	for (ig = (struct xinpgen *)((char *)ig + ig->xig_len);
	     ig->xig_len == sizeof(struct xtcpcb);
	     ig = (struct xinpgen *)((char *)ig + ig->xig_len)) {
	    ++count;
	}
	memmove(buffer, buffer + ((struct xinpgen *)buffer)->xig_len, count * sizeof(struct xtcpcb));
	qsort(buffer, count, sizeof(struct xtcpcb), cmp_xtcpcb_sock_pcb);
	ret = 0;

end:
	*pcbs = (struct xtcpcb *)buffer;
	*n_pcbs = count;
	if (ret)
	    free(buffer);
	return ret;
}


static int
get_udp_pcbs(struct xinpcb **pcbs, size_t *n_pcbs)
{
	size_t len;
	char *buffer = NULL;
	struct xinpgen *ig = (struct xinpgen *)buffer;
	int count = 0;
	int ret = 1;

	if (sysctlbyname("net.inet.udp.pcblist", NULL, &len, NULL, 0))
	    goto end;
	if ((buffer = malloc(len)) == NULL)
	    goto end;
	if (sysctlbyname("net.inet.udp.pcblist", buffer, &len, NULL, 0))
	    goto end;
	for (ig = (struct xinpgen *)((char *)ig + ig->xig_len);
	     ig->xig_len == sizeof(struct xinpcb);
	     ig = (struct xinpgen *)((char *)ig + ig->xig_len)) {
	    ++count;
	}
	memmove(buffer, buffer + ((struct xinpgen *)buffer)->xig_len, count * sizeof(struct xinpcb));
	qsort(buffer, count, sizeof(struct xinpcb), cmp_xinpcb_sock_pcb);
	ret = 0;

end:
	*pcbs = (struct xinpcb *)buffer;
	*n_pcbs = count;
	if (ret)
	    free(buffer);
	return 0;
}


struct pcb_lists*
read_pcb_lists(void)
{
	struct pcb_lists *pcbs;
	size_t len;
	int ret;
	int succeeded = 0;

	pcbs = calloc(1, sizeof(struct pcb_lists));
	if (!pcbs)
	    goto end;

	if (get_unix_pcbs("net.local.stream.pcblist", &pcbs->un_stream_pcbs, &pcbs->n_un_stream_pcbs))
	    goto end;
	if (get_unix_pcbs("net.local.dgram.pcblist", &pcbs->un_dgram_pcbs, &pcbs->n_un_dgram_pcbs))
	    goto end;
	if (get_unix_pcbs("net.local.seqpacket.pcblist", &pcbs->un_seqpacket_pcbs, &pcbs->n_un_seqpacket_pcbs))
	    goto end;
	if (get_tcp_pcbs(&pcbs->tcp_pcbs, &pcbs->n_tcp_pcbs))
	    goto end;
	if (get_udp_pcbs(&pcbs->udp_pcbs, &pcbs->n_udp_pcbs))
	    goto end;
	succeeded = 1;

end:
	if (!succeeded) {
	    free_pcb_lists(pcbs);
	    return NULL;
	}
	return pcbs;
}


void
free_pcb_lists(struct pcb_lists *pcbs)
{
	if (pcbs) {
	    free(pcbs->un_stream_pcbs);
	    free(pcbs->un_dgram_pcbs);
	    free(pcbs->un_seqpacket_pcbs);
	    free(pcbs->tcp_pcbs);
	    free(pcbs->udp_pcbs);
	}
	free(pcbs);
}


/*
 * ckstate() -- read TCP control block and check TCP state for inclusion
 *		or exclusion
 * return: -1 == no TCP CB available
 *	    0 == TCP DB available; continue processing file
 *	    1 == stop processing file
 */

static int
ckstate(pcb, ta, t, fam)
	KA_T pcb;			/* PCB address */
	KA_T ta;			/* TCP control block address */
	struct tcpcb *t;		/* TCP control block receptor */
	int fam;			/* protocol family */
{
	int tsnx;
/*
 * Read TCP control block.
 */
	if (kread(ta, (char *)t, sizeof(struct tcpcb))
	||  (KA_T)t->t_inpcb != pcb)
	{
	    return(-1);
	}
	if (TcpStXn || TcpStIn) {

	/*
	 * If there are TCP state inclusions or exclusions, check them.
	 */
	    if ((tsnx = (int)t->t_state + TcpStOff) >= TcpNstates)
		return(0);
	    if (TcpStXn) {
		if (TcpStX[tsnx]) {
		    Lf->sf &= ~SELNET;
		    Lf->sf |= SELEXCLF;
		    return(1);
		}
	    }
	    if (TcpStIn) {
		if (TcpStI[tsnx]) {
		    TcpStI[tsnx] = 2;
		    Lf->sf |= SELNET;
		} else {
		    Lf->sf &= ~SELNET;
		    Lf->sf |= SELEXCLF;
		    return(1);
		}
	    }
	}
	if (!(Lf->sf & SELNET) && !TcpStIn) {

	/*
	 * See if this TCP file should be selected.
	 */
	    if (Fnet) {
		if (!FnetTy
		||  ((FnetTy == 4) && (fam == AF_INET))

#if	defined(HASIPv6)
		||  ((FnetTy == 6) && (fam == AF_INET6))
#endif 	/* defined(HASIPv6) */

		) {
		    Lf->sf |= SELNET;
		}
	    }
	}
	return(0);
}


/*
 * process_socket() - process socket
 */

void
process_socket(sa)
	KA_T sa;			/* socket address in kernel */
{
	struct domain d;
	unsigned char *fa = (unsigned char *)NULL;
	int fam;
	int fp, lp;
	struct inpcb inp;
	unsigned char *la = (unsigned char *)NULL;
	struct protosw p;
	struct socket s;
	struct tcpcb t;
	int ts = -1;
	struct unpcb uc, unp;
	struct sockaddr_un *ua = NULL;
	struct sockaddr_un un;

	int unl;

#if	defined(HASIPv6) && !defined(HASINRIAIPv6)
	struct inpcb in6p;
#endif	/* defined(HASIPv6) && !defined(HASINRIAIPv6) */

	(void) snpf(Lf->type, sizeof(Lf->type), "sock");
	Lf->inp_ty = 2;
/*
 * Read the socket, protocol, and domain structures.
 */
	if (!sa) {
	    enter_nm("no socket address");
	    return;
	}
	if (kread(sa, (char *) &s, sizeof(s))) {
	    (void) snpf(Namech, Namechl, "can't read socket struct from %s",
		print_kptr(sa, (char *)NULL, 0));
	    enter_nm(Namech);
	    return;
	}
	if (!s.so_type) {
	    enter_nm("no socket type");
	    return;
	}
	if (!s.so_proto
	||  kread((KA_T)s.so_proto, (char *)&p, sizeof(p))) {
	    (void) snpf(Namech, Namechl, "can't read protocol switch from %s",
		print_kptr((KA_T)s.so_proto, (char *)NULL, 0));
	    enter_nm(Namech);
	    return;
	}
	if (!p.pr_domain
	||  kread((KA_T)p.pr_domain, (char *)&d, sizeof(d))) {
	    (void) snpf(Namech, Namechl, "can't read domain struct from %s",
		print_kptr((KA_T)p.pr_domain, (char *)NULL, 0));
	    enter_nm(Namech);
	    return;
	}
/*
 * Save size information.
 */
	if (Fsize) {
	    if (Lf->access == 'r')
		Lf->sz = (SZOFFTYPE)s.so_rcv.SOCK_CC;
	    else if (Lf->access == 'w')
		Lf->sz = (SZOFFTYPE)s.so_snd.SOCK_CC;
	    else
		Lf->sz = (SZOFFTYPE)(s.so_rcv.SOCK_CC + s.so_snd.SOCK_CC);
	    Lf->sz_def = 1;
	} else
	    Lf->off_def = 1;

#if	defined(HASTCPTPIQ)
	Lf->lts.rq = s.so_rcv.SOCK_CC;
	Lf->lts.sq = s.so_snd.SOCK_CC;
	Lf->lts.rqs = Lf->lts.sqs = 1;
#endif	/* defined(HASTCPTPIQ) */

#if	defined(HASSOOPT)
	Lf->lts.ltm = (unsigned int)s.so_linger;
	Lf->lts.opt = (unsigned int)s.so_options;

# if	__FreeBSD_version>=1200027
	if (s.so_options & SO_ACCEPTCONN) {
	    Lf->lts.pqlen = (unsigned int)s.sol_incqlen;
	    Lf->lts.qlen = (unsigned int)s.sol_qlen;
	    Lf->lts.qlim = (unsigned int)s.sol_qlimit;
	} else {
	    Lf->lts.rbsz = (unsigned long)s.so_rcv.sb_mbmax;
	    Lf->lts.sbsz = (unsigned long)s.so_snd.sb_mbmax;

#  if	defined(HASSBSTATE)
	    Lf->lts.sbs_rcv = s.so_rcv.sb_state;
	    Lf->lts.sbs_snd = s.so_snd.sb_state;
#  endif	/* defined(HASSBSTATE) */

	}

# else	/* __FreeBSD_version<1200027 */
	Lf->lts.pqlen = (unsigned int)s.so_incqlen;
	Lf->lts.qlen = (unsigned int)s.so_qlen;
	Lf->lts.qlim = (unsigned int)s.so_qlimit;
	Lf->lts.rbsz = (unsigned long)s.so_rcv.sb_mbmax;
	Lf->lts.sbsz = (unsigned long)s.so_snd.sb_mbmax;

#  if	defined(HASSBSTATE)
	Lf->lts.sbs_rcv = s.so_rcv.sb_state;
	Lf->lts.sbs_snd = s.so_snd.sb_state;
#  endif	/* defined(HASSBSTATE) */
# endif	/*__FreeBSD_version>=1200027 */

	Lf->lts.pqlens = Lf->lts.qlens = Lf->lts.qlims = Lf->lts.rbszs
		       = Lf->lts.sbszs = (unsigned char)1;

# if	defined(HASSOSTATE)
	Lf->lts.ss = (unsigned int)s.so_state;
# endif	/* defined(HASSOSTATE) */
#endif	/* defined(HASSOPT) */

/*
 * Process socket by the associated domain family.
 */
	switch ((fam = d.dom_family)) {
/*
 * Process an Internet domain socket.
 */
	case AF_INET:

#if	defined(HASIPv6)
	case AF_INET6:
#endif	/* defined(HASIPv6) */

	    if (Fnet) {
		if (!FnetTy
		||  ((FnetTy == 4) && (fam == AF_INET))

#if	defined(HASIPv6)
		||  ((FnetTy == 6) && (fam == AF_INET6))
#endif	/* defined(HASIPv6) */

		) {
		    if (!TcpStIn && !UdpStIn)
			Lf->sf |= SELNET;
		}
	    }
	    printiproto(p.pr_protocol);

#if	defined(HASIPv6)
	    (void) snpf(Lf->type, sizeof(Lf->type),
		(fam == AF_INET) ? "IPv4" : "IPv6");
#else	/* !defined(HASIPv6) */
	    (void) snpf(Lf->type, sizeof(Lf->type), "inet");
#endif	/* defined(HASIPv6) */

#if	defined(HASIPv6) && !defined(HASINRIAIPv6)
	    if (fam == AF_INET6) {

	    /*
	     * Read IPv6 protocol control block.
	     */
		if (!s.so_pcb
		||  kread((KA_T)s.so_pcb, (char *)&in6p, sizeof(in6p))) {
		    (void) snpf(Namech, Namechl, "can't read in6pcb at %s",
			print_kptr((KA_T)s.so_pcb, (char *)NULL, 0));
		    enter_nm(Namech);
		    return;
		}
	    /*
	     * Save IPv6 address information.
	     */
		if (p.pr_protocol == IPPROTO_TCP) {
		    if (in6p.in6p_ppcb) {
			if ((ts = ckstate((KA_T)s.so_pcb, (KA_T)in6p.in6p_ppcb,
					  &t, fam)) == 1)
			{
			    return;
			}
		    }
		}
		enter_dev_ch(print_kptr((KA_T)(in6p.in6p_ppcb ? in6p.in6p_ppcb
							      : s.so_pcb),
					       (char *)NULL, 0));
	        la = (unsigned char *)&in6p.in6p_laddr;
	        lp = (int)ntohs(in6p.in6p_lport);
		if (!IN6_IS_ADDR_UNSPECIFIED(&in6p.in6p_faddr)
		||  in6p.in6p_fport)
		{
		    fa = (unsigned char *)&in6p.in6p_faddr;
		    fp = (int)ntohs(in6p.in6p_fport);
		}
	    } else
#endif	/* defined(HASIPv6) && !defined(HASINRIAIPv6) */

	    {

	    /*
	     * Read Ipv4 or IPv6 (INRIA) protocol control block.
	     */
		if (!s.so_pcb
		||  kread((KA_T) s.so_pcb, (char *) &inp, sizeof(inp))) {
		    if (!s.so_pcb) {
			(void) snpf(Namech, Namechl, "no PCB%s%s",

#if	defined(HASSBSTATE)
			    (s.so_snd.sb_state & SBS_CANTSENDMORE) ?
#else	/* !defined(HASSBSTATE) */
			    (s.so_state & SS_CANTSENDMORE) ?
#endif	/* defined(HASSBSTATE) */

				", CANTSENDMORE" : "",
#if	defined(HASSBSTATE)
			    (s.so_rcv.sb_state & SBS_CANTRCVMORE) ?
#else	/* !defined(HASSBSTATE) */
			    (s.so_state & SS_CANTRCVMORE) ?
#endif	/* defined(HASSBSTATE) */

				", CANTRCVMORE" : "");
		    } else {
			(void) snpf(Namech, Namechl, "can't read inpcb at %s",
			    print_kptr((KA_T)s.so_pcb, (char *)NULL, 0));
		    }
		    enter_nm(Namech);
		    return;
		}
		if (p.pr_protocol == IPPROTO_TCP) {
		    if (inp.inp_ppcb) {
			if ((ts = ckstate((KA_T)s.so_pcb, (KA_T)inp.inp_ppcb,
					  &t, fam)) == 1)
			{
			    return;
			}
		    }
		}
		enter_dev_ch(print_kptr((KA_T)(inp.inp_ppcb ? inp.inp_ppcb
							    : s.so_pcb),
					       (char *)NULL, 0));
		lp = (int)ntohs(inp.inp_lport);
		if (fam == AF_INET) {

		/*
		 * Save IPv4 address information.
		 */
		    la = (unsigned char *)&inp.inp_laddr;
		    if (inp.inp_faddr.s_addr != INADDR_ANY || inp.inp_fport) {
			fa = (unsigned char *)&inp.inp_faddr;
			fp = (int)ntohs(inp.inp_fport);
		    }
		}

#if	defined(HASIPv6) && defined(HASINRIAIPv6)
		else {
		    la = (unsigned char *)&inp.inp_laddr6;
		    if (!IN6_IS_ADDR_UNSPECIFIED(&inp.inp_faddr6)
		    ||  inp.inp_fport)
		    {
			fa = (unsigned char *)&inp.inp_faddr6;
			fp = (int)ntohs(inp.inp_fport);
		    }
		}
#endif	/* defined(HASIPv6) && defined(HASINRIAIPv6) */

	    }


#if	defined(HASIPv6)
	    if ((fam == AF_INET6)
	    &&  ((la && IN6_IS_ADDR_V4MAPPED((struct in6_addr *)la))
	    ||  ((fa && IN6_IS_ADDR_V4MAPPED((struct in6_addr *)fa))))) {

	    /*
	     * Adjust for IPv4 addresses mapped in IPv6 addresses.
	     */
		if (la)
		    la = (unsigned char *)IPv6_2_IPv4(la);
		if (fa)
		    fa = (unsigned char *)IPv6_2_IPv4(fa);
		fam = AF_INET;
	    }
#endif	/* defined(HASIPv6) */

	/*
 	 * Enter local and remote addresses by address family.
 	 */
	    if (fa || la)
		(void) ent_inaddr(la, lp, fa, fp, fam);
	    if (ts == 0) {
		Lf->lts.type = 0;
		Lf->lts.state.i = (int)t.t_state;

#if	defined(HASTCPOPT)
		Lf->lts.mss = (unsigned long)t.t_maxseg;
		Lf->lts.msss = (unsigned char)1;
		Lf->lts.topt = (unsigned int)t.t_flags;
#endif	/* defined(HASTCPOPT) */

	    }
	    break;
/*
 * Process a ROUTE domain socket.
 */
	case AF_ROUTE:
	    (void) snpf(Lf->type, sizeof(Lf->type), "rte");
	    if (s.so_pcb)
		enter_dev_ch(print_kptr((KA_T)(s.so_pcb), (char *)NULL, 0));
	    else
		(void) snpf(Namech, Namechl, "no protocol control block");
	    if (!Fsize)
		Lf->off_def = 1;
	    break;
/*
 * Process a Unix domain socket.
 */
	case AF_UNIX:
	    if (Funix)
		Lf->sf |= SELUNX;
	    (void) snpf(Lf->type, sizeof(Lf->type), "unix");
	/*
	 * Read Unix protocol control block and the Unix address structure.
	 */

	    enter_dev_ch(print_kptr(sa, (char *)NULL, 0));
	    if (kread((KA_T) s.so_pcb, (char *) &unp, sizeof(unp))) {
		(void) snpf(Namech, Namechl, "can't read unpcb at %s",
		    print_kptr((KA_T)s.so_pcb, (char *)NULL, 0));
		break;
	    }
	    if ((struct socket *)sa != unp.unp_socket) {
		(void) snpf(Namech, Namechl, "unp_socket (%s) mismatch",
		    print_kptr((KA_T)unp.unp_socket, (char *)NULL, 0));
		break;
	    }
	    if (unp.unp_addr) {

		if (kread((KA_T)unp.unp_addr, (char *)&un, sizeof(un)))

		{
		    (void) snpf(Namech, Namechl, "can't read unp_addr at %s",
			print_kptr((KA_T)unp.unp_addr, (char *)NULL, 0));
		    break;
		}

		ua = &un;

	    }
	    if (!ua) {
		ua = &un;
		(void) bzero((char *)ua, sizeof(un));
		ua->sun_family = AF_UNSPEC;
	    }
	/*
	 * Print information on Unix socket that has no address bound
	 * to it, although it may be connected to another Unix domain
	 * socket as a pipe.
	 */
	    if (ua->sun_family != AF_UNIX) {
		if (ua->sun_family == AF_UNSPEC) {
		    if (unp.unp_conn) {
			if (kread((KA_T)unp.unp_conn, (char *)&uc, sizeof(uc)))
			    (void) snpf(Namech, Namechl,
				"can't read unp_conn at %s",
				print_kptr((KA_T)unp.unp_conn,(char *)NULL,0));
			else
			    (void) snpf(Namech, Namechl, "->%s",
				print_kptr((KA_T)uc.unp_socket,(char *)NULL,0));
		    } else
			(void) snpf(Namech, Namechl, "->(none)");
		} else
		    (void) snpf(Namech, Namechl, "unknown sun_family (%d)",
			ua->sun_family);
		break;
	    }
	    if (ua->sun_path[0]) {

		unl = ua->sun_len - offsetof(struct sockaddr_un, sun_path);
		if ((unl < 0) || (unl >= sizeof(ua->sun_path)))
		    unl = sizeof(ua->sun_path) - 1;
		ua->sun_path[unl] = '\0';

		if (ua->sun_path[0] && Sfile && is_file_named(ua->sun_path, 0))
		    Lf->sf |= SELNM;
		if (ua->sun_path[0] && !Namech[0])
		    (void) snpf(Namech, Namechl, "%s", ua->sun_path);
	    } else
		(void) snpf(Namech, Namechl, "no address");
	    break;
	default:
	    printunkaf(fam, 1);
	}
	if (Namech[0])
	    enter_nm(Namech);
}
