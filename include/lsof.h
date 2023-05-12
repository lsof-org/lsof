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
 * $Id: lsof.h,v 1.70 2018/03/26 21:50:45 abe Exp $
 */

#if !defined(LSOF_H)
#    define LSOF_H 1

/** lsof error returns */
enum lsof_error {
    LSOF_SUCCESS = 0,            /**< Success */
    LSOF_ERROR_INVALID_ARGUMENT, /**< Invalid argument */
    LSOF_ERROR_NO_MEMORY,        /**< No memory */
    LSOF_ERROR_UNSUPPORTED,      /**< Unsupported operation */
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
    LSOF_LOCK_SCO_PARTIAL,   /**< SCO OpenServer lock on part of the file */
    LSOF_LOCK_SCO_FULL,      /**< SCO OpenServer lock on the entire file */
};

/** File descriptor type */
enum lsof_fd_type {
    LSOF_FD_NUMERIC,         /**< Numeric fd opened in process */
    LSOF_FD_UNKNOWN,         /**< Unknown fd type */
    LSOF_FD_CWD,             /**< Current working directory */
    LSOF_FD_ERROR,           /**< Failed to get fd information */
    LSOF_FD_NOFD,            /**< No file descriptors */
    LSOF_FD_ROOT_DIR,        /**< Root directory */
    LSOF_FD_PARENT_DIR,      /**< Parent directory */
    LSOF_FD_PROGRAM_TEXT,    /**< Program text */
    LSOF_FD_LIBRARY_TEXT,    /**< Library text */
    LSOF_FD_MEMORY,          /**< Memory-mapped file */
    LSOF_FD_DELETED,         /**< Deleted file */
    LSOF_FD_FILEPORT,        /**< Darwin fileport */
    LSOF_FD_TASK_CWD,        /**< Per task/thread cwd */
    LSOF_FD_CTTY,            /**< Character TTY */
    LSOF_FD_JAIL_DIR,        /**< Jail directory */
    LSOF_FD_VIRTUAL_8086,    /**< Virtual 8086 */
    LSOF_FD_MERGE_386,       /**< MERGE386 vm86 region */
    LSOF_FD_MMAP_DEVICE,     /**< Memory-mapped device */
    LSOF_FD_LIBRARY_REF,     /**< Library references */
    LSOF_FD_MMAP_UNKNOWN,    /**< Unknown memory-mapped file */
    LSOF_FD_PREGION_UNKNOWN, /**< Unknown HP-UX pregion */
};

/** File type */
enum lsof_file_type {
    LSOF_FILE_NONE, /**< No file type */

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
    LSOF_FILE_PROC_AS,             /**< Solaris /proc/<PID>/as file */
    LSOF_FILE_PROC_AUXV,           /**< /proc/<PID>/auxv file */
    LSOF_FILE_PROC_CRED,           /**< Solaris /proc/<PID>/cred file */
    LSOF_FILE_PROC_CTRL,           /**< /proc/<PID>/ctl control file */
    LSOF_FILE_PROC_CUR_PROC,       /**< NetBSD /proc/curproc file */
    LSOF_FILE_PROC_CWD,            /**< Solaris /proc/<PID>/cwd folder */
    LSOF_FILE_PROC_DIR,            /**< /proc directory */
    LSOF_FILE_PROC_EXEC_TYPE,      /**< FreeBSD /proc executable type (etype) */
    LSOF_FILE_PROC_FD,             /**< /proc/<PID>/fd/<FD> file */
    LSOF_FILE_PROC_FD_DIR,         /**< /proc/<PID>/fd directory */
    LSOF_FILE_PROC_FILE,           /**< /proc/<PID>/file executable file */
    LSOF_FILE_PROC_FP_REGS,        /**< /proc/<PID>/fpregs fp registers */
    LSOF_FILE_PROC_PAGE_DATA,      /**< Solaris /proc/<PID>/pagedata file */
    LSOF_FILE_PROC_GROUP_NOTIFIER, /**< /proc/<PID>/notepg group notifier */
    LSOF_FILE_PROC_LDT,            /**< Solaris /proc/<PID>/ldt file */
    LSOF_FILE_PROC_LPS_INFO,       /**< Solaris /proc/<PID>/lpsinfo file */
    LSOF_FILE_PROC_LSTATUS,        /**< Solaris /proc/<PID>/lstatus file */
    LSOF_FILE_PROC_LUSAGE,         /**< Solaris /proc/<PID>/lusage file */
    LSOF_FILE_PROC_LWP_GWINDOWS,   /**< Solaris /proc/<PID>/lwp/<LWPID>/gwindows
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
    LSOF_FILE_PROC_OLD_PAGE,      /**< Solaris old /proc/<PID> page data file */
    LSOF_FILE_PROC_REGS,          /**< /proc/<PID>/regs register set */
    LSOF_FILE_PROC_RMAP,          /**< Solaris /proc/<PID>/rmap file */
    LSOF_FILE_PROC_ROOT,          /**< Solaris /proc/<PID>/root directory */
    LSOF_FILE_PROC_SIGACT,        /**< Solaris /proc/<PID>/sigact file */
    LSOF_FILE_PROC_PSINFO,        /**< Solaris /proc/<PID>/psinfo file */
    LSOF_FILE_PROC_STATUS,        /**< /proc/<PID>/status status file */
    LSOF_FILE_PROC_USAGE,         /**< Solaris /proc/<PID>/usage file */
    LSOF_FILE_PROC_WATCH,         /**< Solaris /proc/<PID>/watch file */
    LSOF_FILE_PROC_XMAP,          /**< Solaris /proc/<PID>/xmap file */

