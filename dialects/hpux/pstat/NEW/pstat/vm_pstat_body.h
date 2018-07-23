/*
 * BEGIN_DESC
 *
 *  File:
 *      @(#)B11.23_LR    common/pstat/vm_pstat_body.h
 *
 *  Purpose:
 *      pst_vminfo body declarations and user constants
 *
 *  Classification:                     Release to Release Consistency Req:
 *      public                                  binary & source
 *
 *  BE header:  yes
 *
 *  Shipped:  yes
 *      /usr/include/pstat/vm_pstat_body.h
 *
 * END_DESC
 */

/* NO MULTIPLE-INCLUDE PROTECTION (intentional) */

/*
 * Constants
 *	PST_N_PG_SIZES
 *	PST_SZ_DEFAULT
 *	PS_MEMORY_LOCKED, PS_EXECUTABLE, PS_SHARED, PS_SHARED_LIBRARY
 *	PS_NOTUSED, PS_USER_AREA, PS_TEXT, PS_DATA, PS_STACK, PS_SHARED_MEMORY
 *	PS_NULLDEREF, PS_IO, PS_MMF, PS_GRAPHICS, PS_GRAPHICS_DMA
 *	PS_PROT_READ, PS_PROT_WRITE, PS_PROT_EXECUTE
 *	PST_VADDR
 *	pss_dev, pss_start, pss_nblks, pss_nblksavail, pss_allocated, pss_min
 *	pss_nblksenabled, Pss_nblksenabled, pss_limit, pss_reserve, pss_mntpt
 *	pss_major, pss_minor
 *	SW_ENABLED
 *	SW_BLOCK
 *	SW_FS
 *
 * Size-invariant types
 *  Defines:
 *	pss_reserved;
 *
 * Size-variant types
 *  Defines:
 *	__pst_vminfo
 *	__pst_vm_status
 *	__pst_swapinfo
 *	__pss_blk
 *	__pss_fs
 *
 *  Uses:
 *	__psfileid
 *	__psdev
 *	_T_ULONG_T
 *	_T_LONG_T
 *
 */

#if !defined(_PST_VMINFO_CONSTANTS_DEFINED)
#define _PST_VMINFO_CONSTANTS_DEFINED

/*
 * Indicates default page size selection heuristic is being used.
 */
#define PST_SZ_DEFAULT 0

#endif /* !defined(_PST_VMINFO_CONSTANTS_DEFINED) */

