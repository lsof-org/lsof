/* @(#) socket_pstat_body.h $Date: 2004/08/05 11:41:55 $Revision: i380/2 PATCH_11.23 (PHKL_31500) */
/*
 * BEGIN_DESC
 *
 *  File:
 *      @(#)    common/pstat/socket_pstat_body.h
 *
 *  Purpose:
 * 	pst_socket body declarations and user manifest constants 
 *
 *  Classification:                     Release to Release Consistency Req:
 *      public                                  binary & source
 *
 *  BE header:  yes
 *
 *  Shipped:  yes
 *      /usr/include/pstat/socket_pstat_body.h
 *
 * END_DESC
 */

/*
 * Constants
 *
 * PS_SOCK_STREAM, PS_SOCK_DGRAM, PS_SOCK_RAW, PS_SOCK_RDM, PS_SOCK_SEQPACKET, 
 * PS_SOCK_ISASTREAM, PS_SO_DEBUG, PS_SO_ACCEPTCONN, PS_SO_REUSEADDR, 
 * PS_SO_GETIFADDR, PS_SO_PMTU, PS_SO_INPCB_COPY, PS_AF_UNSPEC, PS_AF_UNIX, 
 * PS_AF_INET, PS_AF_IMPLINK, PS_AF_PUP, PS_AF_CHAOS, PS_AF_NS, PS_AF_NBS, 
 * PS_AF_ECMA, PS_AF_DATAKIT, PS_AF_CCITT, PS_AF_SNA, PS_AF_DECnet, PS_AF_DLI, 
 * PS_AF_LAT, PS_AF_HYLINK, PS_AF_APPLETALK, PS_AF_OTS, PS_AF_NIT, 
 * PS_AF_VME_LINK, PS_AF_KEY, PS_AF_POLICY, PS_AF_INET6, 
 * PS_SS_NOFDREF, PS_SS_ISCONNECTED, PS_SS_ISCONNECTING, PS_SS_ISDISCONNECTI, 
 * PS_SS_CANTSENDMORE, PS_SS_CANTRCVMORE, PS_SS_NOUSER, PS_SS_BOUND, 
 * PS_SS_XOPEN_EXT1, PS_PROTO_IP, PS_PROTO_ICMP, PS_PROTO_IGMP, PS_PROTO_GGP, 
 * PS_PROTO_IPIP, PS_PROTO_TCP, PS_PROTO_EGP, PS_PROTO_IGP, PS_PROTO_PUP, 
 * PS_PROTO_UDP, PS_PROTO_IDP, PS_PROTO_XTP, PS_PROTO_ESP, PS_PROTO_AH, 
 * PS_PROTO_OSPF, PS_PROTO_IPENCAP, PS_PROTO_ENCAP, PS_PROTO_PXP, 
 * PS_PROTO_RAW, PS_ADDR_SZ 
 *
 * Size-invariant types
 *  Defines:
 *	none;
 *
 * Size-variant types
 *  Defines:
 *	__pst_socket
 *
 *  Uses:
 *	_T_LONG_T
 *
 */

#if !defined(_PST_SOCKET_CONSTANTS_DEFINED)
#define _PST_SOCKET_CONSTANTS_DEFINED

/* 
 * Types of sockets (for pst_type) 
 */
#define PS_SOCK_STREAM     1        /* stream socket */
#define PS_SOCK_DGRAM      2        /* datagram socket */
#define PS_SOCK_RAW        3        /* raw-protocol interface */
#define PS_SOCK_RDM        4        /* reliably-delivered message */
#define PS_SOCK_SEQPACKET  5        /* sequenced packet stream */

#define PS_SOCK_ISASTREAM  1	  /* Is a stream? */

/* 
 * Option flags per-socket (for pst_options)
 */
#define PS_SO_DEBUG        0x0001 /* turn on debugging info recording */
#define PS_SO_ACCEPTCONN   0x0002 /* socket has had listen() */
#define PS_SO_REUSEADDR    0x0004 /* allow local address reuse */
#define PS_SO_KEEPALIVE    0x0008 /* keep connections alive */
#define PS_SO_DONTROUTE    0x0010 /* just use interface addresses */
#define PS_SO_BROADCAST    0x0020 /* permit sending of broadcast msgs */
#define PS_SO_USELOOPBACK  0x0040 /* bypass hardware when possible */
#define PS_SO_LINGER       0x0080 /* linger on close if data present */
#define PS_SO_OOBINLINE    0x0100 /* leave received OOB data in line */
#define PS_SO_REUSEPORT    0x0200 /* allow local address & port reuse */
#define PS_SO_GETIFADDR    0x0400 /* 
				   * HP: save the loc IP addr for 
				   * inb UDP packets 
				   */
#define PS_SO_PMTU         0x0800 /* HP: advise to use PMTU discovery */
#define PS_SO_INPCB_COPY   0x1000 /* HP: local copy of inpcb      */

/* 
 * Address families (for pst_family)
 */
