/*
 * dproto.h - FreeBSD function prototypes for lsof
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
 * $Id: dproto.h,v 1.15 2018/02/14 14:26:03 abe Exp $
 */

#if !defined(N_UNIX)
_PROTOTYPE(extern char *get_nlist_path, (struct lsof_context * ctx, int ap));
#endif /* !defined(N_UNIX) */

_PROTOTYPE(extern int is_file_named,
           (struct lsof_context * ctx, char *p, int cd));
_PROTOTYPE(extern void process_vnode,
           (struct lsof_context * ctx, struct kinfo_file *kf,
            struct xfile *xfile, struct lock_list *locks));
_PROTOTYPE(extern void process_socket,
           (struct lsof_context * ctx, struct kinfo_file *kf,
            struct pcb_lists *pcbs));
_PROTOTYPE(extern struct l_vfs *readvfs,
           (struct lsof_context * ctx, uint64_t fsid, const char *path));
_PROTOTYPE(extern struct pcb_lists *read_pcb_lists, (void));
_PROTOTYPE(extern void free_pcb_lists, (struct pcb_lists * pcb_lists));
_PROTOTYPE(extern int cmp_kinfo_lockf, (const void *a, const void *b));

_PROTOTYPE(extern void process_pts,
           (struct lsof_context * ctx, struct kinfo_file *kf));

#if defined(KF_TYPE_EVENTFD)
_PROTOTYPE(extern void process_eventfd,
           (struct lsof_context * ctx, struct kinfo_file *kf));
#endif /* defined(KF_TYPE_EVENTFD) */

#if defined(HASKQUEUE)
_PROTOTYPE(extern void process_kf_kqueue,
           (struct lsof_context * ctx, struct kinfo_file *kf, KA_T ka));
#endif /* defined(HASKQUEUE) */

_PROTOTYPE(extern void process_pipe,
           (struct lsof_context * ctx, struct kinfo_file *kf, KA_T pa));
_PROTOTYPE(extern void process_shm,
           (struct lsof_context * ctx, struct kinfo_file *kf));
_PROTOTYPE(extern void process_procdesc,
           (struct lsof_context * ctx, struct kinfo_file *kf));

#if defined(HAS9660FS)
_PROTOTYPE(extern int read_iso_node, (struct vnode * v, dev_t *d, int *dd,
                                      INODETYPE *ino, long *nl, SZOFFTYPE *sz));
#endif /* defined(HAS9660FS) */

#if defined(HASMSDOSFS)
_PROTOTYPE(extern int read_msdos_node,
           (struct vnode * v, dev_t *d, int *dd, INODETYPE *ino, long *nl,
            SZOFFTYPE *sz));
#endif /* defined(HASMSDOSFS) */
