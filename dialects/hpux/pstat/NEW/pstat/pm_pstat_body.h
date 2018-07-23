/*
 * BEGIN_DESC
 *
 *  File:
 *      @(#)B11.23_LR    common/pstat/pm_pstat_body.h
 *
 *  Purpose:
 * 	pst_status body declarations and user constants 
 *
 *  Classification:                     Release to Release Consistency Req:
 *      public                                  binary & source
 *
 *  BE header:  yes
 *
 *  Shipped:  yes
 *      /usr/include/pstat/pm_pstat_body.h
 *
 * END_DESC
 */


/* NO MULTIPLE-INCLUDE PROTECTION (intentional) */

/*
 * Constants
 *	PST_CLEN, PST_UCOMMLEN
 *	pst_major, pst_minor
 *	PS_INCORE, PS_SYS, PS_LOCKED, PS_TRACE, PS_TRACE2, PS_64ASL
 *	PS_TXTLOCKED, PS_DATLOCKED, PS_SYSCALLTRACE, PS_SWLAZY
 *	PST_ADDR, PST_WCHAN, PST_MAXRSS
 *
 * Size-invariant types
 *	none
 *
 * Size-variant types
 *  Defines:
 *	__pst_status
 *
 *  Uses:
 *	__psdev
 *	__psfileid
 *	__pstcycles
 *	_T_ULONG_T
 *	_T_LONG_T
 *
 */



#if !defined(PST_CLEN)

/*
 * lengths for cached command line and u_comm entries
 */
#define PST_CLEN 64
#define PST_UCOMMLEN (14 + 1)
#endif /* !defined(PST_CLEN) */

/*
 * This structure contains per-process information
 */
