/*
 * BEGIN_DESC
 *
 *  File:
 *      @(#)B11.23_LR    common/pstat/global_pstat_body.h
 *
 *  Purpose:
 *  	pst_static, pst_dynamic, pst_processor body declarations and user 
 *	constants
 *
 *  Classification:                     Release to Release Consistency Req:
 *      public                                  binary & source
 *
 *  BE header:  yes
 *
 *  Shipped:  yes
 *      /usr/include/pstat/global_pstat_body.h
 *
 * END_DESC
 */

/* NO MULTIPLE-INCLUDE PROTECTION (intentional) */

/*
 * Constants
 *	PST_N_PG_SIZES
 *	PST_MAX_CPUSTATES
 *	PST_MAX_PROCS
 *	PSD_VM, PSD_AVM
 *	PS_PA83_FPU, PS_PA89_FPU
 *
 * Size-invariant types
 *  Defines:
 *	none
 *
 * Size-variant types
 *  Defines:
 *	__pst_static
 *	__pst_dynamic
 *	__pst_processor
 *	__pscoproc
 *
 *  Uses:
 *	__pstcycles
 *	__psdev
 *	_T_ULONG_T
 *	_T_LONG_T
 *
 */

/*
 * This structure contains global system information.  This data may change
 * while the system is running due to administrative changes in the associated
 * kernel tunables.
 */
struct __pst_static {
	_T_LONG_T	max_proc;
	struct __psdev	console_device;
	_T_LONG_T	boot_time;
	_T_LONG_T	physical_memory;
	_T_LONG_T	page_size;
	_T_LONG_T	cpu_states;
	_T_LONG_T	pst_status_size;
	_T_LONG_T	pst_static_size;
	_T_LONG_T	pst_dynamic_size;
	_T_LONG_T	pst_vminfo_size;
	_T_LONG_T	command_length;
	_T_LONG_T	pst_processor_size;
	_T_LONG_T	pst_diskinfo_size;
	_T_LONG_T	pst_lvinfo_size;
	_T_LONG_T	pst_swapinfo_size;
	_T_LONG_T	pst_maxmem;
	_T_LONG_T	pst_lotsfree;
	_T_LONG_T	pst_desfree;
	_T_LONG_T	pst_minfree;
	_T_LONG_T	pst_max_ninode;
	_T_LONG_T	pst_max_nfile;
	_T_LONG_T	pst_stable_size;
	_T_LONG_T	pst_supported_pgsize[PST_N_PG_SIZES]; /* obsolete */
	_T_LONG_T	pst_fileinfo_size;
	_T_LONG_T	pst_fileinfo2_size;
	_T_LONG_T       pst_filedetails_size;
	_T_LONG_T       pst_socket_size;
	_T_LONG_T       pst_stream_size;
	_T_LONG_T       pst_mpathnode_size;
	_T_LONG_T       pst_ipcinfo_size;
	_T_LONG_T       pst_msginfo_size;
	_T_LONG_T       pst_seminfo_size;
	_T_LONG_T       pst_shminfo_size;
	_T_LONG_T       pst_vm_status_size;
	_T_LONG_T       lwp_status_size;
	_T_LONG_T       pst_crashinfo_size;
	_T_LONG_T       pst_crashdev_size;
	_T_LONG_T       pst_node_size;
	_T_LONG_T       clonemajor;
        _T_LONG_T       pst_pset_size;
	_T_LONG_T       pst_pmqinfo_size;
        _T_LONG_T       pst_pseminfo_size;
        _T_LONG_T       pst_locality_size;
        _T_LONG_T       pst_proc_locality_size;
	_T_LONG_T       pst_supported_pgsize_first[PST_N_PG_SIZES_FIRST];
};


/* pst_dynamic support */

#if !defined(_PST_DYNAMIC_CONSTANTS_DEFINED)
#define _PST_DYNAMIC_CONSTANTS_DEFINED

/* Max number of cpu states
 * that we can return info about.
 */
#define PST_MAX_CPUSTATES 15

/* Max number of processors in
 * system.  Used to define sizes
 * of per-processor structures.
 */
#define PST_MAX_PROCS 32

/* 
 * Macros for field validity check for struct pst_dynamic
 */
#define PSD_VM       0x1
#define PSD_AVM      0x2

#endif /* !defined(_PST_DYNAMIC_CONSTANTS_DEFINED) */

/*
 * This structure contains dynamic system variables, ones which may
 * change frequently during normal operation of the kernel.
 *
 * The following fields are obsolete and provided only for backward
 * compatibility:
 *
 *	psd_mp_avg_1_min[]
 *	psd_mp_avg_5_min[]
 *	psd_mp_avg_15_min[]
 *	psd_mp_cpu_time[][]
 *
 * Since the size of these arrays is hard-wired, there is no way to
 * get information about more than a fixed number of processors with
 * the pstat_getdynamic() call.  Instead, pstat_getprocessor() should
 * be used.  It returns the same information (see below) and supports
 * any number of processors.
 */
