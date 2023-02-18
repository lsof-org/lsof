/*
 * proto.h - common function prototypes for lsof
 */

/*
 * Copyright 1994 Purdue Research Foundation, West Lafayette, Indiana
 * 47907.  All rights reserved.
 *
 * Written by Victor A. Abell
 *
 * This software is not subject to any license of the American Telephone
 * and Telegraph Company or the Regents of the University of California.
 *
 * Permission is granted to anyone to use this software for any purpose on
 * any computer system, and to alter it and redistribute it freely, subject
 * to the following restrictions:
 *
 * 1. Neither the authors nor Purdue University are responsible for any
 *    consequences of the use of this software.
 *
 * 2. The origin of this software must not be misrepresented, either by
 *    explicit claim or by omission.  Credit to the authors and Purdue
 *    University must appear in documentation and sources.
 *
 * 3. Altered versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 4. This notice may not be removed or altered.
 */

/*
 * $Id: proto.h,v 1.39 2018/02/14 14:20:14 abe Exp $
 */

#if !defined(PROTO_H)
#    define PROTO_H 1

/*
 * The _PROTOTYPE macro provides strict ANSI C prototypes if __STDC__
 * is defined, and old-style K&R prototypes otherwise.
 *
 * (With thanks to Andy Tanenbaum)
 */

#    if defined(__STDC__)
#        define _PROTOTYPE(function, params) function params
#    else /* !defined(__STDC__) */
#        define _PROTOTYPE(function, params) function()
#    endif /* defined(__STDC__) */

/*
 * The following define keeps gcc>=2.7 from complaining about the failure
 * of the Exit() function to return.
 *
 * Paul Eggert supplied it.
 */

#    if defined(__GNUC__) &&                                                   \
        !(__GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 7))
#        define exiting __attribute__((__noreturn__))
#    else /* !gcc || gcc<2.7 */
#        define exiting
#    endif /* gcc && gcc>=2.7 */

_PROTOTYPE(extern void add_nma, (struct lsof_context * ctx, char *cp, int len));
_PROTOTYPE(extern void alloc_lfile,
           (struct lsof_context * ctx, enum lsof_fd_type fd_type, int num));
_PROTOTYPE(extern void clean_lfile,
           (struct lsof_context * ctx, struct lfile *lf));
_PROTOTYPE(extern void alloc_lproc,
           (struct lsof_context * ctx, int pid, int pgid, int ppid, UID_ARG uid,
            char *cmd, int pss, int sf));
_PROTOTYPE(extern void build_IPstates, (struct lsof_context * ctx));
_PROTOTYPE(extern void childx, (struct lsof_context * ctx));
_PROTOTYPE(extern int ck_fd_status,
           (struct lsof_context * ctx, enum lsof_fd_type fd_type, int num));
_PROTOTYPE(extern enum lsof_error ck_file_arg,
           (struct lsof_context * ctx, char *arg, int fv, int rs,
            struct stat *sbp, int accept_deleted_file));
_PROTOTYPE(extern void ckkv,
           (struct lsof_context * ctx, char *d, char *er, char *ev, char *ea));
_PROTOTYPE(extern void clr_devtab, (struct lsof_context * ctx));
_PROTOTYPE(extern int compdev, (COMP_P * a1, COMP_P *a2));
_PROTOTYPE(extern int comppid, (COMP_P * a1, COMP_P *a2));

#    if defined(WILLDROPGID)
_PROTOTYPE(extern void dropgid, (struct lsof_context * ctx));
#    endif /* defined(WILLDROPGID) */

_PROTOTYPE(extern char *endnm, (struct lsof_context * ctx, size_t *sz));
_PROTOTYPE(extern void enter_dev_ch, (struct lsof_context * ctx, char *m));

#    if defined(HASEOPT)
_PROTOTYPE(extern int enter_efsys, (char *e, int rdlnk));
#    endif /* defined(HASEOPT) */

