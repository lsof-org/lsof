/* ipc_pstat_body.h : IPC pst_*info body declarations and user constants */

/*   @(#)B11.23_LR   */
/* NO MULTIPLE-INCLUDE PROTECTION (intentional) */


/*
 * Constants
 *	PSI_SHMMAX
 *	PS_SEM_ALLOC
 *	PS_MSG_ALLOC, PS_MSG_RWAIT, PS_MSG_WWAIT
 *	PS_SHM_ALLOC, PS_SHM_DEST, PS_SHM_CLEAR
 *	PSH_SEGSZ
 *
 * Size-invariant types
 *	none
 *
 * Size-variant types
 *  Defines:
 *	__pst_ipcinfo
 *	__pst_seminfo
 *	__pst_msginfo
 *	__pst_shminfo
 *
 *  Uses:
 *	none
 *
 */


/* pst_ipcinfo support */


/*
 * This structure describes the system-wide global System V IPC constants.
 * These are typically (currently) defined at boot time, but may become
 * dynamic in future releases.
 */
struct __pst_ipcinfo {
	_T_LONG_T	psi_semmap; /* OBSOLETE resource map size for SysV semaphores
                                     * kept for binary compatibility */
	_T_LONG_T	psi_semmni; /* number of identifiers for SysV sems */
	_T_LONG_T	psi_semmns; /* system-wide total of SysV sems */
	_T_LONG_T	psi_semmnu; /* system-wide total of SysV sem undo structs */
	_T_LONG_T	psi_semmsl; /* max # of sems per identifier */
	_T_LONG_T	psi_semopm; /* max # of operations per semop() call */
	_T_LONG_T	psi_semume; /* max # of undo entries per process */
	_T_LONG_T	psi_semusz; /* size in bytes of undo structure */
	_T_LONG_T	psi_semvmx; /* maximum value for sem */
	_T_LONG_T	psi_semaem; /* adjust-on-exit maximum value */

	_T_LONG_T	psi_msgmap; /* resource map size for SysV messages */
	_T_LONG_T	psi_msgmax; /* maximum message size */
	_T_LONG_T	psi_msgmnb; /* maximum bytes on message queue */
	_T_LONG_T	psi_msgmni; /* system-wide total of SysV msg queue IDs */
	_T_LONG_T	psi_msgssz; /* message segment size */
	_T_LONG_T	psi_msgtql; /* system-wide total of SysV msg headers */
	_T_LONG_T	psi_msgseg; /* system-wide total of SysV msg segments */

	_T_LONG_T	psi_shmmax; /* maximum shared memory segment size */
	_T_LONG_T	psi_shmmin; /* minimum shared memory segment size */
	_T_LONG_T	psi_shmmni; /* system-wide total of SysV shm identifiers */
	_T_LONG_T	psi_shmseg; /* max # of attached SysV shm segs per process */
	int32_t	psi_valid;  /* valid vector */
};

#if !defined(_PST_IPCINFO_CONSTANTS_DEFINED)
#define _PST_IPCINFO_CONSTANTS_DEFINED

/* Macros for field validity check in pst_ipcinfo */
#define PSI_SHMMAX       0x1

#endif /* !defined(_PST_IPCINFO_CONSTANTS_DEFINED) */


/* pst_seminfo support */


/*
 * This structure describes per-(System V semaphore set) information.
 * Each structure returned describes on semaphore identifier on the system.
 */

struct __pst_seminfo {
	_T_ULONG_T	pse_idx;	/* Idx for further pstat() requests */
	_T_LONG_T	pse_uid;	/* UID of semaphore set owner */
	_T_LONG_T	pse_gid;	/* GID of semaphore set owner */
	_T_LONG_T	pse_cuid;	/* UID of semaphore set creator */
	_T_LONG_T	pse_cgid;	/* GID of semaphore set creator */
	_T_ULONG_T	pse_mode;	/* mode of semaphore set (9 bits) */
	_T_ULONG_T	pse_seq;	/* sequence number of semaphore set */
	_T_ULONG_T	pse_key;	/* IPC key of semaphore set */
	_T_ULONG_T	pse_nsems;	/* number of semaphores for this set */
	_T_LONG_T	pse_otime;	/* last semop time (secs since 1970) */
	_T_LONG_T	pse_ctime;	/* last change time (since 1970) */
	_T_ULONG_T	pse_flags;	/* flags for the semaphore set */
	_T_LONG_T	pse_semid;	/* IPC identifier of semaphore set */
};

#if !defined(_PST_SEMINFO_CONSTANTS_DEFINED)
#define _PST_SEMINFO_CONSTANTS_DEFINED

/*
 * flag bit definitions for pse_flags
 */
#define PS_SEM_ALLOC	0x1	/* semaphore set is in use */

