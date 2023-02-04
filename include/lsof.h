/** @file
 * lsof.h - header file for lsof
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
 * $Id: common.h,v 1.70 2018/03/26 21:50:45 abe Exp $
 */

#if !defined(LSOF_H)
#    define LSOF_H 1

#    include <stddef.h>
#    include <stdint.h>
#    include <stdio.h>

/** \mainpage
 * liblsof provides a cross platform mechanism to list open files. To use
 * liblsof, you should:
 *
 * 1. Check if compile-time and run-time API versions match.
 * 2. Create a context via `lsof_new()`.
 * 3. Set options of the context via `lsof_select_process()` etc.
 * 4. freeze context via `lsof_freeze()`.
 * 5. List open files via `lsof_gather()`, you can call `lsof_gather()` multiple
 * times. Remember to free the result via `lsof_free_result()`.
 * 6. Destroy the context via `lsof_destroy()`.
 */

/** lsof error returns */
enum lsof_error {
    LSOF_SUCCESS = 0,            /**< Success */
    LSOF_ERROR_INVALID_ARGUMENT, /**< Invalid Argument */
    LSOF_ERROR_NO_MEMORY,        /**< No memory */
};

/** File access mode */
enum lsof_file_access_mode {
    LSOF_FILE_ACCESS_NONE = 0,  /**< None */
    LSOF_FILE_ACCESS_READ = 1,  /**< Read */
    LSOF_FILE_ACCESS_WRITE = 2, /**< Write */
    LSOF_FILE_ACCESS_READ_WRITE =
        LSOF_FILE_ACCESS_READ | LSOF_FILE_ACCESS_WRITE, /**< Read and write */
};

/** File lock mode */
enum lsof_lock_mode {
    LSOF_LOCK_NONE,          /**< None */
    LSOF_LOCK_UNKNOWN,       /**< Unknown */
    LSOF_LOCK_READ_PARTIAL,  /**< Read lock on part of the file */
    LSOF_LOCK_READ_FULL,     /**< Read lock on the entire file */
    LSOF_LOCK_WRITE_PARTIAL, /**< Write lock on part of the file */
    LSOF_LOCK_WRITE_FULL,    /**< Write lock on the entire file */
    LSOF_LOCK_READ_WRITE,    /**< Read and write lock */
    LSOF_LOCK_SOLARIS_NFS,   /**< Solaris NFS lock */
    LSOF_LOCK_SCO_PARTIAL, /**< SCO OpenServer Xenix lock on part of the file */
    LSOF_LOCK_SCO_FULL,    /**< SCO OpenServer Xenix lock on the entire file */
};

/** File descriptor type */
enum lsof_fd_type {
    LSOF_FD_NUMERIC,    /**< Numeric fd, e.g fd returned by open() in process */
    LSOF_FD_UNKNOWN,    /**< Unknown fd type */
    LSOF_FD_CWD,        /**< Current working directory */
    LSOF_FD_ERROR,      /**< Failed to get fd information */
    LSOF_FD_ROOT_DIR,   /**< Root directory */
    LSOF_FD_PARENT_DIR, /**< Parent directory */
    LSOF_FD_PROGRAM_TEXT, /**< Program text */
    LSOF_FD_MEMORY,       /**< Memory */
    LSOF_FD_DELETED,      /**< Deleted file */
    LSOF_FD_FILEPORT,     /**< Darwin fileport */
    LSOF_FD_TASK_CWD,     /**< Per task/thread cwd */
    LSOF_FD_CTTY,         /**< Character TTY */
    LSOF_FD_JAIL_DIR,     /**< Jail directory */
};

/** File type */
enum lsof_file_type {
    /* struct stat S_IFMT modes */
    LSOF_FILE_FIFO,    /**< FIFO special file */
    LSOF_FILE_CHAR,    /**< Character special file */
    LSOF_FILE_DIR,     /**< Directory */
    LSOF_FILE_BLOCK,   /**< Block special file */
    LSOF_FILE_REGULAR, /**< Regular file */
    LSOF_FILE_LINK,    /**< Symolic link */
    LSOF_FILE_SOCKET,  /**< Socket of unknown domain */

    /* Network */
    LSOF_FILE_IPV4,              /**< IPv4 socket */
    LSOF_FILE_IPV6,              /**< IPv6 socket */
    LSOF_FILE_AX25,              /**< AX.25 socket */
    LSOF_FILE_INET,              /**< Internet(either IPv4 or IPv6) socket */
    LSOF_FILE_LINK_LEVEL_ACCESS, /**< HP-UX link level access file */
    LSOF_FILE_ROUTE,             /**< AF_ROUTE socket */
    LSOF_FILE_UNIX,              /**< UNIX domain socket */
    LSOF_FILE_X25,               /**< HP-UX x.25 socket */
    LSOF_FILE_APPLETALK,         /**< Appletalk socket */
    LSOF_FILE_NET_DRIVER,        /**< AF_NDRV network driver raw socket */
    LSOF_FILE_INTERNAL_KEY,      /**< Darwin internal key-management socket */
    LSOF_FILE_SYSTEM,            /**< AF_SYSTEM kernel event messages socket */
    LSOF_FILE_PPP,               /**< PPP socket */
    LSOF_FILE_IPX,               /**< IPX socket */
    LSOF_FILE_RAW,               /**< raw socket */
    LSOF_FILE_RAW6,              /**< raw IPv6 socket */
    LSOF_FILE_NETLINK,           /**< netlink socket */
    LSOF_FILE_PACKET,            /**< packet socket */
    LSOF_FILE_ICMP,              /**< icmp socket */