#define PS_AF_UNSPEC       0      /* unspecified */
#define PS_AF_UNIX         1      /* local to host (pipes, portals) */
#define PS_AF_INET         2      /* internetwork: UDP, TCP, etc. */
#define PS_AF_IMPLINK      3      /* arpanet imp addresses */
#define PS_AF_PUP          4      /* pup protocols: e.g. BSP */
#define PS_AF_CHAOS        5      /* mit CHAOS protocols */
#define PS_AF_NS           6      /* XEROX NS protocols */
#define PS_AF_NBS          7      /* nbs protocols */
#define PS_AF_ECMA         8      /* european computer manufacturers */
#define PS_AF_DATAKIT      9      /* datakit protocols */
#define PS_AF_CCITT        10     /* CCITT protocols, X.25 etc */
#define PS_AF_SNA          11     /* IBM SNA */
#define PS_AF_DECnet       12     /* DECnet */
#define PS_AF_DLI          13     /* Direct data link interface */
#define PS_AF_LAT          14     /* LAT */
#define PS_AF_HYLINK       15     /* NSC Hyperchannel */
#define PS_AF_APPLETALK    16     /* Apple Talk */
#define PS_AF_OTS          17     /* Used for OSI in the ifnets */
#define PS_AF_NIT          18     /* NIT */
#define PS_AF_VME_LINK     19     /* VME backplane protocols */
#define PS_AF_KEY          20     /* IPSec PF_KEY protocol */
#define PS_AF_POLICY       21     /* IPSec PF_POLICY protocol */
#define PS_AF_INET6        22     /* IPv6 protocol */

/*
 * Socket state bits (for pst_state)
 */
#define PS_SS_NOFDREF         0x0001  /* no file table ref any more */
#define PS_SS_ISCONNECTED     0x0002  /* socket connected to a peer */
#define PS_SS_ISCONNECTING    0x0004  /* of connecting to peer */
#define PS_SS_ISDISCONNECTI   0x0008  /* in process of disconnecting */
#define PS_SS_CANTSENDMORE    0x0010  /* can't send more data to peer */
#define PS_SS_CANTRCVMORE     0x0020  /* can't receive more data from peer */
#define PS_SS_RCVATMARK       0x0040  /* at mark on input */
#define PS_SS_PRIV            0x0080  /* privileged for broadcast, raw... */
#define PS_SS_NBIO            0x0100  /* non-blocking ops */
#define PS_SS_ASYNC           0x0200  /* async i/o notify */
#define PS_SS_INTERRUPTED     0x0400  /* connect() was interrupted by signal */
#define PS_SS_NOWAIT          0x0800  /* no sleeps for any reason *HP*/
#define PS_SS_NOUSER          0x1000  /* no user context (u.) *HP*/
#define PS_SS_BOUND           0x2000  /* DGRAM socket is bound */
#define PS_SS_XOPEN_EXT1      0x4000  /* _XOPEN_EXTENDED == 1 */

/* 
 * Protocols (for pst_protocol)
 */
#define PS_PROTO_IP         0   /* dummy for IP */
#define PS_PROTO_ICMP       1   /* control message protocol */
#define PS_PROTO_IGMP       2   /* group mgmt protocol */
#define PS_PROTO_GGP        3   /* gateway^2 (deprecated) */
#define PS_PROTO_IPIP       4   /* IP in IP ecanpsulation */
#define PS_PROTO_TCP        6   /* tcp */
#define PS_PROTO_EGP        8   /* exterior gateway protocol */
#define PS_PROTO_IGP        9   /* interior gateway protocol */
#define PS_PROTO_PUP        12  /* pup */
#define PS_PROTO_UDP        17  /* user datagram protocol */
#define PS_PROTO_IDP        22  /* xns idp */
#define PS_PROTO_XTP        36  /* xtp */
#define PS_PROTO_ESP        50  /* IPSec encapsulated security payload*/
#define PS_PROTO_AH         51  /* IPSec authentication header */
#define PS_PROTO_OSPF       89  /* open SPF routing protocol */
#define PS_PROTO_IPENCAP    94  /* former IP in IP encap */
#define PS_PROTO_ENCAP      98  /* encapsulation header */
#define PS_PROTO_PXP        241 /* Netipc PXP protocol */
#define PS_PROTO_RAW        255 /* raw IP packet */

/* 
 * TCP states  for pst_pstate field if the socket is a TCP socket 
 */
#define PS_TCPS_CLOSED         -6
#define PS_TCPS_IDLE           -5  /* idle (opened, but not bound) */
#define PS_TCPS_BOUND          -4  /* bound, ready to connect or accept */
#define PS_TCPS_LISTEN         -3  /* listening for connection */
#define PS_TCPS_SYN_SENT       -2  /* active, have sent syn */
#define PS_TCPS_SYN_RCVD       -1  /* have received syn (and sent ours) */

/* 
 * States < PS_TCPS_ESTABLISHED are those where connections not established 
 */
#define PS_TCPS_ESTABLISHED    0   /* established */
#define PS_TCPS_CLOSE_WAIT     1   /* rcvd fin, waiting for close */

