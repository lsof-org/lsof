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
 * access_to_char() - convert enum lsof_file_access_mode to char
 */
char access_to_char(enum lsof_file_access_mode access) {
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
 * lock_to_char() - convert enum lsof_lock_mode to char
 */
char lock_to_char(enum lsof_lock_mode lock) {
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
 * file_type_to_string() - convert enum lsof_file_type to string
 */
void file_type_to_string(enum lsof_file_type type,
                         uint32_t unknown_file_type_number, char *buf,
                         size_t buf_len) {
    switch (type) {
    default:
    case LSOF_FILE_UNKNOWN_RAW:
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
        (void)snpf(buf, buf_len, "LINK");
        break;
    /* Use lower case for network-related files except IPv4/IPv6/ATALK for
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
        (void)snpf(buf, buf_len, "ATALK");
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
    case LSOF_FILE_PROC_LWP_CTL:
        (void)snpf(buf, buf_len, "PLC");
        break;
    case LSOF_FILE_PROC_LWP_DIR:
        (void)snpf(buf, buf_len, "PLDR");
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
    case LSOF_FILE_UNKNOWN:
        (void)snpf(buf, buf_len, "UNKN");
        break;
    case LSOF_FILE_UNKNOWN_STAT:
        (void)snpf(buf, buf_len, "unknown");
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
        (void)snpf(buf, buf_len, "SOCK");
        break;
    case LSOF_FILE_VNODE_VBAD:
        (void)snpf(buf, buf_len, "VBAD");
        break;
    case LSOF_FILE_VNODE_VMPC:
        (void)snpf(buf, buf_len, "VMPC");
        break;
    case LSOF_FILE_VNODE_VFIFO:
        (void)snpf(buf, buf_len, "FIFO");
        break;
    case LSOF_FILE_VNODE_VDOOR:
        (void)snpf(buf, buf_len, "DOOR");
        break;
    case LSOF_FILE_VNODE_VPORT:
        (void)snpf(buf, buf_len, "PORT");
        break;
    case LSOF_FILE_VNODE_VUNNAMED:
        (void)snpf(buf, buf_len, "UNNM");
        break;
    }
}