_PROTOTYPE(extern int enter_fd, (char *f));
_PROTOTYPE(extern int enter_network_address, (char *na));
_PROTOTYPE(extern int enter_id, (enum IDType ty, char *p));
_PROTOTYPE(extern void enter_IPstate,
           (struct lsof_context * ctx, char *ty, char *nm, int nr));
_PROTOTYPE(extern void enter_nm, (struct lsof_context * ctx, char *m));

#    if defined(HASTCPUDPSTATE)
_PROTOTYPE(extern int enter_state_spec, (char *ss));
#    endif /* defined(HASTCPUDPSTATE) */

_PROTOTYPE(extern int enter_cmd, (char *opt, char *s));
_PROTOTYPE(extern int enter_uid, (char *us));
_PROTOTYPE(extern void ent_inaddr,
           (struct lsof_context * ctx, unsigned char *la, int lp,
            unsigned char *fa, int fp, int af));
_PROTOTYPE(extern int examine_lproc, (struct lsof_context * ctx));
_PROTOTYPE(extern void Exit, (struct lsof_context * ctx, enum ExitStatus xv))
exiting;
_PROTOTYPE(extern void Error, (struct lsof_context * ctx));
_PROTOTYPE(extern void find_ch_ino, (struct lsof_context * ctx));

#    if defined(HASEPTOPTS)
_PROTOTYPE(extern void clear_pinfo, (struct lsof_context * ctx));
_PROTOTYPE(extern pxinfo_t *find_pepti, (struct lsof_context * ctx, int pid,
                                         struct lfile *lf, pxinfo_t *pp));
_PROTOTYPE(extern void process_pinfo, (struct lsof_context * ctx, int f));
_PROTOTYPE(extern void clear_psxmqinfo, (struct lsof_context * ctx));
_PROTOTYPE(extern pxinfo_t *find_psxmqinfo, (struct lsof_context * ctx, int pid,
                                             struct lfile *lf, pxinfo_t *pp));
_PROTOTYPE(extern void process_psxmqinfo, (struct lsof_context * ctx, int f));
#        if defined(HASUXSOCKEPT)
_PROTOTYPE(extern void clear_uxsinfo, (struct lsof_context * ctx));
_PROTOTYPE(extern struct uxsin *find_uxsepti,
           (struct lsof_context * ctx, struct lfile *lf));
_PROTOTYPE(extern void process_uxsinfo, (struct lsof_context * ctx, int f));
#        endif /* defined(HASUXSOCKEPT) */
#        if defined(HASPTYEPT)
_PROTOTYPE(extern void clear_ptyinfo, (struct lsof_context * ctx));
_PROTOTYPE(extern void enter_ptmxi, (struct lsof_context * ctx, int mn));
_PROTOTYPE(extern pxinfo_t *find_ptyepti,
           (struct lsof_context * ctx, int pid, struct lfile *lf, int m,
            pxinfo_t *pp));
_PROTOTYPE(extern int is_pty_slave, (int sm));
_PROTOTYPE(extern int is_pty_ptmx, (dev_t dev));
_PROTOTYPE(extern void process_ptyinfo, (struct lsof_context * ctx, int f));
#        endif /* defined(HASPTYEPT) */
_PROTOTYPE(extern void clear_evtfdinfo, (struct lsof_context * ctx));
_PROTOTYPE(extern void enter_evtfdinfo, (struct lsof_context * ctx, int id));
_PROTOTYPE(extern pxinfo_t *find_evtfdinfo, (struct lsof_context * ctx, int pid,
                                             struct lfile *lf, pxinfo_t *pp));
_PROTOTYPE(extern void process_evtfdinfo, (struct lsof_context * ctx, int f));
_PROTOTYPE(extern void clear_netsinfo, (struct lsof_context * ctx));
_PROTOTYPE(extern void process_netsinfo, (struct lsof_context * ctx, int f));
#        if defined(HASIPv6)
_PROTOTYPE(extern void clear_nets6info, (struct lsof_context * ctx));
_PROTOTYPE(extern void process_nets6info, (struct lsof_context * ctx, int f));
#        endif /* defined(HASIPv6) */
#    endif     /* defined(HASEPTOPTS) */