struct __pst_vminfo {
	_T_LONG_T	psv_rdfree;	/* rate: cpages freed by daemon */
	_T_LONG_T	psv_rintr;	/* device interrupts */
	_T_LONG_T	psv_rpgpgin;	/* pages paged in */
	_T_LONG_T	psv_rpgpgout;	/* pages paged out */
	_T_LONG_T	psv_rpgrec;	/* total page reclaims */
	_T_LONG_T	psv_rpgtlb;	/* tlb flushes - 800 only */
	_T_LONG_T	psv_rscan;	/* scans in pageout daemon */
	_T_LONG_T	psv_rswtch;	/* context switches */
	_T_LONG_T	psv_rsyscall;	/* calls to syscall() */
	_T_LONG_T	psv_rxifrec;	/* found in freelist rather than in 
					 * filesys 
					 */
	_T_LONG_T	psv_rxsfrec;	/* found in freelist rather than on 
					 * swapdev 
					 */
	_T_LONG_T	psv_cfree;	/* cnt: free memory pages */
	_T_LONG_T	psv_sswpin;	/* sum:	swapins */
	_T_LONG_T	psv_sswpout;	/* swapouts */
	_T_LONG_T	psv_sdfree;	/* pages freed by daemon */
	_T_LONG_T	psv_sexfod;	/* pages filled on demand from 
					 * executables 
					 */
	_T_LONG_T	psv_sfaults;	/* total faults taken */
	_T_LONG_T	psv_sintr;	/* device interrupts */
	_T_LONG_T	psv_sintrans;	/* intransit blocking page faults */
	_T_LONG_T	psv_snexfod;	/* number of exfod's created */
	_T_LONG_T	psv_snzfod;	/* number of zero filled on demand */
	_T_LONG_T	psv_spgfrec;	/* page reclaims from free list */
	_T_LONG_T	psv_spgin;	/* pageins */
	_T_LONG_T	psv_spgout;	/* pageouts */
	_T_LONG_T	psv_spgpgin;	/* pages paged in */
	_T_LONG_T	psv_spgpgout;	/* pages paged out */
	_T_LONG_T	psv_spswpin;	/* pages swapped in */
	_T_LONG_T	psv_spswpout;	/* pages swapped out */
	_T_LONG_T	psv_srev;	/* revolutions of the hand */
	_T_LONG_T	psv_sseqfree;	/* pages taken from sequential 
					 * programs 
					 */
	_T_LONG_T	psv_sswtch;	/* context switches */
	_T_LONG_T	psv_ssyscall;	/* calls to syscall() */
	_T_LONG_T	psv_strap;	/* calls to trap */
	_T_LONG_T	psv_sxifrec;	/* found in free list rather than in 
					 * filesys 
					 */
	_T_LONG_T	psv_sxsfrec;	/* found on free list rather than on 
					 * swapdev
					 */
	_T_LONG_T	psv_szfod;	/* pages zero filled on demand */
	_T_LONG_T	psv_sscan;	/* scans in pageout daemon */
	_T_LONG_T	psv_spgrec;	/* total page reclaims */
	_T_LONG_T	psv_deficit;	/* estimate of needs of new swapped-in
					 * procs 
					 */
	_T_LONG_T	psv_tknin;	/* number of characters read from 
					 * ttys 
					 */
	_T_LONG_T	psv_tknout;	/* number of characters written to 
					 * ttys 
					 */
	_T_LONG_T	psv_cntfork;	/* number of forks */
	_T_LONG_T	psv_sizfork;	/* number of pages forked */
	_T_ULONG_T	psv_lreads;	/* number of disk blk reads issued */
	_T_ULONG_T	psv_lwrites;	/* number of disk blk writes issued */
	_T_ULONG_T	psv_swpocc;	/* # of times swrq occ'd since boot */
	_T_ULONG_T	psv_swpque;	/* cumulative len of swrq since boot */
	_T_LONG_T	psv_paging_thold;/* paging threshold, moves between
					  * pst_desfree & pst_lotsfree 
					  */
	_T_LONG_T	psv_sysmem;	 /* pages of memory unavailable for 
					  * in-memory backing store 
					  */
	_T_LONG_T	psv_swapspc_cnt; /* pages of on-disk backing store */
	_T_LONG_T	psv_swapspc_max; /* max pages of on-disk backing 
					  * store 
					  */
	_T_LONG_T	psv_swapmem_cnt; /* pages of in-memory backing store */
	_T_LONG_T	psv_swapmem_max; /* max pages of in-memory backing 
					  * store 
					  */
	_T_LONG_T	psv_swapper_mem; /* pages of backing store management 
					  * overhead:- 
					  * psv_swapper_mem + malloc space 
					  * = psv_swapmem_cnt 
					  */
        _T_LONG_T psv_lreadsize;   /* # of char xfer'd by bread */
	_T_LONG_T psv_lwritesize;  /* # of char xfer'd by bwrite */

	_T_LONG_T	psv_swapmem_on;  /* in-memory backing store enabled */
   
