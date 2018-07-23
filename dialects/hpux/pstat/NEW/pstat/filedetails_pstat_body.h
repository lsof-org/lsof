/*
 * BEGIN_DESC
 *
 *  File:
 *      @(#)B11.23_LR    common/include/pstat/filedetails_pstat_body.h 
 *
 *  Purpose:
 * 	pst_filedetails body declarations and user constants
 *
 *  Classification:                     Release to Release Consistency Req:
 *      public                                  binary & source
 *
 *  BE header:  yes
 *
 *  Shipped:  yes
 *	/usr/include/pstat/filedetails_pstat_body.h
 *
 * END_DESC
 */

/* NO MULTIPLE-INCLUDE PROTECTION (intentional) */

/*
 * Constants
 * PS_ISUID, PS_ISGID, PS_IRWXU, PS_IRUSR, PS_IWUSR, PS_IXUSR, PS_IRWXG,
 * PS_IRGRP, PS_IWGRP, PS_IXGRP, PS_IRWXO, PS_IROTH, PS_IWOTH, PS_IXOTH,
 * PS_IFMT, PS_IFREG, PS_IFBLK, PS_IFDIR,  PS_IFDIR, PS_IFCHR, PS_IFIFO,
 *
 * Size-invariant types
 *      none
 *
 * Size-variant types
 *  Defines:
 *      __pst_filedetails
 *
 *  Uses:
 *      _T_LONG_T
 *      _T_ULONG_T
 *      _T_PTR_T
 *
 */

#if !defined(_PST_FILEDETAILS_CONSTANTS_DEFINED)
#define _PST_FILEDETAILS_CONSTANTS_DEFINED

/* 
 * Symbols and macros for decoding the value of psfd_mode. 
 */
#define PS_ISUID 0004000	/* set user ID on execution */
#define PS_ISGID 0002000	/* set group ID on execution */

#define PS_IRWXU 0000700	/* read, write, execute permission(owner) */
#define PS_IRUSR 0000400	/* read permission (owner) */
#define PS_IWUSR 0000200	/* write permission (owner) */
#define PS_IXUSR 0000100	/* execute permission (owner) */

#define PS_IRWXG 0000070	/* read, write, execute permission(group) */
#define PS_IRGRP 0000040	/* read permission (group) */
#define PS_IWGRP 0000020	/* write permission (group) */
#define PS_IXGRP 0000010	/* execute permission (group) */

#define PS_IRWXO 0000007	/* read, write, execute permission(other) */
#define PS_IROTH 0000004	/* read permission (other) */
#define PS_IWOTH 0000002	/* write permission (other) */
#define PS_IXOTH 0000001	/* execute permission (other) */

#define PS_IFMT  0170000	/* type of file mask */
#define PS_IFREG 0100000	/* regular */
#define PS_IFBLK 0060000	/* block special */
#define PS_IFDIR 0040000	/* directory */
#define PS_IFCHR 0020000	/* character special */
#define PS_IFIFO 0010000	/* pipe or FIFO */

#define PS_ISDIR(_M) (PS_IFDIR==(_M & PS_IFMT))	/* test for directory */
#define PS_ISCHR(_M) (PS_IFCHR==(_M & PS_IFMT))	/* for char special */
#define PS_ISBLK(_M) (PS_IFBLK==(_M & PS_IFMT))	/* for block special */
#define PS_ISREG(_M)  (PS_IFREG==(_M & PS_IFMT))	/* for regular file */
#define PS_ISFIFO(_M) (PS_IFIFO==(_M & PS_IFMT))	/* for pipe or FIFO */

/* 
 * Define flag bits for file lock status ( for variable psfd_lckflag) 
 */
#define PS_FPARTRDLCK  0x1	/* Partial read lock  */
#define PS_FPARTWRLCK  0x2	/* Partial write lock  */
#define PS_FFULLRDLCK  0x4	/* Full read lock  */
#define PS_FFULLWRLCK  0x8	/* Full write lock  */

/*
 * Macros for field validity check for struct pst_filedetails
 */
#define PSFD_SIZE       0x1

#endif				/* _PST_FILEDETAILS_CONSTANTS_DEFINED */

/* 
 * NO multiple include protection: Intentional 
 */
struct __pst_filedetails {
	uint32_t        psfd_hi_fileid;	/* Per shared file ID */
	uint32_t        psfd_lo_fileid;
	uint32_t        psfd_hi_nodeid;	/* Per vnode ID */
	uint32_t        psfd_lo_nodeid;
	int32_t         psfd_ftype;	/* File type, PS_TYPE_VNODE etc. */
	pst_subtype_t   psfd_subtype;	/* File sub type */
	int32_t         psfd_lckflag;	/* Flags associated with file locking */
	
	/* 
	 * This is a vector that will indicate if a certain field in the 
	 * structure is valid or not. User should check this field when 
	 * EOVERFLOW * is set. 
         */
        int32_t 	psfd_valid; 	

	/*
	 * ID of device containing a directory entry for this file 
	 */
	_T_LONG_T       psfd_dev;
	_T_LONG_T       psfd_ino;	/* file Inode number */
	_T_ULONG_T      psfd_mode;	/* file type, attributes, and acl */
	_T_LONG_T       psfd_nlink;	/* number of links to the file */
	_T_LONG_T       psfd_uid;	/* user ID of file owner */
	_T_LONG_T       psfd_gid;	/* group ID of file group */
	_T_LONG_T       psfd_rdev;	/* device ID (char/block special files*/
	_T_LONG_T       psfd_size;	/* file size in bytes */

	/*
	 * Time measured in secs since 00:00:00 GMT, Jan 1, 1970 
	 */
	_T_LONG_T       psfd_atime;	/* time of last access */
	_T_LONG_T       psfd_mtime;	/* time of last data modification */
	_T_LONG_T       psfd_ctime;	/* time of last file status change */
	_T_LONG_T       psfd_blksize;	/* preferred I/O block size */

	/*
	 * Number of File system specific blocks allocated to this file. 
	 */
	_T_LONG_T       psfd_blocks;
};


struct __pst_filedetails_args {
	_T_PTR_T        buf;
	_T_PTR_T        id;
};