    /* procfs */
    LSOF_FILE_PROC_AS,       /**< Solaris /proc/<PID>/as file */
    LSOF_FILE_PROC_AUXV,     /**< /proc/<PID>/auxv file */
    LSOF_FILE_PROC_CRED,     /**< Solaris /proc/<PID>/cred file */
    LSOF_FILE_PROC_CTRL,     /**< /proc/<PID>/ctl control file */
    LSOF_FILE_PROC_CUR_PROC, /**< NetBSD /proc/curproc file */
    LSOF_FILE_PROC_CWD, /**< Solaris /proc/<PID>/cwd current working directory
                         */
    LSOF_FILE_PROC_DIR, /**< /proc directory */
    LSOF_FILE_PROC_PID, /**< /proc/<PID> directory */
    LSOF_FILE_PROC_EXEC_TYPE, /**< FreeBSD /proc executable type (etype) */
    LSOF_FILE_PROC_FD,        /**< /proc/<PID>/fd/<FD> file */
    LSOF_FILE_PROC_FD_DIR,    /**< /proc/<PID>/fd directory */
    LSOF_FILE_PROC_FILE,      /**< /proc/<PID>/file executable file */
    LSOF_FILE_PROC_FP_REGS, /**< /proc/<PID>/fpregs floating point register set
                             */
    LSOF_FILE_PROC_PAGE_DATA,      /**< Solaris /proc/<PID>/pagedata file */
    LSOF_FILE_PROC_GROUP_NOTIFIER, /**< /proc/<PID>/notepg group notifier file
                                    */
    LSOF_FILE_PROC_LDT,            /**< Solaris /proc/<PID>/ldt file */
    LSOF_FILE_PROC_LPS_INFO,       /**< Solaris /proc/<PID>/lpsinfo file */
    LSOF_FILE_PROC_LSTATUS,        /**< Solaris /proc/<PID>/lstatus file */
    LSOF_FILE_PROC_LUSAGE,         /**< Solaris /proc/<PID>/lusage file */
    LSOF_FILE_PROC_LWP_GWINDOWS,   /**< Solaris /proc/<PID>/lwp/<LWPID>/gwindows
                                    * file
                                    */
    LSOF_FILE_PROC_LWP_CTL, /**< Solaris /proc/<PID>/lwp/<LWPID>/lwpctl file */
    LSOF_FILE_PROC_LWP_DIR, /**< Solaris /proc/<PID>/lwp or
                               /proc/<PID>/lwp/<LWPID> directory */
    LSOF_FILE_PROC_LWP_SINFO, /**< Solaris /proc/<PID>/lwp/<LWPID>/lwpsinfo file
                               */
    LSOF_FILE_PROC_LWP_STATUS, /**< Solaris /proc/<PID>/lwp/<LWPID>/lwpstatus
                                  file */
    LSOF_FILE_PROC_LWP_USAGE, /**< Solaris /proc/<PID>/lwp/<LWPID>/lwpusage file
                               */
    LSOF_FILE_PROC_LWP_XREGS, /**< Solaris /proc/<PID>/lwp/<LWPID>/xregs file */
    LSOF_FILE_PROC_MAP,       /**< /proc/<PID>/map memory mapping file */
    LSOF_FILE_PROC_MAPS,      /**< /proc/<PID>/maps memory mapping file */
    LSOF_FILE_PROC_MEMORY,    /**< /proc/<PID>/mem memory image file */
    LSOF_FILE_PROC_PROC_NOTIFIER, /**< /proc/<PID>/note process notifier file */
    LSOF_FILE_PROC_OBJ,           /**< Solaris /proc/<PID>/object file */
    LSOF_FILE_PROC_OBJ_DIR,       /**< Solaris /proc/<PID>/object directory */
    LSOF_FILE_PROC_OLD_LWP,       /**< Solaris old format /proc/<LWPID> file */
    LSOF_FILE_PROC_OLD_PID,       /**< Solaris old format /proc/<PID> file */
    LSOF_FILE_PROC_OLD_PAGE, /**< Solaris old format /proc/<PID> page data file
                              */
    LSOF_FILE_PROC_REGS,     /**< /proc/<PID>/regs register set */
    LSOF_FILE_PROC_RMAP,     /**< Solaris /proc/<PID>/rmap file */
    LSOF_FILE_PROC_ROOT,     /**< Solaris /proc/<PID>/root root directory */
    LSOF_FILE_PROC_SIGACT,   /**< Solaris /proc/<PID>/sigact file */
    LSOF_FILE_PROC_PSINFO,   /**< Solaris /proc/<PID>/psinfo file */
    LSOF_FILE_PROC_STATUS,   /**< /proc/<PID>/status status file */
    LSOF_FILE_PROC_USAGE,    /**< Solaris /proc/<PID>/usage file */
    LSOF_FILE_PROC_WATCH,    /**< Solaris /proc/<PID>/watch file */
    LSOF_FILE_PROC_XMAP,     /**< Solaris /proc/<PID>/xmap file */

