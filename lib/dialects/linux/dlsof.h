/*
 * dlsof.h - Linux header file for /proc-based lsof
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

/*
 * $Id: dlsof.h,v 1.23 2015/07/07 19:46:33 abe Exp $
 */

#if !defined(LINUX_LSOF_H)
#    define LINUX_LSOF_H 1

#    include <dirent.h>
#    define DIRTYPE dirent /* for arg.c's enter_dir() */
#    define __USE_GNU      /* to get all O_* symbols in fcntl.h */
#    include <fcntl.h>
#    include <malloc.h>
#    include <signal.h>
#    include <stdlib.h>
#    include <string.h>
#    include <setjmp.h>
#    include <unistd.h>
#    include <netinet/in.h>

#    if defined(GLIBCV) || defined(__UCLIBC__) || defined(NEEDS_NETINET_TCPH)
#        include <netinet/tcp.h>
#    else /* !defined(GLIBCV) && !defined(__UCLIBC__) &&                       \
             !defined(NEEDS_NETINET_TCPH) */
#        include <linux/tcp.h>
#    endif /* defined(GLIBCV) || defined(__UCLIBC__) ||                        \
              defined(NEEDS_NETINET_TCPH) */

#    if !defined(HASNORPC_H)
#        include <rpc/rpc.h>
#        include <rpc/pmap_prot.h>
#    endif /* !defined(HASNORPC_H) */

#    if defined(HASSELINUX)
#        include <selinux/selinux.h>
#    endif /* defined(HASSELINUX) */

#    include <sys/sysmacros.h>
#    include <sys/socket.h>
#    include <arpa/inet.h>
#    include <linux/if_ether.h>
#    include <linux/netlink.h>

#    include <sys/syscall.h>

/*
 * This definition is needed for the common function prototype definitions
 * in "proto.h", but isn't used in /proc-based lsof.
 */

typedef unsigned long KA_T;

/*
 * Local definitions
 */

#    define COMP_P const void
#    define DEVINCR 1024 /* device table malloc() increment */
#    define FSNAMEL 4
#    define MALLOC_P void
#    define FREE_P MALLOC_P
#    define MALLOC_S size_t
#    define MAXSYSCMDL                                                         \
        15 /* max system command name length                                   \
            *   This value should be obtained from a                           \
            * header file #define, but no consistent one                       \
            * exists.  Some versions of the Linux kernel                       \
            * have a hard-coded "char comm[16]" command                        \
            * name member of the task structured                               \
            * definition in <linux/sched.h>, while others                      \
            * have a "char comm[TASK_COMM_LEN]" member                         \
            * with TASK_COMM_LEN #define'd to be 16.                           \
            *   Hence, a universal, local definition of                        \
            * 16 is #define'd here. */
#    define PROCFS "/proc"
#    define QSORT_P void
#    define READLEN_T size_t

/*
 * Definitions that indicate what values are present in a stat(2) or lstat(2)
 * buffer.
 */

#    define SB_DEV 0x01   /* st_dev */
#    define SB_INO 0x02   /* st_ino */
#    define SB_MODE 0x04  /* st_mode */
#    define SB_NLINK 0x08 /* st_nlink */
#    define SB_RDEV 0x10  /* st_rdev */
#    define SB_SIZE 0x20  /* st_size */
#    define SB_ALL                                                             \
        (SB_DEV | SB_INO | SB_MODE | SB_NLINK | SB_RDEV |                      \
         SB_SIZE) /* all values                                                \
                   */

#    define STRNCPY_L size_t
#    define STRNML 32

#    if defined(_FILE_OFFSET_BITS) && _FILE_OFFSET_BITS == 64
/* size and offset internal storage
 * type */
#        define SZOFFTYPE unsigned long long
#        define SZOFFPSPEC                                                     \
            "ll" /* SZOFFTYPE print specification                              \
                  * modifier */
#    endif       /* defined(_FILE_OFFSET_BITS) && _FILE_OFFSET_BITS==64 */

#    define XDR_PMAPLIST (xdrproc_t) xdr_pmaplist
#    define XDR_VOID (xdrproc_t) xdr_void

#    include "lsof.h"

/*
 * Global storage definitions (including their structure definitions)
 */

