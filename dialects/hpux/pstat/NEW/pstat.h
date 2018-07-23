/*
 * BEGIN_DESC
 * 
 *  File: 
 *      @(#)B11.23_LR	common/sys/pstat.h
 * 
 *  Purpose:
 *  <<please update with a synopis of the functionality provided by this file>>
 * 
 *  Classification:			Release to Release Consistency Req:
 * 	public					binary & source
 * 
 *  BE header:  yes
 *
 *  Shipped:  yes
 *	/usr/include/sys/pstat.h
 *	/usr/conf/sys/pstat.h
 *
 * END_DESC  
 */

#ifndef _SYS_PSTAT_INCLUDED
#define _SYS_PSTAT_INCLUDED

#ifndef _NO_USER_PROTOS

#include <sys/stdsyms.h>

#ifdef _INCLUDE_HPUX_SOURCE

#include <sys/types.h>


/* PSTAT FUNCTION CODES */


#if defined(_PSTAT64)
#define _PSTAT_INTF _PSTAT64_INTF
#else	/* _PSTAT64 */
#define _PSTAT_INTF _PSTAT32_INTF
#endif /* _PSTAT64 */

#include <sys/pstat/pstat_ops.h>


/* TYPES */

#if defined(_PSTAT64)
# define _T_ULONG_T uint64_t
# define _T_LONG_T int64_t
# define _T_OFF64_T off64_t
# define _PSF_OFFSET64 psf_offset64
#else /* defined(_PSTAT64) */
# define _T_ULONG_T uint32_t
# define _T_LONG_T int32_t
# if defined(__STDC_32_MODE__)
#  define _T_OFF64_T off32_t
#  define _PSF_OFFSET64 psf_dummy[2]
# else /* defined(__STDC_32_MODE__) */
#  define _T_OFF64_T off64_t
#  define _PSF_OFFSET64 psf_offset64
# endif /* defined(__STDC_32_MODE__) */
#endif /* defined(_PSTAT64) */

#define _T_PTR_T	void*
#define __psdev		psdev
#define __psfsid	psfsid
#define __psfileid	psfileid
#define __pstcycles	pstcycles

#define __pst_status	pst_status
#define __pst_pset      pst_pset

#define __pst_static	pst_static
#define __pst_dynamic	pst_dynamic
#define __pst_processor	pst_processor
#define __pscoproc	pscoproc

#define __pst_vminfo	pst_vminfo
#define __pst_vm_status	pst_vm_status
#define __pst_swapinfo	pst_swapinfo
#define __pss_blk	pss_blk
#define __pss_fs	pss_fs

#define __pst_diskinfo	pst_diskinfo
#define __pshwpath	pshwpath
#define __psttime	psttime

#define __pst_ipcinfo	pst_ipcinfo
#define __pst_seminfo	pst_seminfo
#define __pst_msginfo	pst_msginfo
#define __pst_shminfo	pst_shminfo

#define __pst_lvinfo	pst_lvinfo

#define __pst_fileinfo	pst_fileinfo

#define __pst_stable	pst_stable
#define __pst_fileinfo2_args	pst_fileinfo2_args
#define __pst_filedetails_args	pst_filedetails_args
#define __pst_stream_args	pst_stream_args
#define __pst_socket_args	pst_socket_args
#define __pst_pathname_args	pst_pathname_args
#define __pst_mpathname_args	pst_mpathname_args
#define __pst_fileinfo2		pst_fileinfo2
#define __pst_filedetails	pst_filedetails
#define __pst_stream		pst_stream
#define __pst_socket		pst_socket
#define __pst_fid		pst_fid
#define pst_fid_user		pst_fid
#define __pst_mpathnode		pst_mpathnode

#define __pst_pseminfo		pst_pseminfo
#define __pst_pmqinfo		pst_pmqinfo

#include <sys/pstat/pstat_constants.h>
#include <sys/pstat/pstat_body.h>

#include <sys/pstat/pstat_vfs_types.h>
#include <sys/pstat/fid_pstat_body.h>
#include <sys/pstat/filedetails_pstat_body.h>
#include <sys/pstat/stream_pstat_body.h>
#include <sys/pstat/socket_pstat_body.h>
#include <sys/pstat/rpath_pstat_body.h>

#include <sys/pstat/pm_pstat_body.h>
#include <sys/pstat/global_pstat_body.h>
#include <sys/pstat/vm_pstat_body.h>
#include <sys/pstat/disk_pstat_body.h>
#include <sys/pstat/ipc_pstat_body.h>
#include <sys/pstat/lv_pstat_body.h>
#include <sys/pstat/vfs_pstat_body.h>
#include <sys/pstat/mdep_pstat_body.h>

