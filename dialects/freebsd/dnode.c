/*
 * dnode.c - FreeBSD node functions for lsof
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

#ifndef lint
static char copyright[] =
"@(#) Copyright 1994 Purdue Research Foundation.\nAll rights reserved.\n";
#endif


#include "lsof.h"

#if	defined(HAS_LOCKF_ENTRY)
#include "./lockf_owner.h"
#endif	/* defined(HAS_LOCKF_ENTRY) */


#if	defined(HASPTSFN) && defined(DTYPE_PTS)
#include <sys/tty.h>
#endif	/* defined(HASPTSFN) && defined(DTYPE_PTS) */


#if	defined(HAS_TMPFS)
#include <fs/tmpfs/tmpfs.h>
#endif	/* defined(HAS_TMPFS) */

_PROTOTYPE(static void get_lock_state,(KA_T f));


/*
 * get_lock_state() - get the lock state
 */

static void
get_lock_state(f)
	KA_T f;				/* inode's lock pointer */
{
	struct lockf lf;		/* lockf structure */
	int lt;				/* lock type */

#if	defined(HAS_LOCKF_ENTRY)
	struct lockf_entry le;		/* lock_entry structure */
	KA_T lef, lep;			/* lock_entry pointers */
	struct lock_owner lo;		/* lock owner structure */

	if (!f || kread(f, (char *)&lf, sizeof(lf)))
	    return;
	if (!(lef = (KA_T)lf.ls_active.lh_first))
	    return;
	lep = lef;
	do {
	   if (kread(lep, (char *)&le, sizeof(le)))
		return;
	    if (!le.lf_owner
	    ||  kread((KA_T)le.lf_owner, (char *)&lo, sizeof(lo)))
		continue;
	    if (lo.lo_pid == (pid_t)Lp->pid) {
		if (le.lf_start == (off_t)0
		&&  le.lf_end == 0x7fffffffffffffffLL)
		    lt = 1;
		else
		    lt = 0;
		if (le.lf_type == F_RDLCK)
		    Lf->lock = lt ? 'R' : 'r';
		else if (le.lf_type == F_WRLCK)
		    Lf->lock = lt ? 'W' : 'w';
		else if (le.lf_type == (F_RDLCK | F_WRLCK))
		    Lf->lock = 'u';
		return;
	    }
	} while ((lep = (KA_T)le.lf_link.le_next) && (lep != lef));
#else	/* !defined(HAS_LOCKF_ENTRY) */

	unsigned char l;		/* lock status */
	KA_T lfp;			/* lockf structure pointer */

	if ((lfp = f)) {

	/*
	 * Determine the lock state.
	 */
	    do {
		if (kread(lfp, (char *)&lf, sizeof(lf)))
		    break;
		l = 0;
		switch (lf.lf_flags & (F_FLOCK|F_POSIX)) {
		case F_FLOCK:
		    if (Cfp && (struct file *)lf.lf_id == Cfp)
			l = 1;
		    break;
		case F_POSIX:

# if	defined(P_ADDR)
		    if ((KA_T)lf.lf_id == Kpa)
			l = 1;
# endif	/* defined(P_ADDR) */

		    break;
		}
		if (!l)
		    continue;
		if (lf.lf_start == (off_t)0
		&&  lf.lf_end == 0xffffffffffffffffLL)
		    lt = 1;
		else
		    lt = 0;
		if (lf.lf_type == F_RDLCK)
		    Lf->lock = lt ? 'R' : 'r';
		else if (lf.lf_type == F_WRLCK)
		    Lf->lock = lt ? 'W' : 'w';
		else if (lf.lf_type == (F_RDLCK | F_WRLCK))
		    Lf->lock = 'u';
		break;
	    } while ((lfp = (KA_T)lf.lf_next) && (lfp != f));
   	}
#endif	/* defined(HAS_LOCKF_ENTRY) */

}


# if	defined(HASPROCFS)
_PROTOTYPE(static void getmemsz,(pid_t pid));


/*
 * getmemsz() - get memory size of a /proc/<n>/mem entry
 */

