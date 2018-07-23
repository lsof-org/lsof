/* lwp_pstat_body.h : lwp_status body declarations and user constants */

/*   @(#)B11.23_LR   */
/* NO MULTIPLE-INCLUDE PROTECTION (intentional) */

/*
 * Constants
 *	
 *
 * Size-invariant types
 *  Defines:
 *	lwpcycles
 *	lwp_status
 *
 * Uses:
 *	pstsigset
 *
 * Size-variant types
 *  Defines:
 *	none
 *
 *  Uses:
 *	none
 *
 */



/* lwp_status support */


struct lwpcycles {
	uint64_t psc_hi;        /* most significant bits */
	uint64_t psc_lo;        /* least significant bits */
};


/*
 * This structure contains per-lwp information
 */

struct lwp_status {
	int64_t lwp_idx;	/* Index for further pstat_get_lwp() reqs */
	int64_t lwp_lwpid;	/* lwp ID */
	int64_t lwp_pid;	/* pid that lwp belongs to */
	int64_t lwp_flag;	/* flags associated with lwp */
	int64_t lwp_stat;	/* Current status */
	int64_t lwp_wchan;	/* If state LWP_SLEEP, value sleeping on */
	int64_t lwp_pri;	/* priority of lwp */
	int64_t lwp_cpu;	/* (decaying) cpu utilization for scheduling */
	int64_t lwp_spu;	/* spu number lwp is assigned to */
	int64_t lwp_user_suspcnt;/* user-initiated suspend count */
	struct pstsigset lwp_sig;	/* signals pending to lwp */
	struct pstsigset lwp_sigmask;	/* current signal mask */
	int64_t lwp_schedpolicy;	/* scheduling policy */
	int64_t lwp_ticksleft;	/* clock ticks left in lwp's RR timeslice */
	int64_t lwp_start;	/* time lwp created (seconds since epoch) */
	uint64_t lwp_minorfaults;  	/* # page reclaims */
	uint64_t lwp_majorfaults;  	/* # page faults needing disk access */
	uint64_t lwp_ndeact;       	/* # deactivates */
	uint64_t lwp_inblock;      	/* # block input operations */
	uint64_t lwp_oublock;      	/* # block output operations */
	uint64_t lwp_ioch; 		/* # of characters read/written */
	uint64_t lwp_msgsnd; 		/* # messages sent */
	uint64_t lwp_msgrcv;       	/* # messages received */
	uint64_t lwp_nsignals;		/* # signals received */
	uint64_t lwp_nvcsw;        /* # voluntary context switches */
	uint64_t lwp_nivcsw;       /* # involuntary context switches */
	uint64_t lwp_syscall;		/* # syscalls */
	int64_t lwp_syscall_code;	/* last syscall code */
	int64_t lwp_utime;	/* user time spent executing (in secs) */
	int64_t lwp_stime;	/* system time spent executing (in secs) */
	struct lwpcycles
	  lwp_usercycles;	/* 64-bit user mode execution cycle count */
	struct lwpcycles
	  lwp_systemcycles;	/* 64-bit system mode execution cycle count */
	struct lwpcycles
	  lwp_interruptcycles;	/* 64-bit interrupt for thread cycle count */
	int64_t lwp_valid;	/* valid vector */
	int64_t lwp_fss;	/* fair share scheduler group id */
	int64_t lwp_bind_flags; /* processor/ldom binding flag */
	int64_t lwp_bind_spu;   /* processor bound to */
	int64_t lwp_bind_ldom;  /* ldom bound to */
	int64_t lwp_reserved1;  /* reserved  - do not use */
	int64_t lwp_reserved2;  /* reserved  - do not use */
	int64_t lwp_reserved3;  /* reserved  - do not use */
        int64_t lwp_pset_id;    /* pset id to which this thread is assigned */
        float   lwp_pctcpu;     /* %cpu usage(smoothed) */
	int64_t lwp_resident;   /* seconds memory resident or deactivated */
};



/*
 * LWP flags for lwp_bind_flags
 */
#define LWP_BIND_NONE           0
#define LWP_BIND_SPU            1
#define LWP_BIND_SPU_FORCED     2
#define LWP_BIND_LDOM           3

#define LWP_ANY_SPU             -1
#define LWP_ANY_LDOM            -2
/*
 * LWP flag bits for lwp_flag
 */
#define LWP_OMASK          0x1	   /* obsolete */
#define LWP_OWEUPC         0x2	   /* obsolete */
#define LWP_SEL            0x4	   /* Selecting */
#define LWP_FIRSTLWP	   0x8	   /* Initial lwp in process */
#define LWP_DEACT          0x10	   /* lwp is deactivated */
#define LWP_DEACTSELF      0x20	   /* lwp is to self deactivate */
#define LWP_FAULTING       0x40	   /* obsolete */
#define LWP_ANYPAGE        0x80	   /* obsolete */
#define LWP_LPT            0x100   /* obsolete */
#define LWP_RUNPROC        0x200   /* Running on a processor */
#define LWP_MPLOCK         0x400   /* obsolete */
#define LWP_MP_STOP        0x800   /* obsolete */
#define LWP_MP_SEMA_BLOCK  0x1000  /* obsolete */
#define LWP_MP_SEMA_NOSWAP 0x2000  /* obsolete */
#define LWP_SIGABL         0x4000  /* Signalable lwp */
#define LWP_ACTIVE         0x8000  /* lwp active? */
#define LWP_HALTED         0x10000 /* lwp halted at clean point? */
#define LWP_AST_NULL       0x20000 /* obsolete */
#define LWP_AST_HALT       0x40000 /* obsolete */
#define LWP_AST_TERMINATE  0x80000 /* obsolete */
#define LWP_SA                  0x100000   /* A scheduler activation */
#define LWP_SA_CACHED           0x200000   /* A cached scheduler activation  */
#define LWP_KERN_HELPER_DAEMON  0x400000   /* kernel daemon for MxN process */
#define LWP_USER_HELPER_DAEMON  0x800000   /* MxN helper thread */
#define LWP_USER_KERNEL_DAEMON  0x1000000  /* Kernel daemon */
#define LWP_GANGED              0x2000000  /* Thread is in a gang */

/* Sleep types for lwp_sleep_type */
#define LWP_WAKEUP_ALL		0x1
#define LWP_WAKEUP_ONE		0x2
#define LWP_WAKEUP_OTHER        0x100


