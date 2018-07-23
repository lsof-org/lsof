/* disk_pstat_body.h : pst_diskinfo body declarations and user constants */

/*   @(#)B11.23_LR    */
/* NO MULTIPLE-INCLUDE PROTECTION (intentional) */

/*
 * Constants
 *	PS_DK_CYL_SIZE
 *	PS_MAX_HW_ELEMS
 *
 * Size-invariant types
 *  Defines:
 *	psdrvnam
 *
 * Size-variant types
 *  Defines:
 *	__pst_diskinfo
 *	__pshwpath
 *
 *  Uses:
 *	__psdev
 *	__psttime
 *	_T_LONG_T
 *
 */


/* pst_diskinfo support */


#if !defined(_PST_DISKINFO_INVARIANTS)
#define _PST_DISKINFO_INVARIANTS

#define PS_DK_CYL_SIZE	80

/*
 * An abstraction of a hardware path
 */
#define PS_MAX_HW_ELEMS	32


/*
 * An abstraction of a driver name
 */
#define PS_DRV_NAM_MAX	32
struct psdrvnam {
	char psd_name[PS_DRV_NAM_MAX];	/* driver name */
};

#endif /* !defined(_PST_DISKINFO_INVARIANTS) */


struct __pshwpath {
	_T_LONG_T psh_first_index;		
	_T_LONG_T psh_last_index;		
	char psh_name[PS_MAX_HW_ELEMS]; /* hardware path */
};

struct __pst_diskinfo {
	_T_LONG_T	psd_idx;	/* Index for further pstat() requests */
	struct __psdev	psd_dev;	/* device specification for the disk */
	_T_LONG_T	psd_dktime;	/* cumulative ticks on the disk */
	_T_LONG_T	psd_dkseek;	/* cumulative number of seeks done */
	_T_LONG_T	psd_dkxfer;	/* cumulative number of transfers;
					 * includes requests with high 
					 * (read/write) and low (ioctl) priorities
					 */
	_T_LONG_T	psd_dkwds;	/* cumulative number of 64-byte transfers */
	float		psd_dkmspw;	/* OBSOLETE: was milliseconds per word */
	struct __psdev	psd_cdev;	/* device specification for the raw disk */
				/* psd_dev (above) describes the block dev */
	struct psdrvnam	psd_drv_name;	/* driver name */
        _T_LONG_T	psd_token;	/* driver's id */
        _T_LONG_T	psd_instance;   /* the instance of the device */
        struct __pshwpath psd_hw_path;  /* hardware path */
	struct __psttime psd_dkwait; /* cumulative time from enqueue to start */
	struct __psttime psd_dkresp; /* cumulative time from enqueue to done */
        _T_LONG_T psd_dkcyl_index;  /* cylinder number index, used by sadp */
        _T_LONG_T psd_dkcyl[PS_DK_CYL_SIZE]; /* cylinder number array, used by 
                                            * sadp 
                                            */
	_T_LONG_T psd_dkqlen_curr;/* current queue length */
	_T_LONG_T psd_dkqlen;     /* cummulative queue length */
	_T_LONG_T psd_dkq_merged; /* cummulative # of transfer would have been
				 * if some of the requests weren't merged
				 */
	_T_LONG_T psd_dkenq_cnt;  /* number of calls to enqueue */
	_T_LONG_T psd_status;     /* 0 = device is closed, 1 = device is open */
};