_PROTOTYPE(extern void free_lproc, (struct lproc * lp));
_PROTOTYPE(extern void gather_proc_info, (struct lsof_context * ctx));
_PROTOTYPE(extern char *gethostnm, (unsigned char *ia, int af));

#    if !defined(GET_MAX_FD)
/*
 * This is not strictly a prototype, but GET_MAX_FD is the name of the
 * function that, in lieu of getdtablesize(), returns the maximum file
 * descriptor plus one (or file descriptor count).  GET_MAX_FD may be
 * defined in the dialect's machine.h.  If it is not, the following
 * selects getdtablesize().
 */

#        define GET_MAX_FD getdtablesize
#    endif /* !defined(GET_MAX_FD) */

_PROTOTYPE(extern int hashbyname, (char *nm, int mod));
_PROTOTYPE(extern void hashSfile, (struct lsof_context * ctx));
_PROTOTYPE(extern void initialize, (struct lsof_context * ctx));
_PROTOTYPE(extern void deinitialize, (struct lsof_context * ctx));
_PROTOTYPE(extern int is_cmd_excl,
           (struct lsof_context * ctx, char *cmd, short *pss, short *sf));
_PROTOTYPE(extern int is_file_sel,
           (struct lsof_context * ctx, struct lproc *lp, struct lfile *lf));
_PROTOTYPE(extern int is_nw_addr,
           (struct lsof_context * ctx, unsigned char *ia, int p, int af));

#    if defined(HASTASKS)
_PROTOTYPE(extern int is_proc_excl,
           (struct lsof_context * ctx, int pid, int pgid, UID_ARG uid,
            short *pss, short *sf, int tid));
#    else  /* !defined(HASTASKS) */
_PROTOTYPE(extern int is_proc_excl,
           (struct lsof_context * ctx, int pid, int pgid, UID_ARG uid,
            short *pss, short *sf));
#    endif /* defined(HASTASKS) */

_PROTOTYPE(extern int is_readable,
           (struct lsof_context * ctx, char *path, int msg));
_PROTOTYPE(extern int kread,
           (struct lsof_context * ctx, KA_T addr, char *buf, READLEN_T len));
_PROTOTYPE(extern void link_lfile, (struct lsof_context * ctx));
_PROTOTYPE(extern struct l_dev *lkupdev,
           (struct lsof_context * ctx, dev_t *dev, dev_t *rdev, int i, int r));
_PROTOTYPE(extern int main, (int argc, char *argv[]));
_PROTOTYPE(extern int lstatsafely,
           (struct lsof_context * ctx, char *path, struct stat *buf));
_PROTOTYPE(extern char *mkstrcpy, (char *src, MALLOC_S *rlp));
_PROTOTYPE(extern char *mkstrcat, (char *s1, int l1, char *s2, int l2, char *s3,
                                   int l3, MALLOC_S *clp));
_PROTOTYPE(extern int printdevname, (struct lsof_context * ctx, dev_t *dev,
                                     dev_t *rdev, int f, int nty));
_PROTOTYPE(extern void print_file, (void));
_PROTOTYPE(extern void print_init, (void));
_PROTOTYPE(extern void printname, (int nl));
_PROTOTYPE(extern char *print_kptr, (KA_T kp, char *buf, size_t bufl));
_PROTOTYPE(extern int print_proc, (void));
_PROTOTYPE(extern void print_fd,
           (enum lsof_fd_type fd_type, int fd_num, char *buf));
_PROTOTYPE(extern char *printuid, (UID_ARG uid, int *ty));
_PROTOTYPE(extern void printunkaf,
           (struct lsof_context * ctx, int fam, int ty));
_PROTOTYPE(extern char print_access, (enum lsof_file_access_mode access));
_PROTOTYPE(extern char print_lock, (enum lsof_lock_mode lock));
_PROTOTYPE(extern void print_file_type,
           (enum lsof_file_type type, uint32_t unknown_file_type_number,
            char *buf, size_t buf_len));