static void
getmemsz(pid)
	pid_t pid;
{
	int n;
	struct kinfo_proc *p;
	struct vmspace vm;

	for (n = 0, p = P; n < Np; n++, p++) {
	    if (p->P_PID == pid) {
		if (!p->P_VMSPACE
		||  kread((KA_T)p->P_VMSPACE, (char *)&vm, sizeof(vm)))
		    return;
		Lf->sz = (SZOFFTYPE)ctob(vm.vm_tsize+vm.vm_dsize+vm.vm_ssize);
		Lf->sz_def = 1;
		return;
	    }
	}
}
# endif	/* defined(HASPROCFS) */



#if	defined(HASKQUEUE)
/*
 * process_kqueue() - process kqueue file
 *
 * Strictly speaking this function should appear in dfile.c, because it is
 * a file processing function.  However, the Net and Open BSD sources don't
 * require a dfile.c, so this is the next best location for the function.
 */

void
process_kqueue(ka)
	KA_T ka;			/* kqueue file structure address */
{
	struct kqueue kq;		/* kqueue structure */

	(void) snpf(Lf->type, sizeof(Lf->type), "KQUEUE");
	enter_dev_ch(print_kptr(ka, (char *)NULL, 0));
	if (!ka || kread(ka, (char *)&kq, sizeof(kq)))
	    return;
	(void) snpf(Namech, Namechl, "count=%d, state=%#x", kq.kq_count,
	    kq.kq_state);
	enter_nm(Namech);
}
#endif	/* defined(HASKQUEUE) */


/*
 * process_vnode() - process vnode
 */

