/*
 *  BEGIN_DESC
 *
 *  File:
 *      @(#)B11.23_LR    common/pstat/fid_pstat_body_pub.h
 *
 *  Purpose:
 *     To provide a unique id to efficieently reaccess opened files for pstat
 *     use.
 *
 *  Classification:                     Release to Release Consistency Req:
 *      public                                  binary & source
 *
 *  BE header:  yes
 *
 *  Shipped:  yes
 *      /usr/include/sys/pstat/fid_pstat_body_pub.h
 *
 *  Constants
 *      PS_FID_SZ
 *
 *  Size-invariant types
 *  Defines:
 *	pst_fid_user
 *
 *  Size-variant types
 *  Defines:
 *
 *  Uses:
 *
 * END_DESC
 */

#ifndef _SYS_PSTAT_FID_PSTAT_BODY_INCLUDED
#define _SYS_PSTAT_FID_PSTAT_BODY_INCLUDED

#define PS_FID_SZ 64

/* 
 * fid is used to efficiently reaccess the opened files.  This
 * value is returned by pstat_getfile(), pstat_getproc(), and
 * pstat_getprocvm() calls. This ID is then passed to subsequent PSTAT
 * calls such as pstat_getsocket() to efficiently reaccess the opened files.
 * (Please see man pages for examples.)
 */
struct pst_fid_user {
	double          value[PS_FID_SZ / sizeof(double)];
};

#endif /* _SYS_PSTAT_FID_PSTAT_BODY_INCLUDED */