struct __pst_status {
	_T_LONG_T pst_idx;/* Index for further pstat() requests */
	_T_LONG_T pst_uid;	/* Real UID */
	_T_LONG_T pst_pid;	/* Process ID */
	_T_LONG_T pst_ppid;	/* Parent process ID */
	_T_LONG_T pst_dsize;	/* # real pages used for data */
	_T_LONG_T pst_tsize;	/* # real pages used for text */
	_T_LONG_T pst_ssize;	/* # real pages used for stack */
	_T_LONG_T pst_nice;	/* Nice value */
	struct __psdev	/* TTY of this process; -1/-1 if there isn't one */
	    pst_term;
	_T_LONG_T pst_pgrp;	/* Process group of this process */
	_T_LONG_T pst_pri;	/* priority of process */
	_T_LONG_T pst_addr;	/* address of process (in memory) */
	_T_LONG_T pst_cpu;	/* processor utilization for scheduling */
	_T_LONG_T pst_utime;	/* user time spent executing (in seconds) */
	_T_LONG_T pst_stime;	/* system time spent executing (in seconds) */
	_T_LONG_T pst_start;	/* time process started (seconds since epoch) */
	_T_LONG_T pst_flag;	/* flags associated with process */
	_T_LONG_T pst_stat;	/* Current status */
	_T_LONG_T pst_wchan;	/* If state PS_SLEEP, value sleeping on */
	_T_LONG_T pst_procnum;	/* processor this proc last run on */
	char		/* Command line for the process, if available */
	    pst_cmd[PST_CLEN];
	_T_LONG_T pst_time;	/* (deprecated) resident time for scheduling; 
				   use pst_resident instead */
	_T_LONG_T pst_cpticks;	/* ticks of cpu time */
	_T_LONG_T pst_cptickstotal; /* total ticks for life of process */
	_T_LONG_T pst_fss;	/* fair share scheduler group id */
	float pst_pctcpu;	/* %cpu for this process(smoothed) */
	_T_LONG_T pst_rssize;  /* resident set size for process (private pages) */
	_T_LONG_T pst_suid;    /* saved UID */
	char		     /* executable basename the process is running */
	    pst_ucomm[PST_UCOMMLEN];
	_T_LONG_T pst_shmsize;	/* # real pages used for shared memory */
	_T_LONG_T pst_mmsize;	/* # real pages used for memory mapped files */
	_T_LONG_T pst_usize;	/* # real pages used for U-Area & K-Stack */
	_T_LONG_T pst_iosize;	/* # real pages used for I/O device mapping */
	_T_LONG_T pst_vtsize;	/* # virtual pages used for text */
	_T_LONG_T pst_vdsize;	/* # virtual pages used for data */
	_T_LONG_T pst_vssize;	/* # virtual pages used for stack */
	_T_LONG_T pst_vshmsize;	/* # virtual pages used for shared memory */
	_T_LONG_T pst_vmmsize;	/* # virtual pages used for mem-mapped files */
	_T_LONG_T pst_vusize;	/* # virtual pages used for U-Area & K-Stack */
	_T_LONG_T pst_viosize;	/* # virtual pages used for I/O dev mapping */
	_T_ULONG_T pst_minorfaults; /* # page reclaims for the process */
	_T_ULONG_T pst_majorfaults; /* # page faults needing disk access */
	_T_ULONG_T pst_nswap;	/* # of swaps for the process */
	_T_ULONG_T pst_nsignals;	/* # signals received by the process */
	_T_ULONG_T pst_msgrcv;	/* # socket msgs received by the proc*/
	_T_ULONG_T pst_msgsnd;	/* # of socket msgs sent by the proc */
	_T_LONG_T pst_maxrss;	/* highwater mark for proc resident set size */
	_T_LONG_T pst_sid;		/* session ID */
	_T_LONG_T pst_schedpolicy;	/* scheduling policy for the process */
	_T_LONG_T pst_ticksleft;	/* clock ticks left in process' RR timeslice */
	struct __psfileid
	  pst_rdir;		/* File ID of the process' root directory */
	struct __psfileid
	  pst_cdir;		/* File ID of the process' current directory */
	struct __psfileid
	  pst_text;		/* File ID of the process' executable */
	_T_LONG_T pst_highestfd;	/* highest file descriptor currently opened */
	_T_LONG_T pst_euid;	/* Effective UID */
	_T_LONG_T pst_egid;	/* Effective GID */
	_T_LONG_T pst_ioch;	/* # of characters read/written */
	struct __pstcycles
	  pst_usercycles;	/* 64-bit user mode execution cycle count */
	struct __pstcycles
	  pst_systemcycles;	/* 64-bit system mode execution cycle count */
	struct __pstcycles
	  pst_interruptcycles;	/* 64-bit interrupt for proc cycle count */
	_T_LONG_T pst_gid;	/* Real GID */
	_T_LONG_T pst_sgid;	/* saved effective gid */
	_T_LONG_T pst_nlwps;	/* # lwps within this process */
	struct pstsigset pst_psig;	/* signals pending to proc */
        _T_LONG_T pst_lwpid;  /* LWP identifier. NOTE: If this process is 
			     * multi-threaded, this is an lwpid of one of the 
			     * LWPs in the process at this exact moment, which 
			     * LWP is undefined (random)
			     */
	int32_t pst_valid; /* This is a vector that will indicate if a
			    * certain field in the structure is valid or not.
			    */	
	_T_LONG_T pst_text_size; /* Page size used for text objects. */
	_T_LONG_T pst_data_size; /* Page size used for data objects. */