void
process_vnode(struct kinfo_file *kf, struct xfile *xfile, struct xvnode *xvnode)
{
	dev_t dev = 0, rdev = 0;
	unsigned char devs;
	unsigned char rdevs;
	char dev_ch[32], *ep;
	struct inode *i;
	size_t sz;
	char *ty;
	unsigned char ums;
	KA_T va;
	struct vnode *v, vb;
	struct l_vfs *vfs;

	struct inode ib;
#   if	!defined(HAS_CONF_MINOR) && !defined(HAS_CDEV2PRIV)
	struct cdev si;
#   endif	/* !defined(HAS_CONF_MINOR) && !defined(HAS_CDEV2PRIV) */


# if	defined(HAS_UFS1_2)
	int ufst;
	struct ufsmount um;
	struct ufs1_dinode d1;
	struct ufs2_dinode d2;
# endif	/* !defined(HAS_UFS1_2) */

# if	!defined(HAS_CONF_MINOR) && !defined(HAS_CDEV2PRIV)
	struct cdev cd;
# endif	/* !defined(HAS_CONF_MINOR) && !defined(HAS_CDEV2PRIV) */

	int cds;
	struct devfs_dirent de;
	struct devfs_dirent *d;
	char vtbuf[32];
	char *vtbp;
	enum vtagtype { VT_DEVFS, VT_FDESC, VT_FUSEFS, VT_ISOFS, VT_PSEUDOFS,
			VT_NFS, VT_NULL, VT_TMPFS, VT_UFS, VT_ZFS, VT_MSDOSFS,
			VT_UNKNOWN
		      };

# if	defined(HAS_TMPFS)
	struct tmpfs_node tn;
	struct tmpfs_node *tnp;
# endif	/* defined(HAS_TMPFS) */

#if	defined(HASNULLFS)
# if	!defined(HASPRINTDEV)
	char dbuf[32];
# endif	/* !defined(HASPRINTDEV) */
	char *dp, *np, tbuf[1024];
	struct null_node nu;
	int sc = 0;
#endif	/* defined(HASNULLFS) */

#if	defined(HASPROCFS)
	struct pfsnode *p;
	struct procfsid *pfi;
	static int pgsz = -1;
	struct vmspace vm;

	struct pfsnode pb;
#endif	/* defined(HASPROCFS) */

#if	defined(HASPSEUDOFS)
	struct pfs_node pn;
	struct pfs_node *pnp;
#endif	/* defined(HASPSEUDOFS) */

	enum vtagtype vtag;			/* placed here to use the
						 * artificial vtagtype
						 * definition required for
						 * FREEBSDV>=5000 */

	struct stat st;
	const int kf_vtype = kf->kf_un.kf_file.kf_file_type;

	va = xfile ? xfile->xf_vnode : 0;

#if	defined(HASNULLFS)

process_overlaid_node:

	if (++sc > 1024) {
	    (void) snpf(Namech, Namechl, "too many overlaid nodes");
	    enter_nm(Namech);
	    return;
	}
#endif	/* defined(HASNULLFS) */

/*
 * Initialize miscellaneous variables.  This is done so that processing an
 * overlaid node will be a fresh start.
 */
	devs = rdevs = ums = 0;
	i = (struct inode *)NULL;
	Namech[0] = '\0';

	cds = 0;
	d = (struct devfs_dirent *)NULL;
# if	defined(HAS_UFS1_2)
	ufst = 0;
# endif	/* !defined(HAS_UFS1_2) */

#if	defined(HASPROCFS)
	p = (struct pfsnode *)NULL;
#endif	/* defined(HASPROCFS) */

#if	defined(HASPSEUDOFS)
	pnp = (struct pfs_node *)NULL;
#endif	/* defined(HASPSEUDOFS) */

# if	defined(HAS_TMPFS)
	tnp = (struct tmpfs_node *)NULL;
# endif	/* defined(HAS_TMPFS) */


/*
 * Read the vnode.
 */
	v = NULL;
	if (va) {
	    v = &vb;
	    if (kread((KA_T)va, (char *)v, sizeof(struct vnode)))
		v = NULL;
	}

	if (xvnode) {

#if	defined(HASNCACHE)
	    Lf->na = (KA_T)xvnode->xv_vnode;
# if	defined(HASNCVPID)
	    Lf->id = xvnode->xv_id; /* FIXME: always 0 in the kernel */
# endif	/* defined(HASNCVPID) */
#endif	/* defined(HASNCACHE) */

#if	defined(HASFSTRUCT)
	    Lf->fna = (KA_T)xvnode->xv_vnode;
	    Lf->fsv |= FSV_NI;
#endif	/* defined(HASFSTRUCT) */

	}

/*
 * Get the vnode type.
 */
	vfs = readvfs(xvnode ? (KA_T)xvnode->xv_mount : 0, kf->kf_path);
	if (vfs) {

#if	defined(MOUNT_NONE)
	    switch (vfs->type) {
	    case MOUNT_NFS:
		Ntype = N_NFS;
		break;

# if	defined(HASPROCFS)
	    case MOUNT_PROCFS:
		Ntype = N_PROC;
		break;
# endif	/* defined(HASPROCFS) */
	    }
#else	/* !defined(MOUNT_NONE) */
	    if (strcasecmp(vfs->typnm, "nfs") == 0)
		Ntype = N_NFS;

# if	defined(HASPROCFS)
	    else if (strcasecmp(vfs->typnm, "procfs") == 0)
		Ntype = N_PROC;
# endif	/* defined(HASPROCFS) */

# if	defined(HASPSEUDOFS)
	    else if (strcasecmp(vfs->typnm, "pseudofs") == 0)
		Ntype = N_PSEU;
# endif	/* defined(HASPSEUDOFS) */

# if	defined(HAS_TMPFS)
	    else if (strcasecmp(vfs->typnm, "tmpfs") == 0)
		Ntype = N_TMP;
# endif	/* defined(HAS_TMPFS) */
#endif	/* defined(MOUNT_NONE) */

	}
	if (Ntype == N_REGLR) {
	    switch (kf_vtype) {
	    case KF_VTYPE_VFIFO:
		Ntype = N_FIFO;
		break;
	    default:
		break;
	    }
	}

	/* FIXME: needs an API to retrieve v_lockf from the kernel without kvm access */
#if	defined(HAS_V_LOCKF)
	/* Commit eab626f110908f209587469de08f63bf8642aa68 first released in FreeBSD 8.0.0 */
	if (v && v->v_lockf)
	    (void) get_lock_state((KA_T)v->v_lockf);
#endif

	if (!v) return;

/*
 * For FreeBSD 5 and above VCHR and VBLK vnodes get the v_rdev structure.
 */
	if (((v->v_type == VCHR) || (v->v_type == VBLK))
	&&  v->v_rdev

# if	!defined(HAS_CONF_MINOR) && !defined(HAS_CDEV2PRIV)
	&&  !kread((KA_T)v->v_rdev, (char *)&cd, sizeof(cd))
# endif	/* !defined(HAS_CONF_MINOR) && !defined(HAS_CDEV2PRIV) */

	) {
	    cds = 1;
	}

/*
 * Define the specific node pointer.
 */

/*
 * Get the pseudo vnode tag type for FreeBSD >= 5.
 */
	vtag = VT_UNKNOWN;
	if (!kread((KA_T)v->v_lock.lock_object.lo_name, (char *)&vtbuf, sizeof(vtbuf)))
	{
	    vtbuf[sizeof(vtbuf) - 1] = '\0';
	    vtbp = vtbuf;
	    if (!strcmp(vtbuf, "ufs"))
		vtag = VT_UFS;
	    else if (!strcmp(vtbuf, "zfs")) {
		vtag = VT_ZFS;
	    } else if (!strcmp(vtbuf, "devfs"))
		vtag = VT_DEVFS;
	    else if (!strcmp(vtbuf, "nfs"))
		vtag = VT_NFS;
	    else if (!strcmp(vtbuf, "newnfs"))
		vtag = VT_NFS;
	    else if (!strcmp(vtbuf, "oldnfs"))
		vtag = VT_NFS;
	    else if (!strcmp(vtbuf, "isofs"))
		vtag = VT_ISOFS;
	    else if (!strcmp(vtbuf, "pseudofs"))
		vtag = VT_PSEUDOFS;
	    else if (!strcmp(vtbuf, "nullfs"))
		vtag = VT_NULL;
	    else if (!strcmp(vtbuf, "null"))
		vtag = VT_NULL;
	    else if (!strcmp(vtbuf, "fdesc"))
		vtag = VT_FDESC;
	    else if (!strcmp(vtbuf, "fuse"))
		vtag = VT_FUSEFS;
	    else if (!strcmp(vtbuf, "tmpfs"))
		vtag = VT_TMPFS;
	    else if (!strcmp(vtbuf, "msdosfs"))
		vtag = VT_MSDOSFS;
	} else
	    vtbp = "(unknown)";

	switch (vtag) {

	case VT_DEVFS:
	    if (!v->v_data
	    ||  kread((KA_T)v->v_data, (char *)&de, sizeof(de)))
	    {
		(void) snpf(Namech, Namechl, "no devfs node: %s",
		    print_kptr((KA_T)v->v_data, (char *)NULL, 0));
		enter_nm(Namech);
		return;
	    }
	    d = &de;
	    if (v->v_type == VDIR) {
		if (!d->de_dir
		||  kread((KA_T)d->de_dir, (char *)&de, sizeof(de))) {
		    (void) snpf(Namech, Namechl, "no devfs dir node: %s",
			print_kptr((KA_T)d->de_dir, (char *)NULL, 0));
		    enter_nm(Namech);
		    return;
		}
	    }
	    break;

#if	defined(HASNULLFS)
	case VT_NULL:
	    if (sc == 1) {

	    /*
	     * If this is the first null_node, enter a name addition containing
	     * the mounted-on directory, the file system name, and the device
	     * number.
	     */
		if (vfs && (vfs->dir || vfs->fsname || vfs->fsid.val[0])) {
		    if (vfs->fsid.val[0]) {

#if	defined(HASPRINTDEV)
			dp = HASPRINTDEV(Lf, &dev);
#else	/* !defined(HASPRINTDEV) */
			(void) snpf(dbuf, sizeof(dbuf) - 1, "%d,%d",
			    GET_MAJ_DEV(dev), GET_MIN_DEV(dev));
			dbuf[sizeof(dbuf) - 1] = '\0';
			dp = dbuf;
#endif	/* defined(HASPRINTDEV) */

		    } else
			dp = (char *)NULL;
		    (void) snpf(tbuf, sizeof(tbuf) - 1,
			"(nullfs%s%s%s%s%s%s%s)",
			(vfs && vfs->fsname) ? " " : "",
			(vfs && vfs->fsname) ? vfs->fsname : "",
			(vfs && vfs->dir) ? " on " : "",
			(vfs && vfs->dir) ? vfs->dir : "",
			(dp && vfs && vfs->dir) ? " (" : "",
			(dp && vfs && vfs->dir) ? dp : "",
			(dp && vfs && vfs->dir) ? ")" : "");
		    tbuf[sizeof(tbuf) - 1] = '\0';
		    np = tbuf;
		} else
		    np = "(nullfs)";
		(void) add_nma(np, (int)strlen(np));
	    }
	    if (!v->v_data
	    ||  kread((KA_T)v->v_data, (char *)&nu, sizeof(nu))) {
		(void) snpf(Namech, Namechl, "can't read null_node at: %s",
		    print_kptr((KA_T)v->v_data, (char *)NULL, 0));
		enter_nm(Namech);
		return;
	    }
	    if (!nu.null_lowervp) {
		(void) snpf(Namech, Namechl, "null_node overlays nothing");
		enter_nm(Namech);
		return;
	    }
	    va = (KA_T)nu.null_lowervp;
	    goto process_overlaid_node;
#endif	/* defined(HASNULLFS) */

#if	defined(HASPROCFS)
	case VT_PROCFS:

	    if (!v->v_data
	    ||  kread((KA_T)v->v_data, (char *)&pb, sizeof(pb))) {
		(void) snpf(Namech, Namechl, "no pfs node: %s",
		    print_kptr((KA_T)v->v_data, (char *)NULL, 0));
		enter_nm(Namech);
		return;
	    }
	    p = &pb;

	    break;
#endif	/* defined(HASPROCFS) */

#if	defined(HASPSEUDOFS)
	case VT_PSEUDOFS:
	    if (!v->v_data
	    ||  kread((KA_T)v->v_data, (char *)&pn, sizeof(pn))) {
		(void) snpf(Namech, Namechl, "no pfs_node: %s",
		    print_kptr((KA_T)v->v_data, (char *)NULL, 0));
		enter_nm(Namech);
		return;
	    }
	    pnp = &pn;
	    break;
#endif	/* defined(HASPSEUDOFS) */

# if	defined(HAS_TMPFS)
	case VT_TMPFS:
	    if (!v->v_data
	    ||  kread((KA_T)v->v_data, (char *)&tn, sizeof(tn))) {
		(void) snpf(Namech, Namechl, "no tmpfs_node: %s",
		    print_kptr((KA_T)v->v_data, (char *)NULL, 0));
		enter_nm(Namech);
		return;
	    }
	    tnp = &tn;
	    break;
# endif	/* defined(HAS_TMPFS) */

	case VT_UFS:

	    if (!v->v_data
	    ||  kread((KA_T)v->v_data, (char *)&ib, sizeof(ib))) {
		(void) snpf(Namech, Namechl, "no ufs node: %s",
		    print_kptr((KA_T)v->v_data, (char *)NULL, 0));
		enter_nm(Namech);
		return;
	    }
	    i = &ib;

# if	defined(HAS_UFS1_2)
	    if (i->i_ump && !kread((KA_T)i->i_ump, (char *)&um, sizeof(um))) {
		ums =  1;
		if (um.um_fstype == UFS1) {
		    if (i->i_din1
		    &&  !kread((KA_T)i->i_din1, (char *)&d1, sizeof(d1)))
			ufst = 1;
		} else {
		    if (i->i_din2
		    &&  !kread((KA_T)i->i_din2, (char *)&d2, sizeof(d2)))
			ufst = 2;
		}
	    }
# endif	/* defined(HAS_UFS1_2) */

	    break;

	default:
	    if (v->v_type == VBAD || v->v_type == VNON)
		break;

	    (void) snpf(Namech, Namechl, "unknown file system type: %s", vtbp);

	    enter_nm(Namech);
	    return;
	}
/*
 * Get device and type for printing.
 */
	if (kf->kf_un.kf_file.kf_file_fsid != VNOVAL) {
	    dev = kf->kf_un.kf_file.kf_file_fsid;
	    devs = 1;
	}
	if (kf_vtype == KF_VTYPE_VCHR || kf_vtype == KF_VTYPE_VBLK) {
	    rdev = kf->kf_un.kf_file.kf_file_rdev;
	    rdevs = 1;
	}

/*
 * Obtain the inode number.
 */
	if (kf->kf_un.kf_file.kf_file_fileid != VNOVAL) {
	    Lf->inode = kf->kf_un.kf_file.kf_file_fileid;
	    Lf->inp_ty = 1;
	}

/*
 * Obtain the file size.
 */
	if (Foffset)
	    Lf->off_def = 1;
	else {
	    switch (Ntype) {
	    case N_FIFO:
		if (!Fsize)
		    Lf->off_def = 1;
		break;
#if	defined(HASPROCFS)
	    case N_PROC:
		Lf->sz = kf->kf_un.kf_file.kf_file_size;
		Lf->sz_def = 1;
		break;
#endif	/* defined(HASPROCFS) */
#if	defined(HASPSEUDOFS)
	    case N_PSEU:
		Lf->sz = 0;
		Lf->sz_def = 1;
		break;
#endif	/* defined(PSEUDOFS) */
	    case N_REGLR:
# if	defined(HAS_TMPFS)
	    case N_TMP:
# endif	/* defined(HAS_TMPFS) */
		if (kf_vtype == KF_VTYPE_VREG || kf_vtype == KF_VTYPE_VDIR) {
		    Lf->sz = kf->kf_un.kf_file.kf_file_size;
		    Lf->sz_def = 1;
		} else if ((kf_vtype == KF_VTYPE_VCHR || kf_vtype == KF_VTYPE_VBLK) && !Fsize) {
		    Lf->off_def = 1;
		}
		break;
	    default:
		Lf->sz = kf->kf_un.kf_file.kf_file_size;
		Lf->sz_def = 1;
	    }
	}
/*
 * Record the link count.
 */
	if (Fnlink) {
	    /* FIXME: the kernel could provide this from the inode, without us needing a separate stat call */
	    if (kf->kf_path[0] && stat(kf->kf_path, &st) == 0) {
		Lf->nlink = st.st_nlink;
		Lf->nlink_def = 1;
	    }
	    if (Lf->nlink_def && Nlink && (Lf->nlink < Nlink))
		Lf->sf |= SELNLINK;
	}
/*
 * Record an NFS file selection.
 */
	if (Ntype == N_NFS && Fnfs)
	    Lf->sf |= SELNFS;
/*
 * Save the file system names.
 */
	if (vfs) {
	    Lf->fsdir = vfs->dir;
	    Lf->fsdev = vfs->fsname;
	}
/*
 * Save the device numbers and their states.
 *
 * Format the vnode type, and possibly the device name.
 */
	Lf->dev = dev;
	Lf->dev_def = devs;
	Lf->rdev = rdev;
	Lf->rdev_def = rdevs;
	switch (kf_vtype) {
	case KF_VTYPE_VNON:
	    ty ="VNON";
	    break;
	case KF_VTYPE_VREG:
	case KF_VTYPE_VDIR:
	    ty = (kf_vtype == KF_VTYPE_VREG) ? "VREG" : "VDIR";
	    break;
	case KF_VTYPE_VBLK:
	    ty = "VBLK";
	    Ntype = N_BLK;
	    break;
	case KF_VTYPE_VCHR:
	    ty = "VCHR";
	    Ntype = N_CHR;
	    break;
	case KF_VTYPE_VLNK:
	    ty = "VLNK";
	    break;
	case KF_VTYPE_VSOCK:
	    ty = "SOCK";
	    break;
	case KF_VTYPE_VBAD:
	    ty = "VBAD";
	    break;
	case KF_VTYPE_VFIFO:
	    ty = "FIFO";
	    break;
	default:
	     (void) snpf(Lf->type, sizeof(Lf->type), "%04o", (kf_vtype & 0xfff));
	     ty = (char *)NULL;
	}
	if (ty)
	    (void) snpf(Lf->type, sizeof(Lf->type), "%s", ty);
	Lf->ntype = Ntype;
/*
 * Handle some special cases:
 *
 * 	ioctl(fd, TIOCNOTTY) files;
 *	memory node files;
 *	/proc files.
 */

	if (kf_vtype == KF_VTYPE_VBAD)
	    (void) snpf(Namech, Namechl, "(revoked)");

#if	defined(HASPROCFS)
	else if (p) {
	    Lf->dev_def = Lf->rdev_def = 0;

	    ty = (char *)NULL;
	    (void) snpf(Namech, Namechl, "/%s", HASPROCFS);
	    switch (p->pfs_type) {
	    case Proot:
		ty = "PDIR";
		break;
	    case Pproc:
		ep = endnm(&sz);
		(void) snpf(ep, sz, "/%d", p->pfs_pid);
		ty = "PDIR";
		break;
	    case Pfile:
		ep = endnm(&sz);
		(void) snpf(ep, sz, "/%d/file", p->pfs_pid);
		ty = "PFIL";
		break;
	    case Pmem:
		ep = endnm(&sz);
		(void) snpf(ep, sz, "/%d/mem", p->pfs_pid);
		ty = "PMEM";
		break;
	    case Pregs:
		ep = endnm(&sz);
		(void) snpf(ep, sz, "/%d/regs", p->pfs_pid);
		ty = "PREG";
		break;
	    case Pfpregs:
		ep = endnm(&sz);
		(void) snpf(ep, sz, "/%d/fpregs", p->pfs_pid);
		ty = "PFPR";
		break;
	    case Pctl:
		ep = endnm(&sz);
		(void) snpf(ep, sz, "/%d/ctl", p->pfs_pid);
		ty = "PCTL";
		break;
	    case Pstatus:
		ep = endnm(&sz);
		(void) snpf(ep, sz, "/%d/status", p->pfs_pid);
		ty = "PSTA";
		break;
	    case Pnote:
		ep = endnm(&sz);
		(void) snpf(ep, sz, "/%d/note", p->pfs_pid);
		ty = "PNTF";
		break;
	    case Pnotepg:
		ep = endnm(&sz);
		(void) snpf(ep, sz, "/%d/notepg", p->pfs_pid);
		ty = "PGID";
		break;

	    case Pmap:
		ep = endnm(&sz);
		(void) snpf(ep, sz, "/%d/map", p->pfs_pid);
		ty = "PMAP";
		break;
	    case Ptype:
		ep = endnm(&sz);
		(void) snpf(ep, sz, "/%d/etype", p->pfs_pid);
		ty = "PETY";
		break;

	    }
	    if (ty)
		(void) snpf(Lf->type, sizeof(Lf->type), "%s", ty);
	    enter_nm(Namech);

	}
#endif	/* defined(HASPROCFS) */

#if	defined(HASBLKDEV)
/*
 * If this is a VBLK file and it's missing an inode number, try to
 * supply one.
 */
	if ((Lf->inp_ty == 0) && (kf_vtype == KF_VTYPE_VBLK))
	    find_bl_ino();
#endif	/* defined(HASBLKDEV) */

/*
 * If this is a VCHR file and it's missing an inode number, try to
 * supply one.
 */
	if ((Lf->inp_ty == 0) && (kf_vtype == KF_VTYPE_VCHR))
	    find_ch_ino();
/*
 * Test for specified file.
 */

#if	defined(HASPROCFS)
	if (Ntype == N_PROC) {
	    if (Procsrch) {
		Procfind = 1;
		Lf->sf |= SELNM;
	    } else {
		for (pfi = Procfsid; pfi; pfi = pfi->next) {
		    if ((pfi->pid && pfi->pid == p->pfs_pid)

# if	defined(HASPINODEN)
		    ||  (Lf->inp_ty == 1 && Lf->inode == pfi->inode)
# else	/* !defined(HASPINODEN) */
				if (pfi->pid == p->pfs_pid)
# endif	/* defined(HASPINODEN) */

		    ) {
			pfi->f = 1;
			if (!Namech[0])
			    (void) snpf(Namech, Namechl, "%s", pfi->nm);
			Lf->sf |= SELNM;
			break;
		    }
		}
	    }
	} else
#endif	/* defined(HASPROCFS) */

	{
	    if (Sfile && is_file_named((char *)NULL,
				       ((kf_vtype == KF_VTYPE_VCHR) || (kf_vtype == KF_VTYPE_VBLK)) ? 1
									  : 0))
		Lf->sf |= SELNM;
	}
/*
 * Enter name characters.
 */
	if (Namech[0])
	    enter_nm(Namech);
}


