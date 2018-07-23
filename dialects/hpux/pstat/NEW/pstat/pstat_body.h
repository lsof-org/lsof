/* pstat_body.h : helper structure body declarations */


/*   @(#)B11.23_LR   */
/* NO MULTIPLE-INCLUDE PROTECTION (intentional) */

/*
 * Constants
 *	PST_SIGWORDS
 *
 * Size-invariant types
 *  Defines:
 *	pstsigset
 *
 * Size-variant types
 *  Defines:
 *	__psdev
 *	__psttime
 *	__psfsid
 *	__psfileid
 *
 *  Uses:
 *	_T_ULONG_T
 *	_T_LONG_T
 *
 */



/* psdev support */

/*
 * An abstraction of major/minor numbers of a dev
 */
struct __psdev {
	_T_LONG_T psd_major;
	_T_LONG_T psd_minor;
};


/* psttime support */

/*
 * An abstraction of a time
 */
struct __psttime {
	_T_LONG_T pst_sec;	/* seconds */
	_T_LONG_T pst_usec;	/* micro seconds */
};


/* pstcycles support */

/*
 * An abstraction of a 64 bit cycle count
 */
struct __pstcycles {
	_T_ULONG_T psc_hi;	/* most significant bits */
	_T_ULONG_T psc_lo;	/* least significant bits */
};


/* pstsigset support */

#if !defined(_PST_SIGSET_INVARIANT)
#define _PST_SIGSET_INVARIANT

#define PST_SIGWORDS 8

struct pstsigset {
	int32_t pst_sigset[PST_SIGWORDS];
};

#endif /* !defined(_PST_SIGSET_INVARIANT) */



/* psfsid support */

/*
 * An abstraction of a unique identification for a file system
 */
struct __psfsid {
	_T_LONG_T psfs_id;	/* Filesystem ID */
	_T_LONG_T psfs_type;	/* Filesystem type */
};


/* psfileid support */

/*
 * An abstraction of a unique identification for a file
 */
struct __psfileid {
	struct __psfsid	psf_fsid;	/* Filesystem identification */
	_T_LONG_T	psf_fileid;	/* File identification within FS */
	_T_LONG_T	psf_spare;	/* Reserved for future expansion */
};
#ifndef _PSFILEID_T
#define _PSFILEID_T
#endif