/* 
 * States > PS_TCPS_CLOSE_WAIT are those where user has closed 
 */
#define PS_TCPS_FIN_WAIT_1     2   /* have closed and sent fin */
#define PS_TCPS_CLOSING        3   /* closed, xchd FIN, await FIN ACK */
#define PS_TCPS_LAST_ACK       4   /* had fin and close; await FIN ACK */

/* 
 * States > PS_TCPS_CLOSE_WAIT && < PS_TCPS_FIN_WAIT_2 await ACK of FIN 
 */
#define PS_TCPS_FIN_WAIT_2     5   /* have closed, fin is acked */
#define PS_TCPS_TIME_WAIT      6   /* in 2*msl quiet wait after close */

/* 
 * Protocol state values (for pst_pstate)
 */
#define PS_TS_UNINIT 		0	/* uninitialized */
#define PS_TS_UNBND 		1	/* Unbound */
#define PS_TS_WACK_BREQ 	2	/* Wait for ack of BIND_REQ */
#define PS_TS_WACK_UREQ 	3	/* Wait for ack of UNBIND_REQ */
#define PS_TS_IDLE 		4	/* Ho hum */
#define PS_TS_WACK_OPTREQ 	5	/* Wait for ack of OPTMGMT_REQ */
#define PS_TS_WACK_CREQ 	6	/* Wait for ack of CONN_REQ */
#define PS_TS_WCON_CREQ 	7	/* Wait for confirm of CONN_REQ */
#define PS_TS_WRES_CIND 	8 	/* Wait for response to CONN_IND*/
#define PS_TS_WACK_CRES 	9 	/* Wait for ack of CONN_RES */
#define PS_TS_DATA_XFER 	10 	/* Data transfer */
#define PS_TS_WIND_ORDREL 	11 	/* Indication of RD/!WR release */
#define PS_TS_WREQ_ORDREL 	12 	/* Request for WR/!RD release */
#define PS_TS_WACK_DREQ6 	13 	/* Wait for DISCON_REQ ack */
#define PS_TS_WACK_DREQ7 	14 	/* ... */
#define PS_TS_WACK_DREQ9 	15 	/* ... */
#define PS_TS_WACK_DREQ10 	16 	/* ... */
#define PS_TS_WACK_DREQ11 	17 	/* ... */
#define PS_TS_WACK_ORDREL 	18 	/* Used internally  */
#define PS_TS_NOSTATES 		19 	/* Includes "missing" 0 */
	
#define PS_ADDR_SZ		256

#endif /* !defined(_PST_SOCKET_CONSTANTS_DEFINED) */

/* NO MULTIPLE-INCLUDE PROTECTION (intentional) */

struct __pst_socket {
     uint32_t    pst_hi_fileid;      /* File ID */
     uint32_t    pst_lo_fileid; 
     uint32_t    pst_hi_nodeid;      /* Socket node ID */
     uint32_t    pst_lo_nodeid; 
     uint32_t    pst_peer_hi_nodeid; /* Peer Socket node ID for AF_UNIX only */
     uint32_t    pst_peer_lo_nodeid; 
     _T_LONG_T   pst_flags;          /* Is a stream? */
     uint32_t    pst_type;           /* generic type */
     uint32_t    pst_options;      
     uint32_t    pst_linger;         /* time to linger while closing */
     uint32_t    pst_state;          
     uint32_t    pst_family;         /* domain protocol; a member of */
     uint32_t    pst_protocol;       /* protocol number for this addr 
				      * family(such as IPPROTO_*), if 
				      * applicable 
				      */
     uint32_t    pst_qlimit;         /* max #of queued connections */
     uint32_t    pst_qlen;           /* 
				      * # connections on incoming q ready 
				      * to be accepted 
				      */
     _T_ULONG_T  pst_idata;          /* inbound data available for reading */
     uint32_t    pst_ibufsz;         /* receive (inbound) data buffer size */
     _T_ULONG_T  pst_rwnd;           /* current receive window size */
     _T_ULONG_T  pst_odata;          /* unsent outbound data */
     uint32_t    pst_obufsz;         /* send (outbound) data buffer size */
     _T_ULONG_T  pst_swnd;           /* current send window size */
     uint32_t    pst_pstate;         /* protocol state */

     /*
      * The members pst_boundaddr and pst_remaddr contain data of the form
      * struct sockaddr, sockaddr_un, sockaddr_in, sockaddr_in6, etc. depending
      * on the socket family.
      */
     _T_ULONG_T  pst_boundaddr_len;  /* Length of pst_boundaddr */
     _T_ULONG_T  pst_remaddr_len;    /* Length of pst_remaddr */
     char        pst_boundaddr[PS_ADDR_SZ]; /* local addr/port, etc */
     char        pst_remaddr[PS_ADDR_SZ];   /* peer addr/port, etc */
};

struct __pst_socket_args {
        _T_PTR_T buf;
        _T_PTR_T id;
};