#endif /* !defined(_PST_SEMINFO_CONSTANTS_DEFINED) */


/* pst_msginfo support */

/*
 * This structure describes per-(System V message queue) information.
 * Each structure returned describes on message queue identifier on the system.
 */

struct __pst_msginfo {
	_T_ULONG_T	psm_idx;	/* Idx for further pstat() requests */
	_T_LONG_T	psm_uid;	/* UID of msg queue owner */
	_T_LONG_T	psm_gid;	/* GID of msg queue owner */
	_T_LONG_T	psm_cuid;	/* UID of msg queue creator */
	_T_LONG_T	psm_cgid;	/* GID of msg queue creator */
	_T_ULONG_T	psm_mode;	/* mode of msg queue (9 bits) */
	_T_ULONG_T	psm_seq;	/* sequence number of msg queue */
	_T_ULONG_T	psm_key;	/* IPC key of msg queue */
	_T_ULONG_T	psm_qnum;	/* number of msgs on this queue */
	_T_ULONG_T	psm_qbytes;	/* max bytes for msgs on this queue */
	_T_ULONG_T	psm_cbytes;	/* cur bytes for msgs on this queue */
	_T_LONG_T	psm_lspid;	/* pid of last msgsnd()er */
	_T_LONG_T	psm_lrpid;	/* pid of last msgrcv()er */
	_T_LONG_T	psm_stime;	/* last msgsnd() time (since 1970) */
	_T_LONG_T	psm_rtime;	/* last msgrcv() time (since 1970) */
	_T_LONG_T	psm_ctime;	/* last change time (since 1970) */
	_T_ULONG_T	psm_flags;	/* flags for the message queue */
	_T_LONG_T	psm_msgid;	/* IPC identifier for the msg queue */
};

#if !defined(_PST_MSGINFO_CONSTANTS_DEFINED)
#define _PST_MSGINFO_CONSTANTS_DEFINED
/*
 * Definitions of flag bits in psm_flags
 */
#define PS_MSG_ALLOC	0x1	/* message queue is in use */
#define PS_MSG_RWAIT	0x2	/* one or more processes waiting to read */
#define PS_MSG_WWAIT	0x4	/* one or more processes waiting to write */

#endif /* !defined(_PST_MSGINFO_CONSTANTS_DEFINED) */


/* pst_shminfo support */

/*
 * This structure describes per-(System V shared memory segment) information.
 * Each structure returned describes on segment identifier on the system.
 */

struct __pst_shminfo {
	_T_ULONG_T	psh_idx;	/* Idx for further pstat() requests */
	_T_LONG_T	psh_uid;	/* UID of shm segment owner */
	_T_LONG_T	psh_gid;	/* GID of shm segment owner */
	_T_LONG_T	psh_cuid;	/* UID of shm segment creator */
	_T_LONG_T	psh_cgid;	/* GID of shm segment creator */
	_T_ULONG_T	psh_mode;	/* mode of shm segment (9 bits) */
	_T_ULONG_T	psh_seq;	/* sequence number of shm segment */
	_T_ULONG_T	psh_key;	/* IPC key of shm segment id */
	_T_ULONG_T	psh_segsz;	/* size of shm segment (bytes) */
	_T_LONG_T	psh_cpid;	/* PID of shm segment creator */
	_T_LONG_T	psh_lpid;	/* PID of last shmop() */
	_T_ULONG_T	psh_nattch;	/* cur # of procs attached (accurate)*/
	_T_ULONG_T	psh_cnattch;	/* cur # attached/in mem (inaccurate)*/
	_T_LONG_T	psh_atime;	/* last shmat() time (since 1970) */
	_T_LONG_T	psh_dtime;	/* last shmdt() time (since 1970) */
	_T_LONG_T	psh_ctime;	/* last change time (since 1970) */
	_T_ULONG_T	psh_flags;	/* flags for the shm segment */
	int32_t		psh_valid;	/* This is a vector that will indicate 
					 * if a certain field in is valid */
	_T_LONG_T	psh_shmid;	/* IPC identifier for the shm 
					 * segment */
};

#if !defined(_PST_SHMINFO_CONSTANTS_DEFINED)
#define _PST_SHMINFO_CONSTANTS_DEFINED

/*
 * Flag bit definitions for psh_flags
 */
#define PS_SHM_ALLOC	0x1	/* shared mem segment is in use */
#define PS_SHM_DEST	0x2	/* shm segment to be deleted on last detach */
#define PS_SHM_CLEAR	0x4	/* shm segment to be zeroed on first attach */

/* Macros for field validity check in pst_shminfo */
#define PSH_SEGSZ       0x1

#endif /* !defined(_PST_SHMINFO_CONSTANTS_DEFINED) */