_PROTOTYPE(extern void print_iproto,
           (enum lsof_protocol proto, uint32_t unknown_proto_number, char *buf,
            size_t buf_len));
_PROTOTYPE(extern char *printsockty, (int ty));
_PROTOTYPE(extern void process_file, (struct lsof_context * ctx, KA_T fp));
_PROTOTYPE(extern void process_node, (struct lsof_context * ctx, KA_T f));
_PROTOTYPE(extern char *Readlink, (struct lsof_context * ctx, char *arg));
_PROTOTYPE(extern void readdev, (struct lsof_context * ctx, int skip));
_PROTOTYPE(extern struct mounts *readmnt, (struct lsof_context * ctx));
_PROTOTYPE(extern void clean_mnt, (struct lsof_context * ctx));
_PROTOTYPE(extern void rereaddev, (struct lsof_context * ctx));
_PROTOTYPE(extern char *safepup, (unsigned int c, int *cl));
_PROTOTYPE(extern int safestrlen, (char *sp, int flags));
_PROTOTYPE(extern void safestrprtn, (char *sp, int len, FILE *fs, int flags));
_PROTOTYPE(extern void safestrprt, (char *sp, FILE *fs, int flags));
_PROTOTYPE(extern int statsafely,
           (struct lsof_context * ctx, char *path, struct stat *buf));
_PROTOTYPE(extern void stkdir, (struct lsof_context * ctx, char *p));
_PROTOTYPE(extern void usage, (int err, int fh, int version));
_PROTOTYPE(extern int util_strftime, (char *fmtr, int fmtl, char *fmt));
_PROTOTYPE(extern int vfy_dev, (struct lsof_context * ctx, struct l_dev *dp));
_PROTOTYPE(extern char *x2dev, (char *s, dev_t *d));

#    if defined(HASBLKDEV)
_PROTOTYPE(extern void find_bl_ino, (struct lsof_context * ctx));
_PROTOTYPE(extern struct l_dev *lkupbdev,
           (struct lsof_context * ctx, dev_t *dev, dev_t *rdev, int i, int r));
_PROTOTYPE(extern int printbdevname, (dev_t * dev, dev_t *rdev, int f));
#    endif /* defined(HASBLKDEV) */

#    if defined(HASCDRNODE)
_PROTOTYPE(extern int readcdrnode, (KA_T ca, struct cdrnode *c));
#    endif /* defined(HASCDRNODE) */

#    if defined(HASDCACHE)
_PROTOTYPE(extern void alloc_dcache, (struct lsof_context * ctx));
_PROTOTYPE(extern void crc, (char *b, int l, unsigned *s));
_PROTOTYPE(extern void crdbld, (void));
_PROTOTYPE(extern int ctrl_dcache, (char *p));
_PROTOTYPE(extern int dcpath, (struct lsof_context * ctx, int rw, int npw));
_PROTOTYPE(extern int open_dcache,
           (struct lsof_context * ctx, int m, int r, struct stat *sb));
_PROTOTYPE(extern int read_dcache, (struct lsof_context * ctx));
_PROTOTYPE(extern int wr2DCfd,
           (struct lsof_context * ctx, char *b, unsigned *c));
_PROTOTYPE(extern void write_dcache, (struct lsof_context * ctx));
#    endif /* defined(HASDCACHE) */

#    if defined(HASFIFONODE)
_PROTOTYPE(extern int readfifonode,
           (struct lsof_context * ctx, KA_T fa, struct fifonode *f));
#    endif /* defined(HASFIFONODE) */

#    if defined(HASFSTRUCT)
_PROTOTYPE(extern char *print_fflags,
           (struct lsof_context * ctx, long ffg, long pof));
#    endif /* defined(HASFSTRUCT) */

#    if defined(HASGNODE)
_PROTOTYPE(extern int readgnode, (KA_T ga, struct gnode *g));
#    endif /* defined(HASGNODE) */

#    if defined(HASKQUEUE)
_PROTOTYPE(extern void process_kqueue, (struct lsof_context * ctx, KA_T ka));
#    endif /* defined(HASKQUEUE) */

