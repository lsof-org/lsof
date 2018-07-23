/*
 * BEGIN_DESC
 *
 *  File:
 *      @(#)B11.23_LR    common/include/sys/pstat/pipc_pstat_body.h
 *
 *  Purpose:
 *      __pst_pipc  body declarations and user constants
 *
 *  Classification:                     Release to Release Consistency Req:
 *      public                                  binary & source
 *
 *  BE header:  yes
 *
 *  Shipped:  yes
 *      /usr/include/pstat/pipc_pstat_body.h
 *
 * END_DESC
 */

/* NO MULTIPLE-INCLUDE PROTECTION */

/*
 * Constants
 *	PST_UNLINKED
 *	PST_PSEMINFO_NAMELEN
 *	PST_PMQINFO_NAMELEN
 *	PPMQ_CURMSG, PPMQ_MAXMSG, PPMQ_MAXSIZE
 *
 * Size-invariant types
 *	none
 *
 * Size-variant types
 *  Defines:
 *	__pst_pseminfo
 *	__pst_mqinfo 
 *
 *  Uses:
 *	none
 *
 */

/* pst_pipcinfo support */

#define PST_UNLINKED "<unlinked>"	/* Identifies unlinked entries */

/* pst_pseminfo support */

#define PST_PSEMINFO_NAMELEN 1024       /* Same value as MAXPATHLEN */

/*
 * This structure describes per-(PosixV semaphore set) information.
 * Each structure returned describes a semaphore identifier on the system.
 */

struct __pst_pseminfo {
	_T_ULONG_T	ppsem_idx;	/* Idx for further pstat() requests */
	_T_LONG_T	ppsem_euid;	/* effective user id */
	_T_LONG_T	ppsem_egid;	/* effective group id */
	_T_LONG_T	ppsem_perm;	/* access permission */
	_T_LONG_T	ppsem_opencnt;	/* # of procs referencing this sem */
	_T_ULONG_T	ppsem_initial_value;	/* initial sema value */
	_T_LONG_T	ppsem_value;	/* sema value */
	char		ppsem_name[PST_PSEMINFO_NAMELEN]; /* sem name */
};

/* pst_mqinfo support */

#define PST_PMQINFO_NAMELEN 1024        /* Same value as MAXPATHLEN */

/*
 * This structure describes per-(Posix message queue) information.
 * Each structure returned describes a message queue identifier on the system.
 */

struct __pst_pmqinfo {
	_T_ULONG_T	ppmq_idx;	/* Idx for further pstat() requests */
	_T_LONG_T	ppmq_euid;	/* effective user id */
	_T_LONG_T	ppmq_egid;	/* effective group id */
	_T_LONG_T	ppmq_perm;	/* access permission */
	_T_LONG_T	ppmq_opencnt;	/* # of procs referencing this MQ */
	_T_LONG_T	ppmq_curmsg;	/* current # msgs in this MQ */
	_T_LONG_T	ppmq_maxmsg;    /* max # of messages for this MQ */
	_T_LONG_T	ppmq_msgsize;	/* max message size for this MQ */
	char		ppmq_name[PST_PMQINFO_NAMELEN]; /* MQ name */
};

/* Macros for field validity check in pst_pmqinfo */
#define PPMQ_CURMSG    0x1
#define PPMQ_MAXMSG    0x1
#define PPMQ_MAXSIZE   0x1
