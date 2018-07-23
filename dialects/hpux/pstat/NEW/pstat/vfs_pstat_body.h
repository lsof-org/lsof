/*
 * BEGIN_DESC
 *
 *  File:
 *      @(#)B11.23_LR    common/pstat/vfs_pstat_body.h
 *
 *  Purpose:
 *      pst_fileinfo body declarations and user constants
 *
 *  Classification:                     Release to Release Consistency Req:
 *      public                                  binary & source
 *
 *  BE header:  yes
 *
 *  Shipped:  yes
 *      /usr/include/pstat/vfs_pstat_body.h
 *
 * END_DESC
 */

/* NO MULTIPLE-INCLUDE PROTECTION (intentional) */

/*
 * Size-invariant types
 *	none
 *
 * Size-variant types
 *  Defines:
 *	__pst_fileinfo
 *
 *  Uses:
 *	__psfsid
 *	__psfileid
 *	_T_ULONG_T
 *	_T_OFF64_T
 *	_PSF_OFFSET64
 *      __pst_fid
 *
 */


/* 
 * In PSTAT, every file has three types of ids. The reason for this many is 
 * as follows.  The way pstat routines are used is, first the user issues a 
 * pstat_getfile() to get a list of all open files for a process. One can
 * issue pstat_getproc() to get cwd and root and pstat_getprocvm() to get 
 * memory regions.  Then, if more information is needed for an opened file,
 * pstat_getfiledetails(), pstat_getsocket(), or pstat_getstream(), or 
 * pstat_getpathname() calls can be used.
 * 
 * The members hi_fileid, lo_fileid, hi_nodeid, lo_nodeid fields are
 * returned by both sets of calls (with exception of pstat_getpathname()). 
 * Then one should compare those ids to make sure that the state of the
 * system has not changed.  That is, between the pstat_getfile call and 
 * pstat_getfiledetails() call, the opened file may have been closed. It is 
 * possible that the same filedescriptor is reassigned to some other file.  
 * But fileid and nodeid fields would be different. This detects the state 
 * change.
 * 
 * The pst_fid is returned only by the first set of calls, that is
 * pstat_getfile(), pstat_getproc() and pstat_getprocvm(). pst_fid is a
 * unique id that is used by kernel to easily access the opened file.  Its
 * contents are not known to the user, they are for kernel use only. The
 * user need only pass the pst_fid back to kernel in the second set of
 * calls so that kernel can efficiently and effectively reaccess the file
 * in question. 
 *
 * The psfsid is returned with every file, cwd, mem regions and root. This
 * contains the filesystem identification and inode numbers.  This needs to
 * be passed to pstat_getmpathname() call to get all the DNLC entries for
 * that filesystem.
 */


/*
 * This structure describes per-file information.
 * Each structure returned describes one open file a process
 */

struct __pst_fileinfo {
	_T_ULONG_T	psf_idx;	/* Index for further pstat() requests */
	_T_ULONG_T	psf_fd;		/* file descriptor of the file */
	struct __psfileid psf_id;	/* unique identification of the file */
	off32_t		psf_offset;	/* current 32-bit offset in the file */
	_T_OFF64_T	_PSF_OFFSET64;	/*
					 * current 64-bit offset in the file
					 * If __STDC_32_MODE__, defined as
					 * off32_t psf_dummy[2];
					 */
	int32_t		psf_valid;	/* valid vector */
	int32_t     psf_ftype;            /* File type, PS_TYPE_VNODE etc. */
	pst_subtype_t     psf_subtype;  /* File sub type */
	int32_t psf_flag;               /* Flags associated with file status */
	struct __pst_fid psf_fid;       /* 
					 * An efficient means to re-access
                                         * the vnode of opened files.
                                         */
	uint32_t psf_hi_fileid;       /* Per shared file ID */
	uint32_t psf_lo_fileid; 
	uint32_t psf_hi_nodeid;       /* Per vnode/socket ID */
	uint32_t psf_lo_nodeid;    
	int32_t psf_nstrentt;       /* 
				     * # of entities in a stream
                                     * This member is valid only for
                                     * streams or sockets that use a
                                     * stream. Head+Modules+Driver
                                     */
	_T_LONG_T	psf_count;	/* Reference count */
};

/* 
 * This is the new fileinfo struct to be used by pstat_getfile2() call
 * The difference is that there is no pst_idx field. The users do not
 * have to put idx they get from pstat_getproc() and file descriptor number 
 * into the pst_idx anymore. The new interface now takes pid as a separate 
 * argument. The psf_fd is now needs to be used in place of pst_idx to be 
 * passed as index to further pstat_getfile2() calls. 
 */
struct __pst_fileinfo2 {
        int32_t  psf_valid;      	/* Valid vector */
        int32_t  psf_ftype;            	/* File type, PS_TYPE_VNODE etc. */
        pst_subtype_t psf_subtype;  	/* File subtype PS_SUBTYPE_CHARDEV ...*/
        int32_t  psf_flag;              /* Flags associated with file status */
        struct __pst_fid psf_fid;       /* 
					 * An efficient means to re-access
                                         * the vnode of opened files.
                                         */
        uint32_t psf_hi_fileid;       	/* Per shared file ID */
        uint32_t psf_lo_fileid;
        uint32_t psf_hi_nodeid;       	/* Per vnode/socket ID */
        uint32_t psf_lo_nodeid;
        int32_t  psf_nstrentt;     	/* 
					 * # of entities in a stream
                                     	 * This member is valid only for
                                     	 * streams or sockets that use a
                                         * stream. Head+Modules+Driver
                                     	 */
        _T_LONG_T   psf_count;      	/* Reference count */
        _T_ULONG_T psf_fd;         	/* 
					 * File descriptor of the file 
					 * and index for further calls 
					 */
        struct __psfileid psf_id;       /* Unique identification of the file */
        off32_t     psf_offset;     	/* Current 32-bit offset in the file */
        _T_OFF64_T  _PSF_OFFSET64;  	/* 
					 * Current 64-bit offset in the file
                                         * If __STDC_32_MODE__, defined as
                                         * off32_t psf_dummy[2];
                                         */
};


struct __pst_fileinfo2_args {
	_T_PTR_T buf;
	pid_t    pid;
};
