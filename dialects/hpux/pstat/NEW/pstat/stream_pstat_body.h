/* @(#) stream_pstat_body.h $Date: 2007/05/24 10:45:58 $Revision: r11.23/1 PATCH_11.23 (PHKL_36577) */
/*
 * BEGIN_DESC
 *
 *  File:
 *         common/include/sys/pstat/stream_pstat_body.h
 *
 *  Purpose:
 *      pst_stream body declarations and user constants
 *
 *  Classification:                     Release to Release Consistency Req:
 *      public                                  binary & source
 *
 *  BE header:  yes
 *
 *  Shipped:  yes
 *      /usr/include/sys/pstat/stream_pstat_body.h
 *
 * END_DESC
 */

/*
 * Constants
 *
 * PS_STRMODNAME_SZ
 *
 * Size-invariant types
 *  Defines:
 *	pst_streamentity_t;
 *
 * Size-variant types
 *  Defines:
 *	__pst_stream
 *
 *  Uses:
 *	_T_LONG_T
 *	_T_ULONG_T
 *	_T_PTR_T
 */

#if !defined(_PST_STREAM_CONSTANTS_DEFINED)
#define _PST_STREAM_CONSTANTS_DEFINED

typedef enum {
	PS_STR_HEAD,
	PS_STR_MODULE,
	PS_STR_DRIVER
} pst_streamentity_t;

#define PS_STRMODNAME_SZ          32
#define PS_TRANSPORT_ADDR_SZ      256 /* Transport local/remote addr/port etc */


#define PS_STR_ISACLONE 0x0001	/* Is stream a clone device? */

/*
 * pst_extn_flags
 */
#define PS_STR_XPORT_DATA       0x00000001

/*
 * pst_str_xport_family: Address family
 */
#define PS_STR_XPORT_AFINET 	1 /* IPv4 address */
#define	PS_STR_XPORT_AFINET6	2 /* IPv6 address */

/*
 * pst_str_xport_protocol
 */
#define PS_STR_XPORT_TCP_PROTO  1 /* TCP protocol */
#define	PS_STR_XPORT_UDP_PROTO 	2 /* UDP protocol */


/*
 * pst_str_xport_pstate
 */

/*
 * TCP states for pst_str_xport_pstate field if the fd is a TCP end-point
 */
#define PS_STR_XPORT_TCPS_CLOSED   -6 /* inactive */
#define PS_STR_XPORT_TCPS_IDLE     -5 /* idle (opened, but not bound) */
#define PS_STR_XPORT_TCPS_BOUND    -4 /* ready to connect or accept */
#define PS_STR_XPORT_TCPS_LISTEN   -3 /* listening for connection */
#define PS_STR_XPORT_TCPS_SYN_SENT -2 /* active, have sent syn */
#define PS_STR_XPORT_TCPS_SYN_RCVD -1 /* have received syn (& sent ours) */

/*
 * States < PS_STR_XPORT_TCPS_ESTABLISHED are those where
 * Connections not established
 */
#define PS_STR_XPORT_TCPS_ESTABLISHED    0 /* established */
#define PS_STR_XPORT_TCPS_CLOSE_WAIT     1 /* rcvd fin, waiting for close*/

/*
 * States > PS_STR_XPORT_TCPS_CLOSE_WAIT are those where user
 * has closed
 */
#define PS_STR_XPORT_TCPS_FIN_WAIT_1  	2 /* have closed and sent fin */
#define PS_STR_XPORT_TCPS_CLOSING     	3 /* closed, xchd FIN, await FIN ACK */
#define PS_STR_XPORT_TCPS_LAST_ACK      4 /* had fin and close; await FIN ACK */
 

/*
 * States > PS_TCPS_CLOSE_WAIT && < PS_TCPS_FIN_WAIT_2 await ACK of FIN
 */
#define PS_STR_XPORT_TCPS_FIN_WAIT_2     5   /* have closed, fin is acked */
#define PS_STR_XPORT_TCPS_TIME_WAIT      6   /* in 2*msl quiet wait after close */

/*
 * UDP states for pst_str_xport_pstate field if the fd is a UDP end-point.
 */