        struct __pstcycles
          pst_child_usercycles;       /* dead children user mode execution
                                       * cycle count
                                       */
        struct __pstcycles
          pst_child_systemcycles;     /* dead children system mode execution
                                       * cycle count
                                       */
	struct __pstcycles
          pst_child_interruptcycles;  /* dead children interrupt
                                       * mode execution cycle count
                                       */
        struct __psttime
          pst_child_utime;        /* reaped child user mode execution time */
        struct __psttime
          pst_child_stime;        /* reaped child system mode execution time */
        _T_LONG_T pst_inblock;    /* block input operations */
        _T_LONG_T pst_oublock;    /* block output operations */
        _T_LONG_T pst_nvcsw;      /* voluntary context switches */
	_T_LONG_T pst_nivcsw;     /* involuntary context switches */
        _T_LONG_T pst_child_inblock;   /* reaped child block input operations */
        _T_LONG_T pst_child_oublock;   /* reaped child block output operations */
        _T_LONG_T pst_child_ioch;      /* reaped child # of characters
                                        read/written */
        _T_LONG_T pst_child_msgsnd;    /* reaped child # of messages sent */
        _T_LONG_T pst_child_msgrcv;    /* reaped child # of messages received */
        _T_LONG_T pst_child_nvcsw;     /* reaped child voluntary context
                                        switches */
	_T_LONG_T pst_child_nivcsw;    /* reaped child involuntary context
                                        switches */
        _T_LONG_T pst_child_minorfaults;    /* reaped child # of page reclaims */
        _T_LONG_T pst_child_majorfaults;    /* reaped child # of page faults
                                             needing disk access */
        _T_LONG_T pst_logical_node;	/* node this proc last run on	*/

	/* 
	 * current working directory 
	 */
	uint32_t pst_hi_fileid_cdir; /* per shared file id */
	uint32_t pst_lo_fileid_cdir;
	uint32_t pst_hi_nodeid_cdir; /* per vnode id */
	uint32_t pst_lo_nodeid_cdir;
	struct __pst_fid pst_fid_cdir; /* Cookie for current working 
					* directory */
	/* 
	 * Root directory 
	 */
	uint32_t pst_hi_fileid_rdir; /* per shared file id */
	uint32_t pst_lo_fileid_rdir;
	uint32_t pst_hi_nodeid_rdir; /* per vnode id */
	uint32_t pst_lo_nodeid_rdir;
	struct __pst_fid pst_fid_rdir;   /* Cookie for root directory */

	/* 
	 * Text file 
	 */
	uint32_t pst_hi_fileid_text; /* per shared file id */
	uint32_t pst_lo_fileid_text;
	uint32_t pst_hi_nodeid_text; /* per vnode id */
	uint32_t pst_lo_nodeid_text;
	struct __pst_fid pst_fid_text; /* Cookie for text file */
        _T_LONG_T pst_pset_id;          /* pset id to which this process is 
					 * assigned. */
	_T_LONG_T pst_rsesize;		/* # real pages used for RSE stack */
	_T_LONG_T pst_vrsesize;		/* # virtual pages used for RSE stack */
	_T_LONG_T pst_resident;    /* seconds memory resident or deactivated */
};


#if !defined(_PST_STATUS_CONSTANTS_DEFINED)
#define _PST_STATUS_CONSTANTS_DEFINED

#define	pst_major	pst_term.psd_major
#define	pst_minor	pst_term.psd_minor


/*
 * Process flag bits for pst_flag
 */
#define PS_INCORE	0x1	/* this process is in memory */
#define PS_SYS		0x2	/* this process is a system process */
#define PS_LOCKED	0x4	/* this process is locked in memory */
#define PS_TRACE	0x8	/* this process is being traced */
#define PS_TRACE2	0x10	/* this traced process has been waited for */ 
#define PS_TXTLOCKED	0x20	/* this process' text is locked in memory*/ 
#define PS_DATLOCKED	0x40	/* this process' data is locked in memory*/ 
#define PS_SYSCALLTRACE	0x80	/* per-process syscall tracing enabled */ 
#define PS_SWLAZY	0x100	/* process has associated Lazy Swap region(s) */ 
#define PS_64ASL	0x200	/* process has 64-bit address space layout */ 
#define PS_CHANGEDPRIV	0x400	/* process was or is privileged */
#define PS_GANGED       0x800   /* process is ganged */

/* 
 * Macros for field validity check for struct pst_status 
 */
#define PST_ADDR       0x1
#define PST_WCHAN      0x2
#define PST_MAXRSS     0x4

#endif /* !defined(_PST_STATUS_CONSTANTS_DEFINED) */