    /* Others */
    LSOF_FILE_ANON_INODE,        /**< anonymous inode */
    LSOF_FILE_DEL,               /**< Linux map file that has been deleted */
    LSOF_FILE_DOOR,              /**< Solaris VDOOR file */
    LSOF_FILE_KQUEUE,            /**< BSD style kernel event file */
    LSOF_FILE_FSEVENTS,          /**< fsevents file */
    LSOF_FILE_EVENTFD,           /**< eventfd file */
    LSOF_FILE_PROCDESC,          /**< process descriptor file */
    LSOF_FILE_MULTIPLEXED_BLOCK, /**< SCO OpenServer multiplexed block file */
    LSOF_FILE_MULTIPLEXED_CHAR,  /**< SCO OpenServer multiplexed character file
                                  */
    LSOF_FILE_UNKNOWN_DELETED,   /**< Linux unknown deleted file */
    LSOF_FILE_UNKNOWN_MEMORY,    /**< Linux unknown memory file */
    LSOF_FILE_UNKNOWN_FD,        /**< Linux unknown fd */
    LSOF_FILE_UNKNOWN_CWD,       /**< Linux unknown cwd */
    LSOF_FILE_UNKNOWN_ROOT_DIR,  /**< Linux unknown root dir */
    LSOF_FILE_UNKNOWN_PROGRAM_TEXT, /**< Linux unknown program text */
    LSOF_FILE_PIPE,                 /**< pipes */
    LSOF_FILE_PORT,                 /**< Solarise SYSV named pipe */
    LSOF_FILE_POSIX_MQ,             /**< Linux POSIX message queue file */
    LSOF_FILE_POSIX_SEMA,           /**< Darwin POSIX semaphore file */
    LSOF_FILE_POSIX_SHM,            /**< Darwin POSIX shared memory file */
    LSOF_FILE_SHM,                  /**< shared memory file */
    LSOF_FILE_PTS,                  /**< FreeBSD /dev/pts file */
    LSOF_FILE_SHARED_MEM_TRANSPORT, /**< AIX Shared memory transport file */
    LSOF_FILE_STREAM,               /**< HP-UX streams */
    LSOF_FILE_STREAM_SOCKET,        /**< HP-UX stream socket */
    LSOF_FILE_SCO_UNKNOWN, /**< SCO OpenServer Xenix special file of unknown
                              type */
    LSOF_FILE_SCO_SEMA,    /**< SCO OpenServer Xenix semaphore file */
    LSOF_FILE_SCO_SHARED,  /**< SCO OpenServer Xenix shared data file */
    LSOF_FILE_UNSUPPORTED, /**< unsupported file type */

    /* types from struct vnode */
    LSOF_FILE_VNODE_VNON,  /**< The vnode has no type */
    LSOF_FILE_VNODE_VREG,  /**< The vnode represents a regular file */
    LSOF_FILE_VNODE_VDIR,  /**< The vnode represents a directory */
    LSOF_FILE_VNODE_VBLK,  /**< The vnode represents a block special device */
    LSOF_FILE_VNODE_VCHR,  /**< The vnode represents a character special device
                            */
    LSOF_FILE_VNODE_VLNK,  /**< The vnode represents a symbolic link */
    LSOF_FILE_VNODE_VSOCK, /**< The vnode represents a socket */
    LSOF_FILE_VNODE_VFIFO, /**< The vnode represents a pipe */
    LSOF_FILE_VNODE_VBAD,  /**< The vnode represents a bad file */
    LSOF_FILE_VNODE_VMPC,  /**< The vnode represents a multiplexed character
                              special device */
    LSOF_FILE_VNODE_VUNNAMED, /**< The vnode represents an unnamed file */

    /* Unknown, see unknown_file_type_number for raw value */
    LSOF_FILE_UNKNOWN, /**< Unknown file type, only raw numbers provided */
};