#    if defined(HASHSNODE)
_PROTOTYPE(extern int readhsnode,
           (struct lsof_context * ctx, KA_T ha, struct hsnode *h));
#    endif /* defined(HASHSNODE) */

#    if defined(HASINODE)
_PROTOTYPE(extern int readinode,
           (struct lsof_context * ctx, KA_T ia, struct inode *i));
#    endif /* defined(HASINODE) */

#    if defined(HASNCACHE)
_PROTOTYPE(extern void ncache_load, (struct lsof_context * ctx));
_PROTOTYPE(extern char *ncache_lookup,
           (struct lsof_context * ctx, char *buf, int blen, int *fp));
#    endif /* defined(HASNCACHE) */

#    if defined(HASNLIST)
_PROTOTYPE(extern void build_Nl,
           (struct lsof_context * ctx, struct drive_Nl *d));
_PROTOTYPE(extern int get_Nl_value,
           (struct lsof_context * ctx, char *nn, struct drive_Nl *d, KA_T *v));
#    endif /* defined(HASNLIST) */

#    if defined(HASPIPENODE)
_PROTOTYPE(extern int readpipenode, (KA_T pa, struct pipenode *p));
#    endif /* defined(HASPIPENODE) */

#    if defined(HASPRINTDEV)
_PROTOTYPE(extern char *HASPRINTDEV, (struct lfile * lf, dev_t *dev));
#    endif /* defined(HASPRINTDEV) */

#    if defined(HASPRINTINO)
_PROTOTYPE(extern char *HASPRINTINO, (struct lfile * lf));
#    endif /* defined(HASPRINTINO) */

#    if defined(HASPRINTNM)
_PROTOTYPE(extern void HASPRINTNM, (struct lfile * lf));
#    endif /* defined(HASPRINTNM) */

#    if defined(HASPRIVNMCACHE)
_PROTOTYPE(extern int HASPRIVNMCACHE,
           (struct lsof_context * ctx, struct lfile *lf));
#    endif /* defined(HASPRIVNMCACHE) */

_PROTOTYPE(extern void enter_ip_proto, (struct lsof_context * ctx, int p));

#    if defined(HASRNODE)
_PROTOTYPE(extern int readrnode,
           (struct lsof_context * ctx, KA_T ra, struct rnode *r));
#    endif /* defined(HASRNODE) */

#    if defined(HASSPECDEVD)
_PROTOTYPE(extern void HASSPECDEVD,
           (struct lsof_context * ctx, char *p, struct stat *s));
#    endif /* defined(HASSPECDEVD) */

#    if defined(HASSNODE)
_PROTOTYPE(extern int readsnode,
           (struct lsof_context * ctx, KA_T sa, struct snode *s));
#    endif /* defined(HASSNODE) */

#    if defined(HASSTREAMS)
_PROTOTYPE(extern int readstdata,
           (struct lsof_context * ctx, KA_T addr, struct stdata *buf));
_PROTOTYPE(extern int readsthead,
           (struct lsof_context * ctx, KA_T addr, struct queue *buf));
_PROTOTYPE(extern int readstidnm,
           (struct lsof_context * ctx, KA_T addr, char *buf, READLEN_T len));
_PROTOTYPE(extern int readstmin,
           (struct lsof_context * ctx, KA_T addr, struct module_info *buf));
_PROTOTYPE(extern int readstqinit,
           (struct lsof_context * ctx, KA_T addr, struct qinit *buf));
#    endif /* defined(HASSTREAMS) */

#    if defined(HASTMPNODE)
_PROTOTYPE(extern int readtnode,
           (struct lsof_context * ctx, KA_T ta, struct tmpnode *t));
#    endif /* defined(HASTMPNODE) */

#    if defined(HASVNODE)
_PROTOTYPE(extern int readvnode,
           (struct lsof_context * ctx, KA_T va, struct vnode *v));
#    endif /* defined(HASVNODE) */

#endif /* !defined(PROTO_H) */
