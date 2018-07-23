/*
 * BEGIN_DESC
 *
 *  File:
 *      @(#)B11.23_LR    common/pstat/rpath_pstat_body.h
 *
 *  Purpose:
 * 	pst_mpathnode body declarations and user constants
 *
 *  Classification:                     Release to Release Consistency Req:
 *      public                                  binary & source
 *
 *  BE header:  yes
 *
 *  Shipped:  yes
 *      /usr/include/pstat/rpath_pstat_body.h
 *
 * END_DESC
 */

/*
 * Constants
 * PS_SEGMENTNAME_SZ, PS_DIR_DEPTH, PS_PATHNAME_SZ
 *
 * Size-invariant types
 *  Defines:
 *      pst_nameent
 *
 * Size-variant types
 *  Defines:
 *      __pst_pathname_args
 *	__pst_mpathnode
 *	__pst_mpathname_args
 *
 *  Uses:
 *	_T_ULONG_T
 *	__psfileid
 *
 */

#if !defined(_PST_RPATH_CONSTANTS_DEFINED)
#define _PST_RPATH_CONSTANTS_DEFINED

#define PS_SEGMENTNAME_SZ 64  /* segment name size */

#endif /* !defined(_PST_RPATH_CONSTANTS_DEFINED) */

/* NO MULTIPLE-INCLUDE PROTECTION (intentional) */
struct __pst_pathname_args {
     _T_PTR_T  buf;
     _T_PTR_T  id;
};


struct __pst_mpathname_args {
	_T_PTR_T buf; /* pointer to user buffer */
	_T_PTR_T id;  /* pointer to psfsid */
};

struct __pst_mpathnode {
     _T_ULONG_T psr_idx; 	/* 
				 * Current index of the entry on the chain of 
                                 * DNLC entries 
				 */      
     struct __psfileid psr_file;     /* the file this entry describes */
     struct __psfileid psr_parent;   /* parent of this file */
     char       psr_name[PS_SEGMENTNAME_SZ];  /* NULL terminated string */
};
