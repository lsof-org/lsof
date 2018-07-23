/* lv_pstat_body.h : pst_lvinfo* body declarations and user constants */

/*  @(#)B11.23_LR    */
/* NO MULTIPLE-INCLUDE PROTECTION (intentional) */

/*
 * Constants
 *	none
 *
 * Size-invariant types
 *	none
 *	
 *
 * Size-variant types
 *  Defines:
 *	__pst_lvinfo
 *
 *  Uses:
 *	__psdev
 *	_T_ULONG_T
 *
 */




/* pst_lvinfo support */

/*
 * This structure contains per-logical volume information.
 * Each structure returned describes one logical volume.
 */
struct __pst_lvinfo {
	_T_ULONG_T	psl_idx;	/* Index for further pstat() requests*/
	struct __psdev	psl_dev;	/* device specification for the vol */
	_T_ULONG_T	psl_rxfer;	/* # of reads */
	_T_ULONG_T	psl_rcount;	/* # of bytes read */
	_T_ULONG_T	psl_wxfer;	/* # of writes */
	_T_ULONG_T	psl_wcount;	/* # of bytes written */
	_T_ULONG_T	psl_openlv;	/* # of opened LV's in this LV's LVG */
	_T_ULONG_T	psl_mwcwaitq;	/* Length of LV's LVG
					 * mirror write consistency cache (MWC)
					 */
	_T_ULONG_T	psl_mwcsize;	/* Size of LV's LVG's MWC */
	_T_ULONG_T	psl_mwchits;	/* # of hits to the LV's LVG's MWC */
	_T_ULONG_T	psl_mwcmisses;	/* # of misses to the LV's LVG's MWC */
};