/** Network protocol */
enum lsof_protocol {
    LSOF_PROTOCOL_INVALID, /**< No network protocol */
    LSOF_PROTOCOL_UNKNOWN, /**< Unknown network protocol, only raw numbers
                              provided */
    LSOF_PROTOCOL_IP,      /**< IP */
    /* Follow ip protocol numbers order */
    LSOF_PROTOCOL_HOPOPTS,      /**< IPv6 hop-by-hop(0)  */
    LSOF_PROTOCOL_ICMP,         /**< ICMP(1) */
    LSOF_PROTOCOL_IGMP,         /**< IGMP(2) */
    LSOF_PROTOCOL_GGP,          /**< GGP(3) */
    LSOF_PROTOCOL_IPIP,         /**< IPIP(4) */
    LSOF_PROTOCOL_IPV4,         /**< IPV4(4) */
    LSOF_PROTOCOL_TCP,          /**< TCP(6) */
    LSOF_PROTOCOL_ST,           /**< ST(7) */
    LSOF_PROTOCOL_EGP,          /**< EGP(8) */
    LSOF_PROTOCOL_PIGP,         /**< PIGP(9) */
    LSOF_PROTOCOL_RCCMON,       /**< RCC Monitor(10) */
    LSOF_PROTOCOL_NVPII,        /**< NPV-II(11) */
    LSOF_PROTOCOL_PUP,          /**< PUP(12) */
    LSOF_PROTOCOL_ARGUS,        /**< ARGUS(13) */
    LSOF_PROTOCOL_EMCON,        /**< EMCON(14) */
    LSOF_PROTOCOL_XNET,         /**< XNET(15) */
    LSOF_PROTOCOL_CHAOS,        /**< CHAOS(16) */
    LSOF_PROTOCOL_UDP,          /**< UDP(17) */
    LSOF_PROTOCOL_MUX,          /**< MUX(18) */
    LSOF_PROTOCOL_MEAS,         /**< MEAS(19) */
    LSOF_PROTOCOL_HMP,          /**< HMP(20) */
    LSOF_PROTOCOL_PRM,          /**< PRM(21) */
    LSOF_PROTOCOL_IDP,          /**< IDP(22) */
    LSOF_PROTOCOL_TRUNK1,       /**< TRUNK1(23) */
    LSOF_PROTOCOL_TRUNK2,       /**< TRUNK2(24) */
    LSOF_PROTOCOL_LEAF1,        /**< LEAF1(25) */
    LSOF_PROTOCOL_LEAF2,        /**< LEAF2(26) */
    LSOF_PROTOCOL_RDP,          /**< RDP(27) */
    LSOF_PROTOCOL_IRTP,         /**< IRTP(28) */
    LSOF_PROTOCOL_TP,           /**< TP(29) */
    LSOF_PROTOCOL_BLT,          /**< BLT(30) */
    LSOF_PROTOCOL_NSP,          /**< NSP(31) */
    LSOF_PROTOCOL_INP,          /**< INP(32) */
    LSOF_PROTOCOL_DCCP,         /**< DCCP(33) */
    LSOF_PROTOCOL_SEP,          /**< SEP(33) */
    LSOF_PROTOCOL_3PC,          /**< 3PC(34) */
    LSOF_PROTOCOL_IDPR,         /**< IDPR(35) */
    LSOF_PROTOCOL_XTP,          /**< XTP(36) */
    LSOF_PROTOCOL_DDP,          /**< DDP(37) */
    LSOF_PROTOCOL_CMTP,         /**< CMTP(38) */
    LSOF_PROTOCOL_TPXX,         /**< TPXX(39) */
    LSOF_PROTOCOL_IL,           /**< IL(40) */
    LSOF_PROTOCOL_IPV6,         /**< IPv6(41) */
    LSOF_PROTOCOL_SDRP,         /**< SDRP(42) */
    LSOF_PROTOCOL_ROUTING,      /**< IPv6-Route(43) */
    LSOF_PROTOCOL_FRAGMENT,     /**< IPv6-Frag(44) */
    LSOF_PROTOCOL_IDRP,         /**< IDRP(45) */
    LSOF_PROTOCOL_RSVP,         /**< RSVP(46) */
    LSOF_PROTOCOL_GRE,          /**< GRE(47) */
    LSOF_PROTOCOL_MHRP,         /**< MHRP(48) */
    LSOF_PROTOCOL_BHA,          /**< BHA(49) */
    LSOF_PROTOCOL_ESP,          /**< ESP(50) */
    LSOF_PROTOCOL_AH,           /**< AH(51) */
    LSOF_PROTOCOL_INLSP,        /**< INLSP(52) */
    LSOF_PROTOCOL_SWIPE,        /**< SWIPE(53) */
    LSOF_PROTOCOL_NHRP,         /**< NHRP(54) */
    LSOF_PROTOCOL_MOBILE,       /**< MOBILE(55) */
    LSOF_PROTOCOL_TLSP,         /**< TLSP(56) */
    LSOF_PROTOCOL_SKIP,         /**< SKIP(57) */
    LSOF_PROTOCOL_ICMPV6,       /**< ICMPv6(58) */
    LSOF_PROTOCOL_NONE,         /**< IPv6-NoNxt(59) */
    LSOF_PROTOCOL_DSTOPTS,      /**< IPv6-Opts(60) */
    LSOF_PROTOCOL_AHIP,         /**< AHIP(61) */
    LSOF_PROTOCOL_CFTP,         /**< CFTP(62) */
    LSOF_PROTOCOL_HELLO,        /**< HELLO(63) */
    LSOF_PROTOCOL_SATEXPAK,     /**< SATEXPAK(64) */
    LSOF_PROTOCOL_KRYPTOLAN,    /**< KRYPTOLAN(65) */
    LSOF_PROTOCOL_RVD,          /**< RVD(66) */
    LSOF_PROTOCOL_IPPC,         /**< IPPC(67) */
    LSOF_PROTOCOL_ADFS,         /**< ADFS(68) */
    LSOF_PROTOCOL_SATMON,       /**< SATMON(69) */
    LSOF_PROTOCOL_VISA,         /**< VISA(70) */
    LSOF_PROTOCOL_IPCV,         /**< IPCV(71) */
    LSOF_PROTOCOL_CPNX,         /**< CPNX(72) */
    LSOF_PROTOCOL_CPHB,         /**< CPHB(73) */
    LSOF_PROTOCOL_WSN,          /**< WSN(74) */
    LSOF_PROTOCOL_PVP,          /**< PVP(75) */
    LSOF_PROTOCOL_BRSATMON,     /**< BRSATMON(76) */
    LSOF_PROTOCOL_ND,           /**< ND(77) */
    LSOF_PROTOCOL_WBMON,        /**< WBMON(78) */
    LSOF_PROTOCOL_WBEXPAK,      /**< WBEXPAK(79) */
    LSOF_PROTOCOL_EON,          /**< EON(80) */
    LSOF_PROTOCOL_VMTP,         /**< VMTP(81) */
    LSOF_PROTOCOL_SVMTP,        /**< SVMTP(82) */
    LSOF_PROTOCOL_VINES,        /**< VINES(83) */
    LSOF_PROTOCOL_TTP,          /**< TTP(84) */
    LSOF_PROTOCOL_IGP,          /**< IGP(85) */
    LSOF_PROTOCOL_DGP,          /**< DGP(86) */
    LSOF_PROTOCOL_TCF,          /**< TCF(87) */
    LSOF_PROTOCOL_IGRP,         /**< IGRP(88) */
    LSOF_PROTOCOL_OSPFIGP,      /**< OSPFIGP(89) */
    LSOF_PROTOCOL_SRPC,         /**< SRPC(90) */
    LSOF_PROTOCOL_LARP,         /**< LARP(91) */
    LSOF_PROTOCOL_MTP,          /**< MTP(92) */
    LSOF_PROTOCOL_AX25,         /**< AX.25(93) */
    LSOF_PROTOCOL_BEETPH,       /**< BEETPH(94) */
    LSOF_PROTOCOL_IPEIP,        /**< IPEIP(94) */
    LSOF_PROTOCOL_MICP,         /**< MICP(95) */
    LSOF_PROTOCOL_SCCSP,        /**< SCCSP(96) */
    LSOF_PROTOCOL_ETHERIP,      /**< ETHERIP(97) */
    LSOF_PROTOCOL_ENCAP,        /**< ENCAP(98) */
    LSOF_PROTOCOL_APES,         /**< APES(99) */
    LSOF_PROTOCOL_GMTP,         /**< GMTP(100) */
    LSOF_PROTOCOL_PIM,          /**< PIM(103) */
    LSOF_PROTOCOL_COMP,         /**< COMP(108) */
    LSOF_PROTOCOL_IPCOMP,       /**< IPCOMP(108) */
    LSOF_PROTOCOL_CARP,         /**< CARP(112) */
    LSOF_PROTOCOL_PGM,          /**< PGM(113) */
    LSOF_PROTOCOL_SCTP,         /**< SCTP(132) */
    LSOF_PROTOCOL_MH,           /**< Mobility Header(135) */
    LSOF_PROTOCOL_UDPLITE,      /**< UDPLITE(136) */
    LSOF_PROTOCOL_MPLS,         /**< MPLS(137) */
    LSOF_PROTOCOL_HIP,          /**< HIP(139) */
    LSOF_PROTOCOL_SHIM6,        /**< SHIM6(140) */
    LSOF_PROTOCOL_ETHERNET,     /**< Ethernet(143) */
    LSOF_PROTOCOL_PFSYNC,       /**< PFSYNC(240) */
    LSOF_PROTOCOL_RESERVED_253, /**< RESERVED_253(253) */
    LSOF_PROTOCOL_RESERVED_254, /**< RESERVED_254(254) */
    LSOF_PROTOCOL_OLD_DIVERT,   /**< OLD_DIVERT(254) */
    LSOF_PROTOCOL_RAW,          /**< Raw(255) */
    LSOF_PROTOCOL_MAX,          /**< MAX(256) */
    LSOF_PROTOCOL_DONE,         /**< DONE(257) */
    LSOF_PROTOCOL_SEND,         /**< SEND(259) */
    LSOF_PROTOCOL_MPTCP,        /**< MPTCP(262) */
    LSOF_PROTOCOL_SPACER,       /**< SPACER(32767) */
    /* Follow ethernet type order */
    LSOF_PROTOCOL_802_3,      /**< 802.3(0x0001) */
    LSOF_PROTOCOL_ALL,        /**< All(0x0003) */
    LSOF_PROTOCOL_802_2,      /**< 802.2(0x0004) */
    LSOF_PROTOCOL_SNAP,       /**< SNAP(0x0005) */
    LSOF_PROTOCOL_DDCMP,      /**< DDCMP(0x0006) */
    LSOF_PROTOCOL_WAN_PPP,    /**< WAN PPP(0x0007) */
    LSOF_PROTOCOL_PPP_MP,     /**< PPP MP(0x0008) */
    LSOF_PROTOCOL_LOCALTALK,  /**< LOCALTALK(0x0009) */
    LSOF_PROTOCOL_CAN,        /**< CAN(0x000C) */
    LSOF_PROTOCOL_CANFD,      /**< CANFD(0x000D) */
    LSOF_PROTOCOL_CANXL,      /**< CANXL(0x000E) */
    LSOF_PROTOCOL_PPPTALK,    /**< ATALK over PPP(0x0010) */
    LSOF_PROTOCOL_TR_802_2,   /**< 802.2(0x0011) */
    LSOF_PROTOCOL_MOBITEX,    /**< MOBITEX(0x0015) */
    LSOF_PROTOCOL_CONTROL,    /**< CONTROL(0x0016) */
    LSOF_PROTOCOL_IRDA,       /**< IRDA(0x0017) */
    LSOF_PROTOCOL_ECONET,     /**< ECONET(0x0018) */
    LSOF_PROTOCOL_HDLC,       /**< HDLC(0x0019) */
    LSOF_PROTOCOL_ARCNET,     /**< ARCNET(0x001A) */
    LSOF_PROTOCOL_DSA,        /**< DSA(0x001B) */
    LSOF_PROTOCOL_TRAILER,    /**< TRAILER(0x001C) */
    LSOF_PROTOCOL_LOOP,       /**< LOOP(0x0060) */
    LSOF_PROTOCOL_PHONET,     /**< PHONET(0x00F5) */
    LSOF_PROTOCOL_IEEE802154, /**< IEEE 802.15.4(0x00F6) */
    LSOF_PROTOCOL_CAIF,       /**< CAIF(0x00F7) */
    LSOF_PROTOCOL_XDSA,       /**< XDSA(0x00F8) */
    LSOF_PROTOCOL_MAP,        /**< MAP(0x00F9) */
    LSOF_PROTOCOL_MCTP,       /**< MCTP(0x00FA) */
    LSOF_PROTOCOL_PUPAT,      /**< PUPAT(0x0201) */
    LSOF_PROTOCOL_802_3_MIN,  /**< 802_3_MIN(0x0600) */
    LSOF_PROTOCOL_X25,        /**< X25(0x0805) */
    LSOF_PROTOCOL_ARP,        /**< ARP(0x0806) */
    LSOF_PROTOCOL_BPQ,        /**< BPQ(0x08FF) */
    LSOF_PROTOCOL_IEEEPUP,    /**< IEEEPUP(0x0A00) */
    LSOF_PROTOCOL_IEEEPUPAT,  /**< IEEEPUPAT(0x0A01) */
    LSOF_PROTOCOL_ERSPAN2,    /**< ERSPAN2(0x22EB) */
    LSOF_PROTOCOL_TSN,        /**< TSN(0x22F0) */
    LSOF_PROTOCOL_BATMAN,     /**< BATMAN(0x4305) */
    LSOF_PROTOCOL_DEC,        /**< DEC(0x6000) */
    LSOF_PROTOCOL_DNA_DL,     /**< DNA_DL(0x6001) */
    LSOF_PROTOCOL_DNA_RC,     /**< DNA_RC(0x6002) */
    LSOF_PROTOCOL_DNA_RT,     /**< DNA_RT(0x6003) */
    LSOF_PROTOCOL_LAT,        /**< LAT(0x6004) */
    LSOF_PROTOCOL_DIAG,       /**< DIAG(0x6005) */
    LSOF_PROTOCOL_CUST,       /**< CUST(0x6006) */
    LSOF_PROTOCOL_SCA,        /**< SCA(0x6007) */
    LSOF_PROTOCOL_TEB,        /**< TEB(0x6558) */
    LSOF_PROTOCOL_RARP,       /**< RARP(0x8035) */
    LSOF_PROTOCOL_ATALK,      /**< ATALK(0x809B) */
    LSOF_PROTOCOL_AARP,       /**< AARP(0x80F3) */
    LSOF_PROTOCOL_8021Q,      /**< 8021Q(0x8100) */
    LSOF_PROTOCOL_IPX,        /**< IPX(0x8137) */
    LSOF_PROTOCOL_PAUSE,      /**< PAUSE(0x8808) */
    LSOF_PROTOCOL_SLOW,       /**< SLOW(0x8809) */
    LSOF_PROTOCOL_WCCP,       /**< WCCP(0x883E) */
    LSOF_PROTOCOL_MPLS_UC,    /**< MPLS_UC(0x8847) */
    LSOF_PROTOCOL_MPLS_MC,    /**< MPLS_MC(0x8848) */
    LSOF_PROTOCOL_ATMMPOA,    /**< ATMMPOA(0x884C) */
    LSOF_PROTOCOL_PPP_DISC,   /**< PPP_DISC(0x8863) */
    LSOF_PROTOCOL_PPP_SES,    /**< PPP_SES(0x8864) */
    LSOF_PROTOCOL_LINK_CTL,   /**< LINK_CTL(0x886C) */
    LSOF_PROTOCOL_ATMFATE,    /**< ATMFATE(0x8884) */
    LSOF_PROTOCOL_PAE,        /**< PAE(0x888E) */
    LSOF_PROTOCOL_PROFINET,   /**< PROFINET(0x8892) */
    LSOF_PROTOCOL_REALTEK,    /**< REALTEK(0x8899) */
    LSOF_PROTOCOL_AOE,        /**< AOE(0x88A2) */
    LSOF_PROTOCOL_ETHERCAT,   /**< ETHERCAT(0x88A4) */
    LSOF_PROTOCOL_8021AD,     /**< 8021AD(0x88A8) */
    LSOF_PROTOCOL_802_EX1,    /**< 802_EX1(0x88B5) */
    LSOF_PROTOCOL_ERSPAN,     /**< ERSPAN(0x88BE) */
    LSOF_PROTOCOL_PREAUTH,    /**< PREAUTH(0x88C7) */
    LSOF_PROTOCOL_TIPC,       /**< TIPC(0x88CA) */
    LSOF_PROTOCOL_LLDP,       /**< LLDP(0x88CC) */
    LSOF_PROTOCOL_MRP,        /**< MRP(0x88E3) */
    LSOF_PROTOCOL_MACSEC,     /**< MACSEC(0x88E5) */
    LSOF_PROTOCOL_8021AH,     /**< 8021AH(0x88E7) */
    LSOF_PROTOCOL_MVRP,       /**< MVRP(0x88F5) */
    LSOF_PROTOCOL_1588,       /**< 1588(0x88F7) */
    LSOF_PROTOCOL_NCSI,       /**< NCSI(0x88F8) */
    LSOF_PROTOCOL_PRP,        /**< PRP(0x88FB) */
    LSOF_PROTOCOL_CFM,        /**< CFM(0x8902) */
    LSOF_PROTOCOL_FCOE,       /**< FCOE(0x8906) */
    LSOF_PROTOCOL_TDLS,       /**< TDLS(0x890D) */
    LSOF_PROTOCOL_FIP,        /**< FIP(0x8914) */
    LSOF_PROTOCOL_IBOE,       /**< IBOE(0x8915) */
    LSOF_PROTOCOL_80221,      /**< 802.21(0x8917) */
    LSOF_PROTOCOL_HSR,        /**< HSR(0x892F) */
    LSOF_PROTOCOL_NSH,        /**< NSH(0x894F) */
    LSOF_PROTOCOL_LOOPBACK,   /**< LOOPBACK(0x9000) */
    LSOF_PROTOCOL_QINQ1,      /**< QINQ1(0x9100) */
    LSOF_PROTOCOL_QINQ2,      /**< QINQ2(0x9200) */
    LSOF_PROTOCOL_QINQ3,      /**< QINQ3(0x9300) */
    LSOF_PROTOCOL_EDSA,       /**< EDSA(0xDADA) */
    LSOF_PROTOCOL_DSA_8021Q,  /**< DSA 802.1Q(0xDADB) */
    LSOF_PROTOCOL_DSA_A5PSW,  /**< DSA_A5PSW(0xE001) */
    LSOF_PROTOCOL_IFE,        /**< IFE(0xED3E) */
    LSOF_PROTOCOL_AF_IUCV,    /**< AF_IUCV(0xFBFB) */
    /* others */
    LSOF_PROTOCOL_8025,   /**< 802.5 */
    LSOF_PROTOCOL_CCITT,  /**< CCITT */
    LSOF_PROTOCOL_STR,    /**< stream */
    LSOF_PROTOCOL_SHARED, /**< shared */
};

