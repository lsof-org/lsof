/*
 * BEGIN_DESC
 *
 *  File:
 *      @(#)B11.23_LR    common/include/pstat/pset_pstat_body.h
 *
 *  Purpose:
 *      __pst_pset  body declarations and user constants
 *
 *  Classification:                     Release to Release Consistency Req:
 *      public                                  binary & source
 *
 *  BE header:  yes
 *
 *  Shipped:  yes
 *      /usr/include/pstat/pset_pstat_body.h
 *
 * END_DESC
 */

/* NO MULTIPLE-INCLUDE PROTECTION (intentional) */

/*
 * This structure contains per PSET information
 */

struct __pst_pset {
    _T_LONG_T pst_id;               /* unique pset id */
    _T_LONG_T pst_processor_cnt;    /* number of processors
				       currently assigned to pset */
    _T_LONG_T pst_process_cnt;      /* number of processes bound
				       to pset */
    _T_LONG_T pst_thread_cnt;       /* number of threads bound
				       to pset */
    _T_LONG_T pst_owner;            /* owner's uid */
    _T_LONG_T pst_group;            /* owner's gid */
    _T_LONG_T pst_access;           /* access permission bit */
    _T_LONG_T pst_ldom_cnt;         /* number of inclusive ldom's */
    double    pst_reserved_double[3]; /*HP-UX internal use only */
    _T_LONG_T pst_reserved[26]; /*HP-UX internal use only */
    double    pst_load_avg_1_min;   /* per pset load average */
    double    pst_load_avg_5_min;
    double    pst_load_avg_15_min;
    _T_LONG_T pst_thrds_on_runqs;    /* total number of threads currently
				       on runq in this pset */
    _T_LONG_T pst_rtenabled;        /* is Pset a RTE Pset */
};