struct mounts {
    char *dir;           /* directory name (mounted on) */
    char *fsname;        /* file system
                          * (symbolic links unresolved) */
    char *fsnmres;       /* file system
                          * (symbolic links resolved) */
    size_t dirl;         /* length of directory name */
    dev_t dev;           /* directory st_dev */
    dev_t rdev;          /* directory st_rdev */
    INODETYPE inode;     /* directory st_ino */
    mode_t mode;         /* directory st_mode */
    int ds;              /* directory status -- i.e., SB_*
                          * values */
    mode_t fs_mode;      /* file system st_mode */
    int ty;              /* node type -- e.g., N_REGLR, N_NFS */
    struct mounts *next; /* forward link */
};

struct sfile {
    char *aname;               /* argument file name */
    char *name;                /* file name (after readlink()) */
    char *devnm;               /* device name (optional) */
    dev_t dev;                 /* device */
    dev_t rdev;                /* raw device */
    mode_t mode;               /* S_IFMT mode bits from stat() */
    int type;                  /* file type: 0 = file system
                                *	      1 = regular file */
    INODETYPE i;               /* inode number */
    int f;                     /* file found flag */
    struct mounts *mp;         /* mount structure pointer for file
                                * system type entries */
#    define SAVE_MP_IN_SFILE 1 /* for ck_file_arg() im arg.c */
    struct sfile *next;        /* forward link */
};

#    if defined(HASEPTOPTS)
typedef struct pxinfo {  /* hashed pipe, UNIX socket or pseudo-
                          * terminal inode information */
    INODETYPE ino;       /* file's inode */
    struct lfile *lf;    /* connected peer file */
    int lpx;             /* connected process index */
    struct pxinfo *next; /* next entry for hashed inode */
} pxinfo_t;
#    endif /* defined(HASEPTOPTS) */

/* offset type:
 *     0 == unknown
 *     1 == lstat's st_size
 *     2 == from /proc/<PID>/fdinfo
 */
#    define OFFSET_UNKNOWN 0
#    define OFFSET_LSTAT 1
#    define OFFSET_FDINFO 2

typedef struct mntsup {
    char *dir_name;      /* mounted directory name */
    size_t dir_name_len; /* strlen(dir_name) */
    dev_t dev;           /* device number */
    int ln;              /* line on which defined */
    struct mntsup *next; /* next entry */
} mntsup_t;

/* local lock structure */
struct llock {
    int pid;
    dev_t dev;
    INODETYPE inode;
    enum lsof_lock_mode type;
    struct llock *next;
};

/* AX25 socket information */
struct ax25sin {
    char *da;     /* destination address */
    char *dev_ch; /* device characters */
    char *sa;     /* source address */
    INODETYPE inode;
    unsigned long sq, rq;   /* send and receive queue values */
    unsigned char sqs, rqs; /* send and receive queue states */
    int state;
    struct ax25sin *next;
};

struct icmpin {
    INODETYPE inode; /* node number */
    char *la;        /* local address */
    char *ra;        /* remote address */
    MALLOC_S lal;    /* strlen(la) */
    MALLOC_S ral;    /* strlen(ra) */
    struct icmpin *next;
};

/* IPX socket information */
struct ipxsin {
    INODETYPE inode;
    char *la; /* local address */
    char *ra; /* remote address */
    int state;
    unsigned long txq, rxq; /* transmit and receive queue values */
    struct ipxsin *next;
};

/* Netlink socket information */
struct nlksin {
    INODETYPE inode; /* node number */
    unsigned int pr; /* protocol */
    struct nlksin *next;
};

/* packet information */
struct packin {
    INODETYPE inode;
    int ty; /* socket type */
    int pr; /* protocol */
    struct packin *next;
};

/* raw socket information */
struct rawsin {
    INODETYPE inode;
    char *la;     /* local address */
    char *ra;     /* remote address */
    char *sp;     /* state characters */
    MALLOC_S lal; /* strlen(la) */
    MALLOC_S ral; /* strlen(ra) */
    MALLOC_S spl; /* strlen(sp) */
    struct rawsin *next;
};

/* SCTP socket information */
struct sctpsin {
    INODETYPE inode;
    int type;      /* type: 0 = assoc
                    *	 1 = eps
                    *	 2  assoc and eps */
    char *addr;    /* association or endpoint address */
    char *assocID; /* association ID */
    char *lport;   /* local port */
    char *rport;   /* remote port */
    char *laddrs;  /* local address */
    char *raddrs;  /* remote address */
    struct sctpsin *next;
};