#include <sys/pstat/pset_pstat_body.h>
#include <sys/pstat/pipc_pstat_body.h>

#if defined(_PSTAT64)
#include <sys/pstat/crash_pstat_body.h>
#include <sys/pstat/node_pstat_body.h>
#endif /* defined(_PSTAT64) */

#if defined(__STDC_EXT__) || (!defined(__STDC__) && defined (__LL_MODE__)) || defined(__LP64__)
#include <sys/pstat/lwp_pstat_body.h>
#endif /* __STDC_EXT__ || (! __STDC__ &&  __LL_MODE__ )  || __LP64__ */


/*
 * The following are needed only for the narrow platform.
 */
#if defined(_PSTAT64) && !defined(__LP64__)
# define pstat_getstatic    __pstat_getstatic64
# define pstat_getdynamic   __pstat_getdynamic64
# define pstat_getvminfo    __pstat_getvminfo64
# define pstat_getprocessor __pstat_getprocessor64
# define pstat_getproc      __pstat_getproc64
# define pstat_getdisk      __pstat_getdisk64
# define pstat_getlv        __pstat_getlv64
# define pstat_getswap      __pstat_getswap64
# define pstat_getfile      __pstat_getfile64
# define pstat_getsem       __pstat_getsem64
# define pstat_getmsg       __pstat_getmsg64
# define pstat_getshm       __pstat_getshm64
# define pstat_getipc       __pstat_getipc64
# define pstat_getstable    __pstat_getstable64
# define pstat_getprocvm    __pstat_getprocvm64
# define pstat_getcrashinfo __pstat_getcrashinfo64
# define pstat_getcrashdev  __pstat_getcrashdev64
# define pstat_getnode      __pstat_getnode64
# define pstat_getstream      __pstat_getstream64
# define pstat_getsocket      __pstat_getsocket64
# define pstat_getfiledetails      __pstat_getfiledetails64
# define pstat_getpathname      __pstat_getpathname64
# define pstat_getmpathname     __pstat_getmpathname64
# define pstat_getfile2     __pstat_getfile264
# define pstat_getpset      __pstat_getpset64
# define pstat_getpsem      __pstat_getpsem64
# define pstat_getpmq       __pstat_getpmq64
# define pstat_getcommandline      __pstat_getcommandline64
# define pstat_getlocality	__pstat_getlocality64
# define pstat_getproclocality	__pstat_getproclocality64

#endif /* _PSTAT64 && !__LP64__ */


