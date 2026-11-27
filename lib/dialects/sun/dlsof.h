/*
 * dlsof.h - Solaris header file for lsof
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

/*
 * $Id: dlsof.h,v 1.48 2012/04/10 16:40:23 abe Exp $
 */

#if !defined(SOLARIS_LSOF_H)
#    define SOLARIS_LSOF_H 1

#    include <fcntl.h>
#    include <sys/mntent.h>
#    include <sys/mnttab.h>
#    include <sys/param.h>
#    include <sys/stat.h>
#    include <sys/types.h>
#    include <sys/sysmacros.h>
#    include <stddef.h>
#    include <stdint.h>
#    include <stdlib.h>
#    include <dirent.h>
#    include <errno.h>
#    include <signal.h>
#    include <setjmp.h>
#    include <string.h>
#    include <unistd.h>
#    include <arpa/inet.h>
#    include <netdb.h>
#    include <pwd.h>
#    include <grp.h>
#    include <time.h>
#    include <sys/socket.h>
#    include <sys/un.h>
#    include <sys/wait.h>
#    include <netinet/in.h>
#    include <rpc/rpc.h>
#    include <rpc/clnt.h>
#    include <rpc/pmap_prot.h>
#    include <rpc/rpcent.h>

/*
 * The procfs backend has no kernel structures.  Define the small set of
 * types and node/vnode constants that common lsof code still references.
 */
#    define LSOF_NO_KA_T 1

typedef uint32_t major_t;
typedef uint32_t minor_t;

#    if !defined(MAXPATHLEN)
#        define MAXPATHLEN 1024
#    endif
#    if !defined(MAXCOMLEN)
#        define MAXCOMLEN 16
#    endif

#    define GET_MAJ_DEV(d) ((major_t)((d) >> L_BITSMINOR & L_MAXMAJ))
#    define GET_MIN_DEV(d) ((minor_t)((d) & L_MAXMIN))

/* TCP/TPI state values from the Solaris networking headers. */
#    define TCPS_CLOSED 0
#    define TCPS_IDLE 1
#    define TCPS_BOUND 2
#    define TCPS_LISTEN 3
#    define TCPS_SYN_SENT 4
#    define TCPS_SYN_RCVD 5
#    define TCPS_ESTABLISHED 6
#    define TCPS_CLOSE_WAIT 7
#    define TCPS_FIN_WAIT_1 8
#    define TCPS_CLOSING 9
#    define TCPS_LAST_ACK 10
#    define TCPS_FIN_WAIT_2 11
#    define TCPS_TIME_WAIT 12

#    define TS_UNBND 0
#    define TS_WACK_BREQ 1
#    define TS_WACK_UREQ 2
#    define TS_IDLE 3
#    define TS_WACK_OPTREQ 4
#    define TS_WACK_CREQ 5
#    define TS_WCON_CREQ 6
#    define TS_WRES_CIND 7
#    define TS_WACK_CRES 8
#    define TS_DATA_XFER 9
#    define TS_WIND_ORDREL 10
#    define TS_WREQ_ORDREL 11
#    define TS_WACK_DREQ6 12
#    define TS_WACK_DREQ7 13
#    define TS_WACK_DREQ9 14
#    define TS_WACK_DREQ10 15
#    define TS_WACK_DREQ11 16

enum vtype {
    VNON,
    VREG,
    VDIR,
    VBLK,
    VCHR,
    VLNK,
    VSOCK,
    VFIFO,
    VBAD,
    VDOOR,
    VPORT,
    VMPC,
    VV_ROOT
};