	_T_LONG_T psv_select_success[PST_N_PG_SIZES];
				 /* success by page size of LP fault page size
				  * selection. 
				  * 
				  * This field is obsolete.
				  */
	_T_LONG_T psv_select_failure[PST_N_PG_SIZES];
				 /* failure by page size of LP fault page size
				  * selection 
				  *
				  * This field is obsolete.
				  */
	_T_LONG_T psv_pgalloc_success[PST_N_PG_SIZES];
				 /* success by page size of LP allocation 
				  * 
				  * This field is obsolete. 
				  */
	_T_LONG_T psv_pgalloc_failure[PST_N_PG_SIZES];
				 /* failure by page size of LP allocation 
				  * 
				  * This field is obsolete.
				  */
	_T_LONG_T psv_demotions[PST_N_PG_SIZES];
				 /* LP demotions by page size 
				  * 
				  * This field is obsolete. 
				  */
 	_T_ULONG_T  psv_reserved1;   /* reserved -- do not use */
 	_T_ULONG_T  psv_reserved2;   /* reserved -- do not use */
 	_T_ULONG_T  psv_reserved3;   /* reserved -- do not use */
 	_T_ULONG_T  psv_sbreadcache; /* Total bread cache hits */
 	_T_ULONG_T  psv_sbreada;     /* Total read aheads */
 	_T_ULONG_T  psv_sbreadacache;/* Total read ahead cache hits */
 	_T_ULONG_T  psv_rpswpin;     /* Rate pages swapped in */
 	_T_ULONG_T  psv_rpswpout;    /* Rate pages swapped out */
 	_T_ULONG_T  psv_rpgin;       /* Rate pageins */
 	_T_ULONG_T  psv_rpgout;      /* Rate pageouts */
 	_T_ULONG_T  psv_rfaults;     /* Rate faults taken */
 	_T_ULONG_T  psv_rbread;      /* Rate breads */
 	_T_ULONG_T  psv_rbreadcache; /* Rate bread cache hits */
 	_T_ULONG_T  psv_rbreada;     /* Rate read aheads */
 	_T_ULONG_T  psv_rbreadacache;/* Rate read ahead cache hits */
 	_T_ULONG_T  psv_rswpin;      /* Rate swapins */
 	_T_ULONG_T  psv_rswpout;     /* Rate swapouts */
	_T_LONG_T   psv_kern_dynmem; /*
				      * # of pages allocated for kernel
				      * dynamic memory
				      */
	_T_LONG_T psv_select_success_first[PST_N_PG_SIZES_FIRST];
                	/* success by page size of LP fault page size
			 * selection 
			 */
	_T_LONG_T psv_select_failure_first[PST_N_PG_SIZES_FIRST];
	                /* failure by page size of LP fault page size
			 * selection 
			 */
	_T_LONG_T psv_pgalloc_success_first[PST_N_PG_SIZES_FIRST];
	                /* success by page size of LP allocation */
	_T_LONG_T psv_pgalloc_failure_first[PST_N_PG_SIZES_FIRST];
	                /* failure by page size of LP allocation */
	_T_LONG_T psv_demotions_first[PST_N_PG_SIZES_FIRST];
	                /* LP demotions by page size */
};

/* pst_vm_status support */

/*
 * Contains process address space information.  See pstat_procvm().
 */
struct __pst_vm_status {
	_T_LONG_T pst_space;        /* virtual space for this object */
	_T_LONG_T pst_vaddr64bit_pad;
	_T_ULONG_T pst_vaddr;       /* virtual offset for this object */
	_T_LONG_T pst_length;       /* number of pages mapped by this object */
	_T_LONG_T pst_phys_pages;   /* number of incore pages for this object*/
	_T_LONG_T pst_flags;        /* disposition of this object */
	_T_LONG_T pst_type;         /* type of data managed by this object */
	_T_LONG_T pst_permission;   /* access permissions to this object */
	struct __psfileid pst_id;
	int32_t pst_valid;          /* 
				     * This is a vector that will indicate if a
			             * certain field is valid 
				     */	
	_T_LONG_T pst_pagesize_hint;/*
				     * largepage hint for determining the
				     * appropriate pagesize when allocating
				     * a page.
				     */
	_T_LONG_T pst_vps_pgsizes[PST_N_PG_SIZES];
	                            /*
				     * This array lists the number of pages the
				     * region has of each pagesize.
				     *
				     * This field is obsolete.
				     */
 	struct __pst_fid  pst_fid;  /* 
				     * A unique id to efficiently
                                     * reaccess the file corresponding
                                     * to the memory region
				     */
	_T_ULONG_T  pst_refcnt;     /* # of processes sharing the region */
 	_T_ULONG_T  pst_incore;     /* 
				     * # of non-swapped processes sharing
                                     * the region 
				     */
 	_T_LONG_T  pst_lockmem;     /* # of pages locked in memory  */
	uint32_t pst_hi_fileid;     /* Per shared file ID */
	uint32_t pst_lo_fileid;
	uint32_t pst_hi_nodeid;     /* Per vnodet ID*/
	uint32_t pst_lo_nodeid;
	uint32_t padding;	    /* 
				     * This is required to make the user and 
				     * kernel sizes the same 
				     */
	_T_LONG_T pst_swap;         /* # of swap pages allocated */

	_T_LONG_T pst_vps_pgsizes_first[PST_N_PG_SIZES_FIRST];
	                            /*
				     * This array lists the number of pages the
				     * region has of each pagesize.
				     */
};

#if !defined(_PST_VM_STATUS_CONSTANTS_DEFINED)
#define _PST_VM_STATUS_CONSTANTS_DEFINED

/*
 * Flag bits for pst_vm_status.pst_flags.
 */
#define PS_MEMORY_LOCKED	0x1
#define PS_EXECUTABLE		0x2
#define PS_SHARED		0x4	/* Shared object. */
#define PS_SHARED_LIBRARY	0x8	/* Shared library. */

/*
 * Type values for pst_vm_status.pst_type.
 */