#define PS_STR_XPORT_TS_UNINIT        0  /* uninitialized */
#define PS_STR_XPORT_TS_UNBND         1  /* Unbound */
#define PS_STR_XPORT_TS_WACK_BREQ     2  /* Wait for ack of BIND_REQ */
#define PS_STR_XPORT_TS_WACK_UREQ     3  /* Wait for ack of UNBIND_REQ */
#define PS_STR_XPORT_TS_IDLE          4  /* Ho hum */
#define PS_STR_XPORT_TS_WACK_OPTREQ   5  /* Wait for ack of OPTMGMT_REQ */
#define PS_STR_XPORT_TS_WACK_CREQ     6  /* Wait for ack of CONN_REQ */
#define PS_STR_XPORT_TS_WCON_CREQ     7  /* Wait for confirm of CONN_REQ */
#define PS_STR_XPORT_TS_WRES_CIND     8  /* Wait for response to CONN_IND*/
#define PS_STR_XPORT_TS_WACK_CRES     9  /* Wait for ack of CONN_RES */
#define PS_STR_XPORT_TS_DATA_XFER     10 /* Data transfer */
#define PS_STR_XPORT_TS_WIND_ORDREL   11 /* Indication of RD/!WR release */
#define PS_STR_XPORT_TS_WREQ_ORDREL   12 /* Request for WR/!RD release */
#define PS_STR_XPORT_TS_WACK_DREQ6    13 /* Wait for DISCON_REQ ack */
#define PS_STR_XPORT_TS_WACK_DREQ7    14 /* ... */
#define PS_STR_XPORT_TS_WACK_DREQ9    15 /* ... */
#define PS_STR_XPORT_TS_WACK_DREQ10   16 /* ... */
#define PS_STR_XPORT_TS_WACK_DREQ11   17 /* ... */
#define PS_STR_XPORT_TS_WACK_ORDREL   18 /* Used internally */
#define PS_STR_XPORT_TS_NOSTATES      19 /* Includes "missing" 0 */

#endif 				/* !defined(_PST_STREAM_CONSTANTS_DEFINED) */

/* 
 * NO MULTIPLE-INCLUDE PROTECTION (intentional) 
 */

struct __pst_stream {
	pst_streamentity_t type;
	union {
		struct { /* head Information */
			uint32_t pst_hi_fileid; /* Per file ID*/
			uint32_t pst_lo_fileid;
			uint32_t pst_hi_nodeid; /* Per vnode ID*/
			uint32_t pst_lo_nodeid;
			_T_ULONG_T pst_rbytes;  /* queued bytes on read side */
			_T_ULONG_T pst_wbytes;  /* queued bytes on write side */
			_T_ULONG_T pst_flag;    /* Is a clone? PS_STR_ISACLONE*/
			_T_LONG_T pst_dev_major;/* major number */
			_T_LONG_T pst_dev_minor;/* minor number */
			_T_LONG_T pst_dev_seq;  /* clone driver sequence */
		} head;
		struct { /* Module Information */
			_T_ULONG_T pst_rbytes; /* queued bytes on read side */
			_T_ULONG_T pst_wbytes; /* queued bytes on write side */
			char pst_name[PS_STRMODNAME_SZ]; /* NULL terminated */
		} module;
		struct { /* driver information */
			_T_ULONG_T pst_rbytes; /* queued bytes on read side */
			_T_ULONG_T pst_wbytes; /* queued bytes on write side */
			char pst_name[PS_STRMODNAME_SZ]; /* NULL terminated */
		} driver;
	} val;

#if defined(_PSTAT_STREAM_GET_XPORT) || defined(_KERNEL)
     uint32_t    pst_extn_flags;      /* Flag to indicate Xport data avail */
     uint32_t    pst_str_xport_linger;/* time to linger while closing */
     uint32_t    pst_str_xport_family;/* domain protocol; a member of */
     uint32_t    pst_str_xport_protocol;/* protocol number for this addr 
				         * family, if applicable 
				         */
     uint32_t    pst_str_xport_qlimit; /* max #of queued connections */
     uint32_t    pst_str_xport_qlen;   /* 
				        * # connections on incoming q ready 
				        * to be accepted 
				        */
     uint32_t    pst_str_xport_ibufsz; /* max actual char count */
     _T_ULONG_T  pst_str_xport_idata;  /* actual chars in inbound buffer*/
     _T_ULONG_T  pst_str_xport_rwnd;   /* advertised recv window,if applicable*/
     _T_ULONG_T  pst_str_xport_swnd;   /* current send window, if applicable */
     _T_ULONG_T  pst_str_xport_odata;  /* actual chars in outbound buffer */ 
     uint32_t    pst_str_xport_obufsz; /* max Transmit hiwat */ 
     int32_t     pst_str_xport_pstate; /* protocol state */

     /*
      * The members pst_boundaddr and pst_remaddr contain data of the form
      * struct sockaddr_in, sockaddr_in6, etc. depending on the address family
      */
     _T_ULONG_T  pst_str_xport_boundaddr_len;  /* Length of pst_boundaddr */
     _T_ULONG_T  pst_str_xport_remaddr_len;    /* Length of pst_remaddr */
     char        pst_str_xport_boundaddr[PS_TRANSPORT_ADDR_SZ]; 
                                               /* local addr/port, etc */
     char        pst_str_xport_remaddr[PS_TRANSPORT_ADDR_SZ]; 
                                               /* peer addr/port, etc */
#endif

};

struct __pst_stream_args {
	_T_PTR_T buf;  /* pointer to user's buffer */
	_T_PTR_T id;   /* pointer to pst_fid */
};