/** @struct lsof_context
 * Hidden struct of lsof context, use `lsof_new()` to get one
 */
struct lsof_context;

/** An open file
 *
 */
struct lsof_file {
    /* FD column */
    /** File desciptor type */
    enum lsof_fd_type fd_type;

    /** File descriptor number, valid if \ref fd_type == \ref LSOF_FD_NUMERIC */
    uint32_t fd_num;

    /** File access mode */
    enum lsof_file_access_mode access;

    /** File lock mode */
    enum lsof_lock_mode lock;

    /* TYPE column */
    /** File type */
    enum lsof_file_type file_type;
    /** Store raw file type number when \ref file_type == \ref LSOF_FILE_UNKNOWN
     */
    uint32_t unknown_file_type_number;

    /* DEVICE column */
    /** Device ID of device containing file, use major() and minor() to extract
     * major and minor components */
    uint64_t dev;

    /** Whether \ref dev field is valid */
    uint8_t dev_valid;

    /* SIZE, SIZE/OFF, OFFSET column */
    /** File size */
    uint64_t size;

    /** Whether \ref size field is valid */
    uint8_t size_valid;

    /** File offset */
    uint64_t offset;

    /** Whether \ref offset field is valid */
    uint8_t offset_valid;

    /* NLINK column */
    /** Link count */
    uint64_t num_links;

