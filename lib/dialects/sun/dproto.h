/*
 * dproto.h - Solaris function prototypes for lsof
 *
 * The _PROTOTYPE macro is defined in the common proto.h.
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
 * $Id: dproto.h,v 1.21 2010/01/18 19:03:54 abe Exp $
 */

#if defined(HASVXFSUTIL)
extern int access_vxfs_ioffsets(void);
#endif /* defined(HASVXFSUTIL) */

extern void completevfs(struct lsof_context *ctx, struct l_vfs *vfs,
                        dev_t *dev);


extern int is_file_named(struct lsof_context *ctx, char *p, int nt,
                         enum vtype vt, int ps);

#if defined(HAS_AFS)
extern struct vnode *alloc_vcache(void);
extern void ckAFSsym(struct nlist *nl);
extern int hasAFS(struct vnode *vp);
extern int readafsnode(KA_T va, struct vnode *v, struct afsnode *an);
#endif /* defined(HAS_AFS) */

#if defined(HASDCACHE)
extern int rw_clone_sect(struct lsof_context *ctx, int m);
extern void clr_sect(struct lsof_context *ctx);
extern int rw_pseudo_sect(struct lsof_context *ctx, int m);
#endif /* defined(HASDCACHE) */

#if defined(HASIPv6)
extern struct hostent *gethostbyname2(const char *nm, int proto);
#endif /* defined(HASIPv6) */


#if defined(HASVXFS)
extern int read_vxnode(struct lsof_context *ctx, KA_T va, struct vnode *v,
                       struct l_vfs *vfs, int fx, struct l_ino *li,
                       KA_T *vnops);
#    if defined(HASVXFSRNL)
extern int print_vxfs_rnl_path(struct lfile *lf);
#    endif /* defined(HASVXFSRNL) */
#endif     /* defined(HASVXFS) */

#if defined(HASZONES)
extern int enter_zone_arg(struct lsof_context *ctx, char *zn);
#endif /* defined(HASZONES) */

extern void read_clone(struct lsof_context *ctx);

#if solaris < 20500
extern int get_max_fd(void);
#endif /* solaris<20500 */

#if defined(WILLDROPGID)
extern void restoregid(struct lsof_context *ctx);
#endif /* defined(WILLDROPGID) */
