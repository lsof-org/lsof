/*
 * BEGIN_DESC
 *
 *  File:
 *      @(#)B11.23_LR    common/pstat/pstat_ops.h
 *
 *  Purpose:
 * 	function codes to the pstat() system call.
 *
 *  Classification:                     Release to Release Consistency Req:
 *      public                                  binary & source
 *
 *  BE header:  yes
 *
 *  Shipped:  yes
 *      /usr/include/pstat/pstat_ops.h
 *
 * END_DESC
 */

#ifndef _SYS_PSTAT_PSTAT_OPS_INCLUDED
#define _SYS_PSTAT_PSTAT_OPS_INCLUDED

/*
 * The most significant (sign) bit of the function will be
 * set for the 64bit version of the pstat functions.
 */
#define _PSTAT64_INTF	((int)0x80000000)
#define _PSTAT32_INTF	(0)

#define PSTAT_PROC	( 1 | _PSTAT_INTF)
#define PSTAT_STATIC	( 2 | _PSTAT_INTF)
#define PSTAT_DYNAMIC	( 3 | _PSTAT_INTF)
#define PSTAT_SETCMD	( 4 | _PSTAT_INTF)
#define PSTAT_VMINFO	( 5 | _PSTAT_INTF)
#define PSTAT_DISKINFO	( 6 | _PSTAT_INTF)
#define PSTAT_SEMINFO	( 7 | _PSTAT_INTF)
#define PSTAT_MSGINFO	( 8 | _PSTAT_INTF)
#define PSTAT_SHMINFO	( 9 | _PSTAT_INTF)
#define PSTAT_PROCESSOR (10 | _PSTAT_INTF)
#define PSTAT_LVINFO	(11 | _PSTAT_INTF)
#define PSTAT_SWAPINFO	(12 | _PSTAT_INTF)
#define PSTAT_FILEINFO	(13 | _PSTAT_INTF)
#define PSTAT_IPCINFO	(14 | _PSTAT_INTF)
#define PSTAT_PROC_VM   (15 | _PSTAT_INTF)
#define PSTAT_STABLE    (16 | _PSTAT_INTF)

#define PSTAT_CRASHINFO (17 | _PSTAT_INTF) /* 64-bit only */
#define PSTAT_CRASHDEV  (18 | _PSTAT_INTF) /* 64-bit only */

/*
 * Function number 19,20,21 taken by kernel
 */

#define PSTAT_NODE	(22 | _PSTAT_INTF) /* 64-bit only */

/*
 * Function number 23 taken by kernel
 */
#define PSTAT_FILEDETAILS	(24 | _PSTAT_INTF)
#define PSTAT_SOCKET	(25 | _PSTAT_INTF)
#define PSTAT_STREAM	(26 | _PSTAT_INTF)

#define PSTAT_PATHNAME	(27 | _PSTAT_INTF)
#define PSTAT_MPATHNAME	(28 | _PSTAT_INTF)
#define PSTAT_FILEINFO2 (29 | _PSTAT_INTF)

#define PSTAT_PSET      (30 | _PSTAT_INTF)

/* function number 31 taken by kernel */

#define PSTAT_PSEMINFO	(32 | _PSTAT_INTF)
#define PSTAT_PMQINFO	(33 | _PSTAT_INTF)

/* function code for pstat_getcommandline */
#define PSTAT_GETCOMMANDLINE  (34 | _PSTAT_INTF)

#define PSTAT_LOCALITY        (35 | _PSTAT_INTF)
#define PSTAT_PROC_LOCALITY   (36 | _PSTAT_INTF)

#endif /* _SYS_PSTAT_PSTAT_OPS_INCLUDED */