#define PS_NOTUSED		0
#define PS_USER_AREA		1
#define PS_TEXT			2
#define PS_DATA			3
#define PS_STACK		4
#define PS_SHARED_MEMORY	5
#define PS_NULLDEREF		6
#define PS_IO			7
#define PS_MMF			8
#define PS_GRAPHICS		9
#define PS_GRAPHICS_DMA         10
#define PS_RSESTACK		11

#define PS_PROT_READ            0x1
#define PS_PROT_WRITE           0x2
#define PS_PROT_EXECUTE         0x4

/* 
 * Macros for field validity check in pst_vm_status 
 */
#define PST_VADDR       0x1

#endif /* !defined(_PST_VM_STATUS_CONSTANTS_DEFINED) */

/* pst_swapinfo support */

#if !defined(_PST_SWAPINFO_INVARIANT_STRUCTS)
#define _PST_SWAPINFO_INVARIANT_STRUCTS

struct pss_reserved {
	char		Pss_XXX[512];	/* reserved for pss_fs/pss_blk
					   expansion. */
};

#endif /* !defined(_PST_SWAPINFO_INVARIANT_STRUCTS */

struct __pss_blk {
	struct __psdev	Pss_dev;	/* Device specification */
	_T_ULONG_T	Pss_start;	/* For 300,700: starting blk */
	_T_ULONG_T	Pss_nblksenabled; /* # of blocks enabled */
	_T_ULONG_T	Pss_nblksavail;	/* # of blocks available */
};

struct __pss_fs {
	_T_ULONG_T	Pss_allocated;	/* # of blocks curr. avail. */
	_T_ULONG_T	Pss_min;	/* min # of blocks to alloc. */
	_T_ULONG_T	Pss_limit;	/* max # of blocks to alloc. */
	_T_ULONG_T	Pss_reserve;	/* # of blocks to reserve */
	char		Pss_mntpt[256];	/* FS mount point path */
};


/*
 * This structure describes per-swap-area information.
 * Each structure returned describes one "pool" of swap space on the system,
 * either a block device or a portion of a filesystem.
 */
struct __pst_swapinfo {
	_T_ULONG_T	pss_idx;	/* Idx for further pstat() requests */
	_T_ULONG_T	pss_flags;	/* flags associated with swap pool */
	_T_ULONG_T	pss_priority;	/* priority of the swap pool */
	_T_ULONG_T	pss_nfpgs;	/* # of free pages of space in pool */
	union {				/* block and fs swap differ */
		struct __pss_blk Pss_blk;	/* Block device Fields */
		struct __pss_fs  Pss_fs;	/* File System Fields */
		struct pss_reserved Pss_XX; /* reserved for union expansion */
	} pss_un;
	_T_ULONG_T	pss_swapchunk;	/* block size */
};

#if !defined(_PST_SWAPINFO_CONSTANTS_DEFINED)
#define _PST_SWAPINFO_CONSTANTS_DEFINED
#define pss_dev		pss_un.Pss_blk.Pss_dev
#define pss_start	pss_un.Pss_blk.Pss_start
#define pss_nblksavail	pss_un.Pss_blk.Pss_nblksavail
#define pss_nblksenabled pss_un.Pss_blk.Pss_nblksenabled
/*
 * The following pss_nblks and Pss_nblks macros are considered
 * obsolete.  They only provide source compatibility for the
 * old pre 10.10 field name (which was "total # of blocks").
 */
#define pss_nblks	pss_un.Pss_blk.Pss_nblksenabled /* obsolete */
#define Pss_nblks	Pss_nblksenabled	/* obsolete: old total # blks */

#define pss_allocated	pss_un.Pss_fs.Pss_allocated
#define pss_min		pss_un.Pss_fs.Pss_min
#define pss_limit	pss_un.Pss_fs.Pss_limit
#define pss_reserve	pss_un.Pss_fs.Pss_reserve
#define pss_mntpt	pss_un.Pss_fs.Pss_mntpt

#define pss_major	pss_dev.psd_major
#define pss_minor	pss_dev.psd_minor

/*
 * Swap info for pss_flags
 */
#define SW_ENABLED	0x1
#define SW_BLOCK	0x2
#define SW_FS		0x4
#endif /* !defined(_PST_SWAPINFO_CONSTANTS_DEFINED) */