#ifdef __cplusplus
   extern "C" {
#endif /* __cplusplus */

#if defined(__ia64) && !defined(_LIBC)  
  /* pragmas needed to support -B protected */  
#  pragma extern pstat_getproc, pstat_getstatic, pstat_getdynamic
#  pragma extern pstat_getvminfo, pstat_getdisk, pstat_getprocessor
#  pragma extern pstat_getlv, pstat_getswap, pstat_getfile, pstat_getsem
#  pragma extern pstat_getmsg, pstat_getshm, pstat_getipc, pstat_getprocvm
#  pragma extern pstat_getstable
#  ifdef _PSTAT64
#    pragma extern pstat_getcrashinfo, pstat_getcrashdev, pstat_getnode
#    pragma extern pstat_getlocality, pstat_getproclocality
#  endif /* _PSTAT64 */
#  pragma extern pstat_getstream, pstat_getfile2, pstat_getsocket
#  pragma extern pstat_getfiledetails, pstat_getpathname, pstat_getmpathname
#  pragma extern pstat_getpset, pstat_getpmq, pstat_getpsem
#  pragma extern pstat_getcommandline
#endif /* __ia64 && ! _LIBC */ 

    int pstat_getproc __((struct pst_status *, size_t, size_t, int));
    int pstat_getstatic __((struct pst_static *, size_t, size_t, int));
    int pstat_getdynamic __((struct pst_dynamic *, size_t, size_t, int));
    int pstat_getvminfo __((struct pst_vminfo *, size_t, size_t, int));
    int pstat_getdisk __((struct pst_diskinfo *, size_t, size_t, int));
    int pstat_getprocessor __((struct pst_processor *, size_t, size_t, int));
    int pstat_getlv __((struct pst_lvinfo *, size_t, size_t, int));
    int	pstat_getswap __((struct pst_swapinfo *, size_t, size_t, int));
    int	pstat_getfile __((struct pst_fileinfo *, size_t, size_t, int));
    int	pstat_getsem __((struct pst_seminfo *, size_t, size_t, int));
    int	pstat_getmsg __((struct pst_msginfo *, size_t, size_t, int));
    int	pstat_getshm __((struct pst_shminfo *, size_t, size_t, int));
    int	pstat_getipc __((struct pst_ipcinfo *, size_t, size_t, int));
    int pstat_getprocvm __((struct pst_vm_status *, size_t, size_t, int));
    int pstat_getstable __((struct pst_stable *, size_t, size_t, int));
#ifdef _PSTAT64
    int pstat_getcrashinfo __((struct pst_crashinfo *, size_t, size_t, int));
    int pstat_getcrashdev __((struct pst_crashdev *, size_t, size_t, int));
    int pstat_getnode __((struct pst_node *, size_t, size_t, int));
    int pstat_getlocality __((struct pst_locality *, size_t, size_t, int));
    int pstat_getproclocality __((struct pst_proc_locality *, size_t, 
				  size_t, int));
#endif /* _PSTAT64 */
#if defined(__STDC_EXT__) || (!defined(__STDC__) && defined (__LL_MODE__)) || defined(__LP64__)
#   if defined(__ia64) && !defined(_LIBC)  
     /* pragmas needed to support -B protected */  
#     pragma extern pstat_getlwp
#   endif /* __ia64 && ! _LIBC */ 
    int pstat_getlwp __((struct lwp_status *, size_t, size_t, int, pid_t));
#endif /* __STDC_EXT__ || (! __STDC__ &&  __LL_MODE__ )  || __LP64__ */

    int	pstat_getstream __((struct pst_stream *, size_t, size_t, int, 
			    struct pst_fid *));
    int	pstat_getfile2 __((struct pst_fileinfo2 *, size_t, size_t, int, pid_t));
    int	pstat_getsocket __((struct pst_socket *, size_t, 
				struct pst_fid *));
    int	pstat_getfiledetails __((struct pst_filedetails *, size_t, 
				 struct pst_fid *));
    int pstat_getpathname __((char *, size_t, struct pst_fid *));
    int pstat_getmpathname __((struct pst_mpathnode *, size_t, size_t, int, 
			       struct psfsid *));
    int pstat_getpset __((struct pst_pset *, size_t, size_t, int));
    int pstat_getpsem __((struct pst_pseminfo *, size_t, size_t, int));
    int pstat_getpmq __((struct pst_pmqinfo *, size_t, size_t, int));
    int pstat_getcommandline __((char *, size_t, size_t, pid_t));

#ifdef __cplusplus
   }
#endif /* __cplusplus */



/* ============ Beginning of old pstat interface spec =========== */

/* old union for buf argument */
union pstun {
	struct pst_static	*pst_static;
	struct pst_dynamic	*pst_dynamic;
	struct pst_status	*pst_status;
	char			*pst_command;
	struct pst_vminfo	*pst_vminfo;
	struct pst_diskinfo	*pst_diskinfo;
	struct pst_processor	*pst_processor;
	struct pst_lvinfo	*pst_lv;
	struct pst_swapinfo	*pst_swapinfo;
	struct pst_fileinfo	*pst_fileinfo;
	struct pst_seminfo	*pst_seminfo;
	struct pst_msginfo	*pst_msginfo;
	struct pst_shminfo	*pst_shminfo;
	struct pst_ipcinfo	*pst_ipcinfo;
	struct pst_vm_status    *pst_vm_status;
	struct pst_stable       *pst_stable;
        struct pst_crashinfo    *pst_crashinfo;
        struct pst_crashdev     *pst_crashdev;
	struct pst_node		*pst_node;
	struct pst_stream_args  *pst_stream_args;
	struct pst_socket_args  *pst_socket_args;
	struct pst_fileinfo2_args  *pst_fileinfo2_args;
	struct pst_filedetails_args *pst_filedetails_args;
	struct pst_pathname_args *pst_pathname_args;
	struct pst_mpathname_args *pst_mpathname_args;
        struct pst_pset         *pst_pset;
        struct pst_pseminfo	*pst_pseminfo;
        struct pst_pmqinfo	*pst_pmqinfo;
	struct pst_locality	*pst_locality;
	struct pst_proc_locality *pst_proc_locality;
};

/* old function prototype */

#ifdef __cplusplus
   extern "C" {
#endif /* __cplusplus */

#if defined(__ia64) && !defined(_LIBC)  
  /* pragmas needed to support -B protected */  
#  pragma extern pstat
#endif /* __ia64 && ! _LIBC */ 

    int pstat __((int, union pstun, size_t, size_t, int));
#ifdef __cplusplus
   }
#endif /* __cplusplus */


#endif /* _INCLUDE_HPUX_SOURCE */

#endif /* _NO_USER_PROTOS */

#endif /* _SYS_PSTAT_INCLUDED */