    /** Whether \ref num_links field is valid */
    uint8_t num_links_valid;

    /* NODE column */
    /** File inode */
    uint64_t inode;

    /** Whether \ref inode field is valid */
    uint8_t inode_valid;

    /** Network protocol */
    enum lsof_protocol protocol;

    /** Store proto number when \ref protocol == \ref LSOF_PROTOCOL_UNKNOWN */
    uint32_t unknown_proto_number;

    /* NAME column */
    /** File name or description */
    char *name;
};

/** The result of lsof_gather(), grouped by process
 *
 * For each process, you can find a linked list of open files at `files`
 */
struct lsof_process {
    /* COMMAND column */
    char *command; /**< command name */
    /* PID column */
    uint32_t pid; /**< process ID */

    /* TID column */
    uint32_t tid; /**< task ID */
    /* TASKCMD column */
    char *task_cmd; /**< task command name */

    /* PGID column */
    uint32_t pgid; /**< process group ID */
    /* PPID column */
    uint32_t ppid; /**< parent process ID */
    /* USER column */
    uint32_t uid; /**< user ID */

    struct lsof_file *files; /**< array of open files */
    uint32_t num_files;      /**< length of files array */
};

struct lsof_result {
    struct lsof_process *processes; /**< array of processes */
    size_t num_processes;           /**< length of processes array */
};