enum node_type {
    N_UNKN,
    N_REGLR,
    N_BLK,
    N_CHR,
    N_FIFO,
    N_SOCK,
    N_STREAM,
    N_COM,
    N_MNT,
    N_FD,
    N_PROC,
    N_SPEC,
    N_PIPE,
    N_DOOR,
    N_PORT,
    N_DEV,
    N_NM,
    N_OBJF,
    N_PSEU,
    N_SHARED,
    N_AFPFS,
    N_AFS,
    N_ANON_INODE,
    N_AUSX,
    N_AUTO,
    N_CACHE,
    N_CDFS,
    N_CFS,
    N_CTFSADIR,
    N_CTFSBUND,
    N_CTFSCDIR,
    N_CTFSCTL,
    N_CTFSEVT,
    N_CTFSLATE,
    N_CTFSROOT,
    N_CTFSSTAT,
    N_CTFSSYM,
    N_CTFSTDIR,
    N_CTFSTMPL,
    N_HSFS,
    N_KERN,
    N_LOFS,
    N_MPC,
    N_MQUEUE,
    N_MVFS,
    N_NFS,
    N_NFS4,
    N_PCFS,
    N_SAMFS,
    N_SANFS,
    N_SDEV,
    N_TMP,
    N_UFS,
    N_UNIX,
    N_VXFS,
    N_XFS,
    N_ZFS
};

/*
 * Structure for Atria's MVFS nodes
 */

struct mvfsnode {
    unsigned long d1[6];
    unsigned long m_ino; /* node number */
};

#    define COMP_P const void
#    define CWDLEN (MAXPATHLEN + 1)
#    define DEVINCR 1024 /* device table malloc() increment */
#    define DINAMEL 32
#    define DIRTYPE dirent

#    if solaris >= 20501
#        define KMEM "/dev/mem"
#    else /* solaris<20501 */
#        define KMEM "/dev/kmem"
#    endif /* solaris>=20501 */

#    define MALLOC_P char
#    define FREE_P MALLOC_P
#    define MALLOC_S unsigned

#    if !defined(MAXEND)
#        define MAXEND 0x7fffffff
#    endif /* !defined(MAXEND) */

#    define MAXSEGS 100          /* maximum text segments */
#    define MAXSYSCMDL MAXCOMLEN /* max system command name length */
#    define NETCLNML 8
#    define PROCMIN 5 /* processes that make a "good" scan */

#    if defined(HASPROCFS)
#        define PR_ROOTINO 2 /* root inode for proc file system */
#    endif                   /* defined(HASPROCFS) */

#    define PROCDFLT                                                           \
        256 /* default size for local proc table --                            \
             * MUST BE > 4!!!  */
#    define PROCTRYLM 5 /* times to try to read proc table */
#    define QSORT_P char
#    define STRNCPY_L int
#    define STRNML 32 /* stream name length (maximum) */

#    if solaris >= 20501
/*
 * Enable large file support.
 */

#        if solaris >= 20600
#            define fstat fstat64
#            define lstat lstat64
#            define stat stat64
#        endif /* solaris>=20600 */

#        define SZOFFTYPE unsigned long long
/* size and offset internal storage
 * type */
#        define SZOFFPSPEC                                                     \
            "ll" /* SZOFFTYPE printf specification                             \
                  * modifier */
#    endif       /* solaris>=20501 */


/*
 * Global storage definitions (including their structure definitions)
 */


struct clone {
    struct l_dev cd;    /* device, inode, name, and verify */
    int n;              /* network flag */
    struct clone *next; /* forward link */
};
extern struct clone *Clone;

extern major_t CloneMaj;


extern char **Fsinfo;
extern int Fsinfomax;
extern int HaveCloneMaj;

struct l_ino {
    unsigned char dev_def;  /* dev member is defined */
    unsigned char ino_def;  /* ino member is defined */
    unsigned char nl_def;   /* nl member is defined */
    unsigned char rdev_def; /* rdev member is defined */
    unsigned char sz_def;   /* sz member is defined */
    dev_t dev;              /* device */
    long ino;               /* node number */
    long nl;                /* link count */
    dev_t rdev;             /* "raw" device */
    SZOFFTYPE sz;           /* size */
};

struct l_vfs {
    char *dir;    /* mounted directory */
    char *fsname; /* file system name */
    dev_t dev;    /* device */

#    if defined(HASFSINO)
    INODETYPE fs_ino; /* file system inode number */
#    endif            /* defined(HASFSINO) */

#    if defined(HASMNTSTAT)
    unsigned char mnt_stat; /* mount point stat() status:
                             *     0 = succeeded
                             *     1 = failed */
#    endif                  /* defined(HASMNTSTAT) */

#    if solaris >= 80000
    nlink_t nlink; /* directory link count */
    off_t size;    /* directory size */
#    endif         /* solaris>=80000 */