struct __pst_dynamic {
	_T_LONG_T	psd_proc_cnt;	/* MP: number of active processors */
	_T_LONG_T	psd_max_proc_cnt; /* MP: max active processors */
	_T_LONG_T	psd_last_pid;	/* last run process ID */
	_T_LONG_T	psd_rq;		/* run queue length */	
	_T_LONG_T	psd_dw;		/* jobs in disk wait */
	_T_LONG_T	psd_pw;		/* jobs in page wait */
	_T_LONG_T	psd_sl;		/* jobs sleeping in core */
	_T_LONG_T	psd_sw;		/* swapped out runnable jobs */

	/*
	 * The following 8 elements are sums across all non-system
	 * processes.  Thus, "total virtual memory" refers to the number of
	 * virtual pages used by all non-system processes.  The word
	 * "active" means that a thread in a process has run in the last 20
	 * seconds.  Thus, "active virtual memory" refers to the sum of the
	 * virtual memory of all processes which have had a thread run in the
	 * last 20 seconds.  "Real memory" refers to pages residing in core.
	 * Shared memory is included in all these totals, but is counted only
	 * once.  Fields that end in "txt" apply to text pages only.
	 */
	_T_LONG_T	psd_vm;		/* total virtual memory */
	_T_LONG_T	psd_avm;	/* active virtual memory */
	_T_LONG_T	psd_rm;		/* total real memory */
	_T_LONG_T	psd_arm;	/* active real memory */
	_T_LONG_T	psd_vmtxt;	/* virt mem text */
	_T_LONG_T	psd_avmtxt;	/* active virt mem text */
	_T_LONG_T	psd_rmtxt;	/* real mem text */
	_T_LONG_T	psd_armtxt;	/* active real mem text */

	_T_LONG_T	psd_free;	/* free memory pages */
	double		psd_avg_1_min;	/* global run queue lengths */
	double		psd_avg_5_min;
	double		psd_avg_15_min;
					/* global cpu time/state */
	_T_LONG_T	psd_cpu_time[PST_MAX_CPUSTATES];
					/* OBSOLETE: per-processor run
					   queue lengths */
	double		psd_mp_avg_1_min[PST_MAX_PROCS];
	double		psd_mp_avg_5_min[PST_MAX_PROCS];
	double		psd_mp_avg_15_min[PST_MAX_PROCS];
					/* OBSOLETE: per-processor cpu
					   time/state */
	_T_LONG_T	psd_mp_cpu_time[PST_MAX_PROCS][PST_MAX_CPUSTATES];
	_T_LONG_T	psd_openlv;	/* # of open Logical Volumes */
	_T_LONG_T	psd_openvg;	/* # of active LV Volume Groups */
	_T_LONG_T	psd_allocpbuf;	/* # of allocated LV pvol buffer headers */
	_T_LONG_T	psd_usedpbuf;	/* # of LV pvol buffer headers in use */
	_T_LONG_T	psd_maxpbuf;	/* used to be size of pbuf pool,
					 * now always zero
					 */
	_T_LONG_T	psd_activeprocs; /* # of active proc  table entries */
	_T_LONG_T	psd_activeinodes;/* # of active inode table entries */
	_T_LONG_T	psd_activefiles; /* # of active file  table entries */
	_T_LONG_T	psd_mpdcnt;	 /* # of (bad) memory pages deallocated */
	_T_LONG_T	psd_procovf;     /* # of times the proc table overflowed */
	_T_LONG_T	psd_inodeovf;    /* # of times the inode table overflowed */
	_T_LONG_T	psd_fileovf;     /* # of times the file table overflowed */
	_T_LONG_T	psd_global_virtual; /* Available global virt space (pages) */
	int32_t		psd_valid;	 /* This is a vector that will indicate 
					  * if a certain field in is valid */
	_T_LONG_T	psd_monarch_node;
					 /* monarch logical node id      */
	_T_LONG_T	psd_node_cnt;	 /* number of nodes on the system*/
	_T_LONG_T  	psd_dnlc_size;	 /* Size of DNLC */
	_T_ULONG_T      psd_dnlc_hits;   /* Cache hits */
	_T_ULONG_T      psd_dnlc_misses; /* Cache misses */
	_T_ULONG_T      psd_dnlc_long;   /* Long names tried to lookup*/
        _T_LONG_T       psd_num_psets;  /* number of active processor sets in
					   the ysstem */

			/* Current maximum numbers of processes */
        _T_LONG_T       psd_maxprocs;	 

			/* Number of process structures currently allocated */
        _T_LONG_T       psd_numprocsallocd;

			/* Current maximum numbers of kernel threads */
        _T_LONG_T       psd_maxkthreads;

			/* Number of thread structures currently allocated */
        _T_LONG_T       psd_numkthreadsallocd;
	_T_LONG_T	psd_activethreads; /* # of active threads on system */

};



/* pst_processor support */

/*
 * An abstraction of co-processor information for a processor
 */
struct __pscoproc {
	_T_ULONG_T psc_present;	/* mask of coprocessors present */
	_T_ULONG_T psc_enabled;	/* mask of coprocessors enabled */
};

