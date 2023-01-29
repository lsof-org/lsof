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
    LSOF_FILE_UNKNOWN_PROGRAM_TEXT,  /**< Linux unknown program text */
    LSOF_FILE_PIPE,              /**< pipes */
    LSOF_FILE_PORT,              /**< Solarise SYSV named pipe */
    LSOF_FILE_POSIX_MQ,          /**< Linux POSIX message queue file */
    LSOF_FILE_POSIX_SEMA,        /**< Darwin POSIX semaphore file */
    LSOF_FILE_POSIX_SHM,         /**< Darwin POSIX shared memory file */
    LSOF_FILE_SHM,               /**< shared memory file */
    LSOF_FILE_PTS,               /**< FreeBSD /dev/pts file */
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

    /* Unknown, see unknown_file_type for raw value */
    LSOF_FILE_UNKNOWN, /**< Unknown file type, only raw numbers provided */
};

/** Network protocol */
enum lsof_protocol {
    LSOF_PROTOCOL_NONE, /**< No network protocol */
    LSOF_PROTOCOL_TCP,  /**< TCP */
    LSOF_PROTOCOL_UDP,  /**< UDP */
};

/** Display hint of NODE column for lsof cli */
enum lsof_display_node {
    LSOF_DISPLAY_NODE_NONE,          /**< Neither inode nor protocol is shown */
    LSOF_DISPLAY_NODE_INODE_DECIMAL, /**< Display inode in decimal */
    LSOF_DISPLAY_NODE_INODE_HEX,     /**< Display inode in hex */
    LSOF_DISPLAY_NODE_PROTOCOL,      /**< Display protocol */
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
    enum lsof_file_type file_type;

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

    /** Display hint of NODE column */
    enum lsof_display_node display_node_hint;

    /** Whether \ref protocol field is valid */
    uint8_t protocol_valid;

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
    int32_t pid; /**< process ID */

    /* TID column */
    int32_t tid; /**< task ID */
    /* TASKCMD column */
    char *task_cmd; /**< task command name */

    /* PGID column */
    int32_t pgid; /**< process group ID */
    /* PPID column */
    int32_t ppid; /**< parent process ID */
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
