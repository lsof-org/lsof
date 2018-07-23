/*
 * BEGIN_DESC
 *
 *  File:
 *      @(#)B11.23_LR    common/pstat/pstat_vfs_types.h           
 *
 *  Purpose:
 * 	common types and constants used by vfs_pstat.c and filedetails_pstat.c
 *
 *  Classification:                     Release to Release Consistency Req:
 *      public                                  binary & source
 *
 *  BE header:  yes
 *
 *  Shipped:  yes
 *      /usr/include/pstat/pstat_vfs_types.h
 *
 * END_DESC
 */

#ifndef _SYS_PSTAT_PSTAT_VFS_TYPES_INCLUDED
#define _SYS_PSTAT_PSTAT_VFS_TYPES_INCLUDED

/* 
 * Macros for field validity check in pst_fileinfo 
 */
#define PSF_OFFSET       0x1

/* 
 * PSTAT equivalent file types 
 */
#define PS_TYPE_UNKNOWN 0	/* Unknown file type */
#define PS_TYPE_VNODE   1	/* File */
#define PS_TYPE_SOCKET  2	/* Berkeley IPC Socket */
#define PS_TYPE_STREAMS 3	/* Streams file type */
#define PS_TYPE_UNSP    5	/* user nsp control */
#define PS_TYPE_LLA     6	/* link-level lan access */

/* 
 * New enum type to represent sub-file types 
 */
#ifndef _PST_SUBTYPE_T
#define _PST_SUBTYPE_T

enum pst_subtype {

	/* 
	 * Subtypes for type VNODE  
	 */
	PS_SUBTYPE_UNKNOWN,	/* Unknown */
	PS_SUBTYPE_REG,		/* Regular type */
	PS_SUBTYPE_DIR,		/* Directory */
	PS_SUBTYPE_BLKDEV,	/* File opened to block device */

	/* 
	 * Subtypes for type VNODE/STREAM  
	 */
	PS_SUBTYPE_CHARDEV,	/* File opended to character device */
	PS_SUBTYPE_FIFO,	/* A pipe */

	/* 
	 * Subtypes for type SOCKET  
	 */
	PS_SUBTYPE_SOCK,	/* Regular socket */
	PS_SUBTYPE_SOCKSTR	/* Stream socket */
};

typedef enum pst_subtype pst_subtype_t;

#endif				/* _PST_SUBTYPE_T */

/* 
 * Define flag bits for file status (for variable psf_flag) 
 */
#define PS_FRDONLY 0x1		/* Read-only/receive'able file */
#define PS_FWRONLY 0x2		/* Write/send'able  */
#define PS_FAPPEND 0x4		/* append on each write */
#define PS_FNODELY 0x8		/* No delay */
#define PS_FNBLOCK 0x10
#define PS_FSYNC   0x20		/* Synchronized IO file integrity completion */
#define PS_FDSYNC  0x40		/* Synchronized IO data integrity completion */
#define PS_FRSYNC  0x80		/* Synchronized IO read */
#define PS_FLGFILE 0x100	/* large file open flag */
#define PS_FEXCLOS 0x200	/* auto-close on exec */

#endif /* _SYS_PSTAT_PSTAT_VFS_TYPES_INCLUDED */