/*
 * process_pipe() - process a file structure whose type is DTYPE_PIPE
 */

void
process_pipe(struct kinfo_file *kf, KA_T pa)
{
	char dev_ch[32], *ep;
	struct pipe p;
	size_t sz;
	int have_kpipe;

	have_kpipe = (pa && kread(pa, (char *)&p, sizeof(p)) == 0);
	(void) snpf(Lf->type, sizeof(Lf->type), "PIPE");
	(void) snpf(dev_ch, sizeof(dev_ch), "%s",
	    print_kptr(kf->kf_un.kf_pipe.kf_pipe_addr, (char *)NULL, 0));
	enter_dev_ch(dev_ch);
	if (Foffset)
	    Lf->off_def = 1;
	else if (have_kpipe) {
	    /* FIXME: "struct kinfo_file" needs this */
	    Lf->sz = (SZOFFTYPE)p.pipe_buffer.size;
	    Lf->sz_def = 1;
	}
	if (kf->kf_un.kf_pipe.kf_pipe_peer)
	    (void) snpf(Namech, Namechl, "->%s",
		print_kptr((KA_T)kf->kf_un.kf_pipe.kf_pipe_peer, (char *)NULL, 0));
	else
	    Namech[0] = '\0';
	if (kf->kf_un.kf_pipe.kf_pipe_buffer_cnt) {
	    ep = endnm(&sz);
	    (void) snpf(ep, sz, ", cnt=%d", kf->kf_un.kf_pipe.kf_pipe_buffer_cnt);
	}
	/* FIXME: "struct kinfo_file" needs these */
	if (have_kpipe && p.pipe_buffer.in) {
	    ep = endnm(&sz);
	    (void) snpf(ep, sz, ", in=%d", p.pipe_buffer.in);
	}
	if (have_kpipe && p.pipe_buffer.out) {
	    ep = endnm(&sz);
	    (void) snpf(ep, sz, ", out=%d", p.pipe_buffer.out);
	}
/*
 * Enter name characters.
 */
	if (Namech[0])
	    enter_nm(Namech);
}


#if	defined(DTYPE_PTS)
/*
 * process_pts - process a file structure whose type is DTYPE_PTS
 */

void process_pts(struct kinfo_file *kf)
{
	(void) snpf(Lf->type, sizeof(Lf->type), "PTS");
/*
 * Convert the tty's cdev from kernel to user form.
 *     -- already done in the kernel, file sys/kern/tty_pts.c, function ptsdev_fill_kinfo().
 *
 * Set the device number to DevDev, the device number of /dev.
 *
 * Set the inode number to the device number.
 *
 * Set the file type to N_CHR for a character device (That's what a PTS is.)
 *
 * Force the use of offset from file structure.
 *
 * Set rdev to the converted device.
 *
 * Force the reloading of the device cache.
 */
	Lf->dev = DevDev;
	Lf->inode = (INODETYPE)kf->kf_un.kf_pts.kf_pts_dev;
	Lf->inp_ty = Lf->dev_def = Lf->rdev_def = 1;
	Lf->ntype = N_CHR;
	Lf->off_def = 1;
	Lf->rdev = kf->kf_un.kf_pts.kf_pts_dev;
	DCunsafe = 1;
}
#endif	/* defined(DTYPE_PTS) */