/** API version of liblsof
 * you may use this macro to check the existence of
 * functions
 */
#    define LSOF_API_VERSION 1

/** Get runtime API version of liblsof
 *
 * liblsof might not function properly if API version mismatched between compile
 * time and runtime.
 *
 * \since API version 1
 */
int lsof_get_api_version();

/** Get library version of liblsof
 *
 * \return a string like "4.xx.x". The caller must not free it.
 *
 * \since API version 1
 */
char *lsof_get_library_version();

/** Create a new lsof context
 *
 * The context should be freed via `lsof_destroy()`.
 *
 * \since API version 1
 */
struct lsof_context *lsof_new();

/** Set output stream for warning and error messages
 *
 * lsof may want to print warning and error messages to the user. You can allow
 * printing by setting the output stream and whether prints warning or not. You
 * should also supply `program_name` so that the output starts with your program
 * name.
 *
 * By default, the output is suppressed. You can set fp to NULL to suppress
 * output.
 *
 * \since API version 1
 */
enum lsof_error lsof_output_stream(struct lsof_context *ctx, FILE *fp,
                                   char *program_name, int warn);

/** Exit program upon fatal error
 *
 * Call this function to exit the program upon fatal error. It is useful in lsof
 * cli to simplify error handling.
 *
 * \since API version 1
 */
enum lsof_error lsof_exit_on_fatal(struct lsof_context *ctx, int exit);

/** Let lsof avoid using blocking functions
 *
 * lsof may block when calling lstat(), readlink() or stat(). Call this function
 * to let lsof avoid calling these functions.
 *
 * \since API version 1
 */
enum lsof_error lsof_avoid_blocking(struct lsof_context *ctx);

/** Let lsof avoid forking
 *
 * To avoid being blocked by some kernel operations, liblsof does them in forked
 * child processes. Call this function to change this behavior.
 *
 * \since API version 1
 */
enum lsof_error lsof_avoid_forking(struct lsof_context *ctx, int avoid);

/** Let lsof AND the selections
 *
 * By default, lsof OR the selections, for example, if you call
 * lsof_select_unix_socket() and lsof_select_login(), it will report unix
 * sockets OR open files by the user. If lsof_logic_and() is called, it will
 * report unix sockets open by the specified user.
 *
 * \since API version 1
 */
enum lsof_error lsof_logic_and(struct lsof_context *ctx);

/** Ask lsof to select process by command
 *
 * Select process executing the command that begins with the characters of
 * `command`. You can specify exclusion by setting `exclude` to 1.
 *
 * You can call this function multiple times to add more search conditions.
 *
 * \since API version 1
 */
enum lsof_error lsof_select_process(struct lsof_context *ctx, char *command,
                                    int exclude);