/* IPv4 TCP and UDP socket
 * information */
struct tcp_udp {
    INODETYPE inode;
    unsigned long faddr, laddr; /* foreign & local IPv4 addresses */
    int fport, lport;           /* foreign & local ports */
    unsigned long txq, rxq;     /* transmit & receive queue values */
    int proto;                  /* 0 = TCP, 1 = UDP, 2 = UDPLITE */
    int state;                  /* protocol state */
    struct tcp_udp *next;       /* in TcpUdp inode hash table */
#    if defined(HASEPTOPTS)
    pxinfo_t *pxinfo;         /* inode information */
    struct tcp_udp *ipc_next; /* in TcpUdp local ipc hash table */
    struct tcp_udp *ipc_peer; /* locally connected peer(s) info */
#    endif                    /* defined(HASEPTOPTS) */
};

#    if defined(HASIPv6)
/* IPv6 TCP and UDP socket
 * information */
struct tcp_udp6 {
    INODETYPE inode;
    struct in6_addr faddr, laddr; /* foreign & local IPv6 addresses */
    int fport, lport;             /* foreign & local ports */
    unsigned long txq, rxq;       /* transmit & receive queue values */
    int proto;                    /* 0 = TCP, 1 = UDP, 2 = UDPLITE */
    int state;                    /* protocol state */
    struct tcp_udp6 *next;
#        if defined(HASEPTOPTS)
    pxinfo_t *pxinfo;          /* inode information */
    struct tcp_udp6 *ipc_next; /* in TcpUdp6 local ipc hash table */
    struct tcp_udp6 *ipc_peer; /* locally connected peer(s) info */
#        endif                 /* defined(HASEPTOPTS) */
};
#    endif /* defined(HASIPv6) */

/* UNIX socket information */
typedef struct uxsin {
    INODETYPE inode;      /* node number */
    char *pcb;            /* protocol control block */
    char *path;           /* file path */
    unsigned char sb_def; /* stat(2) buffer definitions */
    dev_t sb_dev;         /* stat(2) buffer device */
    INODETYPE sb_ino;     /* stat(2) buffer node number */
    dev_t sb_rdev;        /* stat(2) raw device number */
    uint32_t ty;          /* socket type */
    unsigned int opt;     /* socket options */
    unsigned int ss;      /* socket state */

#    if defined(HASEPTOPTS) && defined(HASUXSOCKEPT)
    struct uxsin *icons; /* incoming socket conections */
    unsigned int icstat; /* incoming connection status
                          * 0 == none */
    pxinfo_t *pxinfo;    /* inode information */
    struct uxsin *peer;  /* connected peer(s) info */
#    endif               /* defined(HASEPTOPTS) && defined(HASUXSOCKEPT) */

    struct uxsin *next;
} uxsin_t;

struct lsof_context_dialect {
    /* pipe endpoint hash buckets */
    pxinfo_t **pipe_endpoint_buckets;
    /* pseudoterminal endpoint hash buckets */
    pxinfo_t **pty_endpoint_buckets;
    /* posix msg queue endpoint hash buckets */
    pxinfo_t **posix_msg_queue_endpoint_buckets;
    /* eventfd endpoint hash buckets */
    pxinfo_t **eventfd_endpoint_buckets;

    /* NFS mount point status:
     *     1 == there is an NFS mount point,
     *          but its device number is
     *          unknown
     *     2 == there is an NFS mount point
     *          and its device number is
     *          known
     */
    int nfs_status;

    /* validity of the hashSfile() hash buckets */
    uint8_t sfile_valid;

    /* mount supplement
     * hash buckets */
    mntsup_t **mount_sup_hash;
    /* error reading mout supplement file */
    int mount_sup_error;

    /* buffer for get_fields() */
    char **get_fields_buffer;
    size_t get_fields_buffer_size;

    /* PID-hashed locks */
    struct llock **locks;

    /* conditional status of regular file
     * checking:
     *     0 = unconditionally check
     *     1 = conditionally check */
    uint8_t check_regular;

    /* socket file only checking status:
     *     0 = none
     *     1 = check only socket files */
    uint8_t check_only_socket;