#if defined(_PSTAT64)
/*
 * The structures pst_locality and pst_proc_locality are returned by
 * the pstat system calls pstat_getlocality and pstat_getproclocality.
 * These calls return information on different parts of a ccNUMA
 * (Cache Coherent Non-Uniform Memory Architecture) system.
 *
 * On an UMA system, one structure will be returned, and it will
 * represent the entire system.  On a ccNUMA system, many structures
 * will be returned and each will represent one "UMA-like" building
 * block of the machine.
 *
 * These building blocks are nearly identical to the concept of the
 * "locality domain" as described in the mpctl(2) man page.  From that
 * man page:
 *
 *	A locality domain consists of a related collection of
 *	processors, memory, and peripheral resources that comprise a
 *	fundamental building block of the system. All processors and
 *	peripheral devices in a given locality domain have equal
 *	latency to the memory contained within that locality domain.
 *
 * There is only one difference, and that is the concept of
 * interleaved memory.  Interleaved memory is a region of physical
 * memory that is created from the memory of several locality domains.
 * This memory is striped together with a very fine granularity.  As 
 * an example, consider a system with four locality domains 0, 1, 2,
 * and 3.  Let's say they all contribute 1 GB of memory to the 
 * interleave.  The interleaved memory would look like this:
 *
 * Memory Address	Comes From
 * --------------	----------
 *   0 -  63 (bytes)	ldom 0
 *  64 - 127		ldom 1
 * 128 - 191		ldom 2
 * 192 - 255		ldom 3
 * 256 - 319		ldom 0
 *   etc, up to 4 GB.
 * 
 * Interleaved memory is a good place to put shared objects, the kernel, 
 * and objects that could be accessed from any part of the system.  There
 * will be at most one interleaved locality.  Some systems may not have
 * interleaved memory.
 * 
 * Given the 4 locality domain system above, these pstat calls would
 * return 5 localities - one for each ldom, and one for interleaved
 * memory.  The reason that mpctl(2) does not count interleaved memory
 * as an LDOM is because mpctl(2) is used for scheduling purposes, and
 * interleaved memory has no cpus to itself!
 * 
 *
 * 
 */

#ifndef _PST_LOCALITY_FLAGS_DEFINED
#define _PST_LOCALITY_FLAGS_DEFINED
/*
 * This enum is for one locality.  A "FLOATING" locality does not
 * contribute any memory to interleaved memory, and is therefore
 * entirely made up of local memory.  This locality may be one that
 * was added as the result of an On-Line-Addition (OLA) event, or it
 * might be set aside as a possible target of an On-Line-Deletion
 * (OLD) event.  Obviously, both PSL_INTERLEAVED and PSL_FLOATING 
 * cannot be set at the same time.
 */
typedef enum pst_locality_flags {
	PSL_INTERLEAVED = 0x1, /* physical memory is interleaved (see above) */
	PSL_LOCAL =       0x2, /* physical memory maps entirely to one LDOM */
	PSL_FLOATING =    0x4  /* This LDOM does not contribute to interleave*/
} pst_locality_flags_t;
#endif /* _PST_LOCALITY_FLAGS_DEFINED */

/*
 * See above for a discussion of the definition of "locality".  This
 * structure represents one of them, and returns system-wide
 * information.
 *
 * If this locality maps to an LDOM, psl_ldom_id will be equal to that
 * LDOM, and PSL_LOCAL will be set in psl_flags.  If the locality
 * represents interleaved memory, psl_ldom_id will be set to -1, and
 * PSL_INTERLEAVED will be set in flags.
 *
 * On an UMA system, there will be one locality, PSL_LOCAL will be set
 * in psl_flags, and psl_ldom_id will be zero.  
 */
struct pst_locality {
	pst_locality_flags_t psl_flags;	/* See above */
	int64_t  psl_ldom_id;		/* LDOM id from mpctl() */
	int64_t  psl_physical_id;	/* Arch. dependent id */
	uint64_t psl_total_pages;	/* total physical pages */
	uint64_t psl_free_pages;	/* free physical pages */
	uint64_t psl_cpus;		/* number of enabled processors */
};

/*
 * This structure returns per-locality information for one process.
 * As an example, ppl_rss_private below will return the number of
 * private physical pages that the examined process has in one
 * particular locality.  
 */
struct pst_proc_locality {
	int64_t  ppl_ldom_id;		/* LDOM id from mpctl() */
	uint64_t ppl_rss_total;		/* private and shared */
	uint64_t ppl_rss_shared;	/* shared only */
	uint64_t ppl_rss_private;	/* private only */
	uint64_t ppl_rss_weighted;	/* private and (shared/sharers) */
};

#endif /* defined(_PSTAT64) */
