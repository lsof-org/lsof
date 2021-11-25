/*
 * dnode1.c - FreeBSD node functions for lsof
 *
 * This module must be separate to keep separate the multiple kernel inode
 * structure definitions.
 */


/*
 * Copyright 1995 Purdue Research Foundation, West Lafayette, Indiana
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

#if	defined(HAS9660FS)

/*
 * Do a little preparation for #include'ing cd9660_node.h, then #include it.
 */

#undef	i_size
#undef	doff_t
#undef	IN_ACCESS

#  if	defined(__alpha__)
#define	dev_t	void *
#  endif	/* defined(__alpha__) */

# if	defined(HAS_NO_ISO_DEV)
#define	_KERNEL
#include <isofs/cd9660/iso.h>
#undef	_KERNEL
# endif	/* defined(HAS_NO_ISO_DEV) */

#include "cd9660_node.h"

#  if	defined(__alpha__)
#undef	dev_t
#  endif	/* defined(__alpha__) */


/*
 * read_iso_node() -- read CD 9660 iso_node
 */

int
read_iso_node(v, d, dd, ino, nl, sz)
	struct vnode *v;		/* containing vnode */
	dev_t *d;			/* returned device number */
	int *dd;			/* returned device-defined flag */
	INODETYPE *ino;			/* returned inode number */
	long *nl;			/* returned number of links */
	SZOFFTYPE *sz;			/* returned size */
{

	struct iso_node i;

	struct cdev udev;
#   if	defined(HAS_NO_ISO_DEV)
	struct iso_mnt im;
#   endif	/* defined(HAS_NO_ISO_DEV) */

	if (!v->v_data
	||  kread((KA_T)v->v_data, (char *)&i, sizeof(i)))
	    return(1);

#  if	defined(HAS_NO_ISO_DEV)
	if (i.i_mnt && !kread((KA_T)i.i_mnt, (char *)&im, sizeof(im))
	&&  im.im_dev && !kread((KA_T)im.im_dev, (char *)&udev, sizeof(udev)))
#  else	/* !defined(HAS_NO_ISO_DEV) */
	if (i.i_dev && !kread((KA_T)i.i_dev, (char *)&udev, sizeof(udev)))
#  endif	/* defined(HAS_NO_ISO_DEV) */

	{

# if	defined(HAS_NO_SI_UDEV)
#   if	defined(HAS_CONF_MINOR) || defined(HAS_CDEV2PRIV)
	    *d = Dev2Udev((KA_T)im.im_dev);
#   else	/* !defined(HAS_CONF_MINOR) && !defined(HAS_CDEV2PRIV) */
	    *d = Dev2Udev(&udev);
#   endif	/* defined(HAS_CONF_MINOR) || defined(HAS_CDEV2PRIV) */
# else	/* !defined(HAS_NO_SI_UDEV) */
	    *d = udev.si_udev;
# endif	/* defined(HAS_NO_SI_UDEV) */

	    *dd = 1;
	}

	*ino = (INODETYPE)i.i_number;
	*nl = (long)i.inode.iso_links;
	*sz = (SZOFFTYPE)i.i_size;

	return(0);
}
#endif	/* defined(HAS9660FS) */


#if	defined(HASFUSEFS)
#undef VTOI
#include <fs/fuse/fuse_node.h>
/*
 * read_fuse_node() -- read FUSE file system fuse_node
 */

int
read_fuse_node(v, d, dd, ino, nl, sz)
	struct vnode *v;		/* containing vnode */
	dev_t *d;			/* returned device number */
	int *dd;			/* returned device-defined flag */
	INODETYPE *ino;			/* returned inode number */
	long *nl;			/* returned number of links */
	SZOFFTYPE *sz;			/* returned size */
{
	struct fuse_vnode_data fn;	/* FUSE node */

	if (!v->v_data
	||  kread((KA_T)v->v_data, (char *)&fn, sizeof(fn)))
	    return(1);
	*d = fn.cached_attrs.va_fsid;
	*dd = 1;
	*ino = (INODETYPE)fn.cached_attrs.va_fileid;
	*nl = (long)fn.cached_attrs.va_nlink;
	*sz = (SZOFFTYPE)fn.cached_attrs.va_size;
	return(0);
}
#endif	/* defined(HASFUSEFS) */


#if     defined(HASMSDOSFS)
#include <fs/msdosfs/bpb.h>
#include <fs/msdosfs/direntry.h>
#include <fs/msdosfs/denode.h>
#include <fs/msdosfs/fat.h>
#define _KERNEL
#include <fs/msdosfs/msdosfsmount.h>
#undef _KERNEL
/*
 * read_msdosfs_node() -- read msdos file system denode
 */

int
read_msdos_node(v, d, dd, ino, nl, sz)
	struct vnode *v;		/* containing vnode */
	dev_t *d;			/* returned device number */
	int *dd;			/* returned device-defined flag */
	INODETYPE *ino;			/* returned inode number */
	long *nl;			/* returned number of links */
	SZOFFTYPE *sz;			/* returned size */
{
	struct denode dep;		/* MSDOSFS node */
	struct msdosfsmount pmp;
	struct cdev udev;
	typedef __uint64_t uoff_t;
	uint64_t fileid;
	u_long dirsperblk;

	if (!v->v_data
	||  kread((KA_T)v->v_data, (char *)&dep, sizeof(dep)))
	    return(1);
	if (!dep.de_pmp || kread((KA_T)dep.de_pmp, (char *)&pmp, sizeof(pmp)))
	    return(1);

#  if   defined(HAS_NO_SI_UDEV)
#   if  defined(HAS_CONF_MINOR) || defined(HAS_CDEV2PRIV)
	*d = Dev2Udev((KA_T)pmp.pm_dev);
#   else	/* !defined(HAS_CONF_MINOR) && !defined(HAS_CDEV2PRIV) */
	if (!pmp.pm_dev || kread((KA_T)pmp.pm_dev, (char *)&udev, sizeof(udev)))
	    return(1);
	*d = Dev2Udev(&udev);
#   endif	/* defined(HAS_CONF_MINOR) || defined(HAS_CDEV2PRIV) */
#   else	/* !defined(HAS_NO_SI_UDEV) */
	if (!pmp.pm_dev || kread((KA_T)pmp.pm_dev, (char *)&udev, sizeof(udev)))
	    return(1);
	*d = udev.si_udev;
#  endif	/* defined(HAS_NO_SI_UDEV) */


	dirsperblk = pmp.pm_BytesPerSec / sizeof(struct direntry);
	/*
	 * The following computation of the fileid must be the same as that
	 * used in msdosfs_readdir() to compute d_fileno. If not, pwd
	 * doesn't work.
	 */
	if (dep.de_Attributes & ATTR_DIRECTORY) {
		fileid = (uint64_t)cntobn(&pmp, dep.de_StartCluster) *
		    dirsperblk;
		if (dep.de_StartCluster == MSDOSFSROOT)
			fileid = 1;
	} else {
		fileid = (uint64_t)cntobn(&pmp, dep.de_dirclust) *
		    dirsperblk;
		if (dep.de_dirclust == MSDOSFSROOT)
			fileid = (uint64_t)roottobn(&pmp, 0) * dirsperblk;
		fileid += (uoff_t)dep.de_diroffset / sizeof(struct direntry);
	}

	*dd = 1;
	*ino = (INODETYPE)fileid;
	*nl = 1;
	*sz = dep.de_FileSize;
	return(0);
}
#endif  /* defined(HASMSDOSFS) */