#if !defined(_PSCOPROC_CONSTANTS_DEFINED)
#define _PSCOPROC_CONSTANTS_DEFINED
/*
 * Coprocessor IDs.  The corresponding bit will be set in the fields of
 * a struct pscoproc for a present or enabled co-processor.
 */
#define PS_PA83_FPU	0x1
#define PS_PA89_FPU	0x2
#endif /* !defined(_PSCOPROC_CONSTANTS_DEFINED) */

/*
 * Processor state definition
 */
#    define PSP_SPU_ENABLED          1
#    define PSP_SPU_DISABLED         2
#    define PSP_SPU_INTRANSITION     3
#    define PSP_SPU_HW_FAIL_DISABLED 5

/*
 * Processor interrupt configuration info for pst_flags
 *
 * PSP_INTERRUPT_ENABLED implies that the given processor is or
 * may be configured to receive I/O interrupts.
 *
 * PSP_INTERRUPT_DISABLED implies that the given processor is not and
 * cannot be configured to receive I/O interrupts.
 *
 */
#    define PSP_INTERRUPT_ENABLED    0x1
#    define PSP_INTERRUPT_DISABLED   0x2


/*
 * This structure describes per-processor information.
 * Each structure returned describes one processor on a multi-processor
 * system.  (A total of one structure for uni-processor machines)
 */

struct __pst_processor {
	_T_ULONG_T	psp_idx;	/* Index of the current spu in the
					 * array of processor statistic entries
					 */
	_T_ULONG_T	psp_fsreads;	/* # of reads from filesys blocks. */
	_T_ULONG_T	psp_fswrites;	/* # of writes to filesys blocks. */
	_T_ULONG_T	psp_nfsreads;	/* # of nfs disk blk reads issued. */
	_T_ULONG_T	psp_nfswrites;	/* # of nfs disk blk writes issued. */
	_T_ULONG_T	psp_bnfsread;	/* # of bytes read from NFS. */
	_T_ULONG_T	psp_bnfswrite;	/* # of bytes written to NFS. */
	_T_ULONG_T	psp_phread;	/* # of physical reads to raw devs. */
	_T_ULONG_T	psp_phwrite;	/* # of physical writes to raw devs. */
	_T_ULONG_T	psp_runocc;	/* # of times the processor had
					 * processes waiting to run.  This
					 * running total is updated once
					 * a second.
					 */
	_T_ULONG_T	psp_runque;	/* # of processes the processor had
					 * waiting to run.  This running total
					 * is updated once a second.
					 */
	_T_ULONG_T	psp_sysexec;	/* # of exec system calls. */
	_T_ULONG_T	psp_sysread;	/* # of read system calls. */
	_T_ULONG_T	psp_syswrite;	/* # of write system calls. */
	_T_ULONG_T	psp_sysnami;	/* # of calls to sysnami(). */
	_T_ULONG_T	psp_sysiget;	/* # of calls to sysiget(). */
	_T_ULONG_T	psp_dirblk;	/* # of filesystem blocks read doing
					 * directory lookup.
					 */
	_T_ULONG_T	psp_semacnt;	/* # of System V semaphore ops. */
	_T_ULONG_T	psp_msgcnt;	/* # of System V message ops. */
	_T_ULONG_T	psp_muxincnt;	/* # of MUX interrupts received. */
	_T_ULONG_T	psp_muxoutcnt;	/* # of MUX interrupts sent. */
	_T_ULONG_T	psp_ttyrawcnt;	/* # of raw characters read. */
	_T_ULONG_T	psp_ttycanoncnt; /* # of canonical chars processed. */
	_T_ULONG_T	psp_ttyoutcnt;	/* # of characters output. */
	struct __pscoproc psp_coprocessor; /* info on any co-processors */
	_T_ULONG_T	psp_iticksperclktick; /* interval timer counts (CR16)
					       * per clock tick,
					       * see sysconf(_SC_CLK_TCK)
					       */
	_T_ULONG_T	psp_sysselect;	/* # of select system calls. */
	struct __pstcycles psp_idlecycles;	/* 64-bit idle execution cycle count */
	double		psp_avg_1_min;	/* per-processor run queue lengths */
	double		psp_avg_5_min;
	double		psp_avg_15_min;
					/* per-processor cpu time/state */
	_T_LONG_T	psp_cpu_time[PST_MAX_CPUSTATES];
	_T_ULONG_T	psp_logical_node; /* node the spu is on		*/
        _T_LONG_T       psp_pset_id;      /* processor set id to which this 
					     processor is assigned */
        _T_LONG_T       psp_logical_id;   /* logical id of a processor */
        _T_LONG_T       psp_processor_state; /* state of a processor */
        _T_LONG_T       psp_flags;        /* processor flags */
					/* per-processor cpu time per state
					  in cycles */ 
	struct __pstcycles 
		psp_usercycles; /* 64-bit cpu user mode execution 
					cycle count */
	struct __pstcycles 
		psp_systemcycles; /* 64-bit cpu system mode execution 
					cycle count */
	struct __pstcycles 
		psp_interruptcycles; /* 64-bit cpu interrupt mode execution 
					cycle count */
};