    /* The number for the device behind
     * mqueue mount point */
    dev_t mqueue_dev;

    /* offset type:
     *     0 == unknown
     *     1 == lstat's st_size
     *     2 == from /proc/<PID>/fdinfo
     */
    int offset_type;

    /* AX25 socket info, hashed by inode */
    struct ax25sin **ax25_sin;
    int ax25_valid;

    /* ICMP socket info, hashed by inode */
    struct icmpin **icmp_sin;
    int icmp_valid;

    /* IPX socket info, hashed by inode */
    struct ipxsin **ipx_sin;
    int ipx_valid;

    /* Netlink socket info, hashed by
     * inode */
    struct nlksin **netlink_sin;
    int netlink_valid;

    /* packet info, hashed by inode */
    struct packin **packet_sin;
    int packet_valid;

    /* raw socket info, hashed by inode */
    struct rawsin **raw_sin;
    int raw_valid;

    /* SCTP info, hashed by inode */
    struct sctpsin **sctp_sin;
    int sctp_valid;

    /* IPv4 TCP & UDP info, hashed by
     * inode */
    struct tcp_udp **tcp_udp;
    /* dynamically sized hash bucket
     * count for TCP and UDP -- will
     * be a power of two */
    int tcp_udp_buckets;
    int tcp_valid;
    int udp_valid;
    int udplite_valid;
    /* IPv4 TCP & UDP info for socket used
       for IPC, hashed by (addr, port paris
       and protocol */
    struct tcp_udp **tcp_udp_ipc;

#    if defined(HASIPv6)
    /* IPv6 raw socket info, hashed by
     * inode */
    struct rawsin **raw6_sin;
    int raw6_valid;

    /* IPv6 TCP & UDP info, hashed by
     * inode */
    struct tcp_udp6 **tcp_udp6;
    /* dynamically sized hash bucket
     * count for IPv6 TCP and UDP -- will
     * be a power of two */
    int tcp_udp6_buckets;
    int tcp6_valid;
    int udp6_valid;
    int udplite6_valid;
    /* IPv6 TCP & UDP info for socket used
       for IPC, hashed by (addr, port paris
       and protocol */
    struct tcp_udp6 **tcp_udp6_ipc;
#    endif /* defined(HASIPv6) */

    /* UNIX socket info, hashed by inode */
    uxsin_t **unix_sin;
    int unix_valid;
};
/* Convenience macros */
/* mount supplement */
#    define MSHash (ctxd.mount_sup_hash)
/* endpoint buckets */
#    define Pinfo (ctxd.pipe_endpoint_buckets)
#    define PtyInfo (ctxd.pty_endpoint_buckets)
#    define PSXMQinfo (ctxd.posix_msg_queue_endpoint_buckets)
#    define EvtFDinfo (ctxd.eventfd_endpoint_buckets)
/* locks */
#    define LckH (ctxd.locks)
/* NFS status */
#    define HasNFS (ctxd.nfs_status)
/* mqueue dev */
#    define MqueueDev (ctxd.mqueue_dev)
/* offset type */
#    define OffType (ctxd.offset_type)
/* check regular file or socket only */
#    define Cckreg (ctxd.check_regular)
#    define Ckscko (ctxd.check_only_socket)
/* socket info */
#    define AX25sin (ctxd.ax25_sin)
#    define Icmpin (ctxd.icmp_sin)
#    define Ipxsin (ctxd.ipx_sin)
#    define Nlksin (ctxd.netlink_sin)
#    define Packin (ctxd.packet_sin)
#    define Rawsin (ctxd.raw_sin)
#    define SCTPsin (ctxd.sctp_sin)
#    define TcpUdp (ctxd.tcp_udp)
#    define TcpUdp_bucks (ctxd.tcp_udp_buckets)
#    define TcpUdpIPC (ctxd.tcp_udp_ipc)
#    define Rawsin6 (ctxd.raw6_sin)
#    define TcpUdp6 (ctxd.tcp_udp6)
#    define TcpUdp6_bucks (ctxd.tcp_udp6_buckets)
#    define TcpUdp6IPC (ctxd.tcp_udp6_ipc)
#    define Uxsin (ctxd.unix_sin)

#endif /* LINUX_LSOF_H	*/