    struct l_vfs *next; /* forward link */
};
extern struct l_vfs *Lvfs;

struct mounts {
    char *dir;           /* directory (mounted on) */
    char *fsname;        /* file system
                          * (symbolic links unresolved) */
    char *fsnmres;       /* file system
                          * (symbolic links resolved) */
    dev_t dev;           /* directory st_dev */
    dev_t rdev;          /* directory st_rdev */
    INODETYPE inode;     /* directory st_ino */
    mode_t mode;         /* directory st_mode */
    mode_t fs_mode;      /* file system st_mode */
    struct mounts *next; /* forward link */

#    if defined(HASFSTYPE)
    char *fstype; /* file system type */
#    endif        /* defined(HASFSTYPE) */

#    if solaris >= 80000
    nlink_t nlink; /* directory st_nlink */
    off_t size;    /* directory st_size */
#    endif         /* solaris>=80000 */

#    if defined(HASMNTSTAT)
    unsigned char stat; /* mount point stat() status:
                         *     0 = succeeded
                         *     1 = failed */
#    endif              /* defined(HASMNTSTAT) */
};

struct pseudo {
    struct l_dev pd;     /* device, inode, path, verify */
    struct pseudo *next; /* forward link */
};
extern struct pseudo *Pseudo;

/*
 * Solaris 11 sdev definitions
 */

#    define SDVOP_IP 0  /* Sdev[] devipnet_vnodeops index */
#    define SDVOP_NET 1 /* Sdev[] devnet_vnodeops index */
#    define SDVOP_PTS 2 /* Sdev[] devpts_vnodeops index */
#    define SDVOP_VT 3  /* Sdev[] devvt_vnodeops index */
#    define SDVOP_NUM 4 /* number of Sdev[] entries */

struct sfile {
    char *aname;        /* file name argument */
    char *name;         /* file name (after readlink()) */
    char *devnm;        /* device name (optional) */
    dev_t dev;          /* device */
    dev_t rdev;         /* raw device */
    u_short mode;       /* S_IFMT mode bits from stat() */
    int type;           /* file type: 0 = file system
                         *	      1 = regular file */
    INODETYPE i;        /* inode number */
    int f;              /* file found flag */
    struct sfile *next; /* forward link */
};
extern int Unof; /* u_nofiles value */

/*
 * VxFS definitions
 */

#    define VXVOP_FCL 0   /* Vvops[] vx_fcl_vnodeops_p index */
#    define VXVOP_FDD 1   /* Vvops[] fdd_vnops index */
#    define VXVOP_FDDCH 2 /* Vvops[] fdd_chain_vnops index */
#    define VXVOP_REG 3   /* Vvops[] vx_vnodeops index */
#    define VXVOP_REG_P 4 /* Vvops[] vx_vnodeops_p index */
#    define VXVOP_NUM 5   /* number of Vvops[] entries */

/*
 * Kernel name list definitions
 */

#    define NL_NAME n_name
#    define X_NCACHE "ncache"
#    define X_NCSIZE "ncsize"

/*
 * Definitions for dvch.c
 */

#    if defined(HASDCACHE)
#        define DCACHE_CLONE                                                   \
            rw_clone_sect /* clone function for read_dcache                    \
                           */
#        define DCACHE_CLR                                                     \
            clr_sect /* function to clear clone and                            \
                      * pseudo caches when reading the                         \
                      * device cache file fails */
#        define DCACHE_PSEUDO                                                  \
            rw_pseudo_sect /* pseudo function for read_dcache */
#    endif                 /* defined(HASDCACHE) */

#    define DVCH_DEVPATH "/devices"

/*
 * Definition for cvfs.c
 */

#    define CVFS_DEVSAVE 1

#    if solaris >= 80000
#        define CVFS_NLKSAVE 1
#        define CVFS_SZSAVE 1
#    endif /* solaris>=80000 */

/*
 * Definitions for rnch.c
 */


struct lsof_context_dialect {};

#endif /* SOLARIS_LSOF_H */