    /* Others */
    LSOF_FILE_ANON_INODE,        /**< anonymous inode */
    LSOF_FILE_DEL,               /**< Linux map file that has been deleted */
    LSOF_FILE_DOOR,              /**< Solaris VDOOR file */
    LSOF_FILE_KQUEUE,            /**< BSD style kernel event file */
    LSOF_FILE_FSEVENTS,          /**< fsevents file */
    LSOF_FILE_EVENTFD,           /**< eventfd file */
    LSOF_FILE_PROCDESC,          /**< process descriptor file */
    LSOF_FILE_MULTIPLEXED_BLOCK, /**< SCO OpenServer multiplexed block file */
    LSOF_FILE_MULTIPLEXED_CHAR,  /**< SCO OpenServer multiplexed char file */
    LSOF_FILE_UNKNOWN_DELETED,   /**< Linux unknown deleted file */
    LSOF_FILE_UNKNOWN_MEMORY,    /**< Linux unknown memory file */
    LSOF_FILE_UNKNOWN_FD,        /**< Linux unknown fd */
    LSOF_FILE_UNKNOWN_CWD,       /**< Linux unknown cwd */
    LSOF_FILE_UNKNOWN_ROOT_DIR,  /**< Linux unknown root dir */
    LSOF_FILE_UNKNOWN_PROGRAM_TEXT, /**< Linux unknown program text */
    LSOF_FILE_UNKNOWN_STAT,         /**< Linux unknown due to failed stat() */
    LSOF_FILE_UNKNOWN_RAW, /**< Unknown file type, raw numbers provided in
                              unknown_file_type_number */
    LSOF_FILE_UNKNOWN,     /**< Unknown file type without raw number */
    LSOF_FILE_PIPE,        /**< pipes */
    LSOF_FILE_PORT,        /**< Solaris SYSV named pipe */
    LSOF_FILE_POSIX_MQ,    /**< POSIX named message queue file */
    LSOF_FILE_POSIX_SEMA,  /**< POSIX named semaphore file */
    LSOF_FILE_POSIX_SHM,   /**< POSIX named shared memory file */
    LSOF_FILE_SHM,         /**< SystemV shared memory file */
    LSOF_FILE_PTS,         /**< FreeBSD /dev/pts file */
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
    LSOF_FILE_VNODE_VCHR,  /**< The vnode represents a char special device */
    LSOF_FILE_VNODE_VLNK,  /**< The vnode represents a symbolic link */
    LSOF_FILE_VNODE_VSOCK, /**< The vnode represents a socket */
    LSOF_FILE_VNODE_VBAD,  /**< The vnode represents a bad file */
    LSOF_FILE_VNODE_VMPC,  /**< The vnode represents a multiplexed character
                              special device */
    LSOF_FILE_VNODE_VFIFO, /**< The vnode represents a FIFO file */
    LSOF_FILE_VNODE_VUNNAMED, /**< The vnode represents an unnamed file */
    LSOF_FILE_VNODE_VDOOR,    /**< The vnode represents a door */
    LSOF_FILE_VNODE_VPORT,    /**< The vnode represents a port */
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

/** Ask lsof to avoid using blocking functions
 *
 * lsof may block when calling lstat(), readlink() or stat(). Call this function
 * with `avoid=1` to let lsof avoid calling these functions.
 *
 * \since API version 1
 */
enum lsof_error lsof_avoid_blocking(struct lsof_context *ctx, int avoid);

/** Ask lsof to avoid forking
 *
 * To avoid being blocked by some kernel operations, liblsof does them in forked
 * child processes. Call this function to change this behavior.
 *
 * \since API version 1
 */
enum lsof_error lsof_avoid_forking(struct lsof_context *ctx, int avoid);

/** Ask lsof to AND the selections
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

#endif