/** Ask lsof to select process by matching regex
 *
 * Select process executing the command that matches with the
 * `regex`.
 *
 * `regex` must begin and end with a slash ('/'), the characters between the
 * slashes are interpreted as a regular expression.
 *
 * The closing slash may be followed by these modifiers:
 * - b the regular expression is a basic one.
 * - i ignore the case of letters.
 * - x the regular expression is an extended one (default).
 *
 * You can call this function multiple times to add more search conditions.
 *
 * \since API version 1
 */
enum lsof_error lsof_select_process_regex(struct lsof_context *ctx,
                                          char *regex);

/** Ask lsof to select process by pid (process id)
 *
 * Select process by comparing pid. You can specify exclusion by setting
 * `exclude` to 1.
 *
 * You can call this function multiple times to add more search conditions.
 *
 * \since API version 1
 */
enum lsof_error lsof_select_pid(struct lsof_context *ctx, int32_t pid,
                                int exclude);

/** Ask lsof to select process by pgid (process group id)
 *
 * Select process by comparing pgid. You can specify exclusion by setting
 * `exclude` to 1.
 *
 * You can call this function multiple times to add more search conditions.
 *
 * \since API version 1
 */
enum lsof_error lsof_select_pgid(struct lsof_context *ctx, int32_t pgid,
                                 int exclude);

/** Ask lsof to select process by uid
 *
 * Select process whose user id equals to or not equals to `uid`
 *
 * You can call this function multiple times to add more search conditions.
 *
 * \since API version 1
 */
enum lsof_error lsof_select_uid(struct lsof_context *ctx, uint32_t uid,
                                int exclude);

/** Ask lsof to select process by user loginc
 *
 * Select process whose user login name equals to or not equals to `login`
 *
 * You can call this function multiple times to add more search conditions.
 *
 * \since API version 1
 */
enum lsof_error lsof_select_login(struct lsof_context *ctx, char *login,
                                  int exclude);

/** Ask lsof to select process by fd type/number
 *
 * Select fd by type(cwd, txt, etc.) or by number(0, 1, 2, etc.). If you want to
 * select fd by type, `fd_num_hi` and `fd_num_lo` paramters are ignored. If you
 * want to select fd by num, set `fd_type` to `LSOF_FD_NUMERIC` and pass the
 * lower and higher bound of fd number.
 *
 * A mixture of exclusions and inclusions is not allowed.
 *
 * You can call this function multiple times to add more search conditions.
 *
 * \since API version 1
 */
enum lsof_error lsof_select_fd(struct lsof_context *ctx,
                               enum lsof_fd_type fd_type, int fd_num_lo,
                               int fd_num_hi, int exclude);

/** Ask lsof to select internet sockets
 *
 * Select internet socket by protocol: 0 means any, 4 means IPv4 only, 6 means
 * IPv6 only.
 *
 * You can call this function multiple times to add more search conditions.
 *
 * \since API version 1
 */
enum lsof_error lsof_select_inet(struct lsof_context *ctx, int inet_proto);

/** Ask lsof to select unix sockets
 *
 * You can call this function multiple times to add more search conditions.
 *
 * \since API version 1
 */
enum lsof_error lsof_select_unix_socket(struct lsof_context *ctx);

/** Ask lsof to select tasks(threads)
 *
 * lsof can report tasks(threads) of processes. Set `show` to 1 to report
 * tasks, or 0 to ignore tasks.
 *
 * \since API version 1
 */
enum lsof_error lsof_select_task(struct lsof_context *ctx, int show);

/** Ask lsof to select by protocol(TCP/UDP) states
 *
 * Select internet socket by protocol states, for example, TCP sockets in LISTEN
 * state. For TCP, see `tcp` to 1, otherwise 0 for UDP.
 *
 * \since API version 1
 */
enum lsof_error lsof_select_proto_state(struct lsof_context *ctx, int tcp,
                                        char *state, int exclude);

/** Ask lsof to select files in NFS
 *
 * \since API version 1
 */
enum lsof_error lsof_select_nfs(struct lsof_context *ctx);

/** Ask lsof to select files by num links
 *
 * Select files whose number of links are less than `threshold`. Set `threshold`
 * to 0 to undo the selection.
 *
 * \since API version 1
 */
enum lsof_error lsof_select_num_links(struct lsof_context *ctx, int threshold);

/** Ask lsof to exempt file system for blocking stat, lstat and readlink
 * calls
 *
 * If `avoid_readlink` is non-zero, lsof will avoid readlink calls.
 *
 * \since API version 1
 */
enum lsof_error lsof_exempt_fs(struct lsof_context *ctx, char *path,
                               int avoid_readlink);

/** Freeze the lsof context
 *
 * You can only call it once per context. After this call, no more options can
 * be changed.
 *
 * \since API version 1
 */
enum lsof_error lsof_freeze(struct lsof_context *ctx);

/** List open files, grouped by processes
 *
 * The result is a struct lsof_result, saved into `result` paramter.
 *
 * You should not alter the content of `result`, nor call `free()` to
 * pointers within. You should free `result` by calling
 * `lsof_free_result()`
 *
 * \return LSOF_INVALID_ARGUMENT if either pointer argument is NULL, or the
 * context is not frozen.
 *
 * \since API version 1
 */
enum lsof_error lsof_gather(struct lsof_context *ctx,
                            struct lsof_result **result);

/** Destroy a lsof context
 *
 * You must not use the context anymore after this call.
 *
 * \since API version 1
 */
void lsof_destroy(struct lsof_context *ctx);

/** Free struct lsof_result
 *
 * \since API version 1
 */
void lsof_free_result(struct lsof_result *result);

#endif /* LSOF_H */
