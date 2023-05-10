/*
 * dmnt.c - NetBSD mount support functions for lsof
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

#if defined(NETBSDV) && defined(HASSTATVFS)
/*
 * NetBSD needs the statvfs structure to be defined without the
 * pre-definition of _KERNEL.
 */

#    include <sys/statvfs.h>
#endif /* defined(NETBSDV) && defined(HASSTATVFS) */

#if defined(NETBSDV)
#    include <sys/param.h>

#    if __NetBSD_Version__ >= 899000100
#        define __EXPOSE_MOUNT
#    endif
#    if __NetBSD_Version__ >= 399002400
#        include <sys/types.h>
#        include <sys/mount.h>
#    endif
#endif

#include "common.h"

/*
 * Local static definitions
 */

/*
 * readmnt() - read mount table
 */

struct mounts *readmnt(struct lsof_context *ctx) {
    char *dn = (char *)NULL;
    char *ln;
    struct mounts *mtp;
    int n;
    struct stat sb;

#if defined(HASPROCFS)
    unsigned char procfs = 0;
#endif /* defined(HASPROCFS) */

#if defined(HASSTATVFS)
    struct statvfs *mb = (struct statvfs *)NULL;
#else  /* !defined(HASSTATVFS) */
    struct statfs *mb = (struct statfs *)NULL;
#endif /* defined(HASSTATVFS) */

    if (Lmi || Lmist)
        return (Lmi);
    /*
     * Access mount information.
     */
    if ((n = getmntinfo(&mb, MNT_NOWAIT)) <= 0) {
        (void)fprintf(stderr, "%s: no mount information\n", Pn);
        return (0);
    }
    /*
     * Read mount information.
     */
    for (; n; n--, mb++) {
        if (mb->f_fstypename[0] == '\0')
            continue;
#if defined(NETBSDV) && __NetBSD_Version__ >= 499002500
        /* MFSNAMELEN was removed from the kernel source after 4.99.24 */
        mb->f_fstypename[sizeof(mb->f_fstypename) - 1] = '\0';
#else
        mb->f_fstypename[MFSNAMELEN - 1] = '\0';
#endif
        /*
         * Interpolate a possible symbolic directory link.
         */
        if (dn)
            (void)free((FREE_P *)dn);
        if (!(dn = mkstrcpy(mb->f_mntonname, (MALLOC_S *)NULL))) {

        no_space_for_mount:

            (void)fprintf(stderr, "%s: no space for mount at ", Pn);
            safestrprt(mb->f_mntonname, stderr, 0);
            (void)fprintf(stderr, " (");
            safestrprt(mb->f_mntfromname, stderr, 0);
            (void)fprintf(stderr, ")\n");
            Error(ctx);
        }
        if ((ln = Readlink(ctx, dn)) == NULL) {
            if (!Fwarn) {
                (void)fprintf(stderr,
                              "      Output information may be incomplete.\n");
            }
            continue;
        }
        if (ln != dn) {
            (void)free((FREE_P *)dn);
            dn = ln;
        }
        if (*dn != '/')
            continue;
        /*
         * Stat() the directory.
         */
        if (statsafely(ctx, dn, &sb)) {
            if (!Fwarn) {
                (void)fprintf(stderr, "%s: WARNING: can't stat() ", Pn);
                safestrprt(mb->f_fstypename, stderr, 0);
                (void)fprintf(stderr, " file system ");
                safestrprt(mb->f_mntonname, stderr, 1);
                (void)fprintf(stderr,
                              "      Output information may be incomplete.\n");
            }
            (void)bzero((char *)&sb, sizeof(sb));

#if defined(HASSTATVFS)
            sb.st_dev = (dev_t)mb->f_fsid;
#else  /* !defined(HASSTATVFS) */
            sb.st_dev = (dev_t)mb->f_fsid.val[0];
#endif /* defined(HASSTATVFS) */

            sb.st_mode = S_IFDIR | 0777;
            if (!Fwarn) {
                (void)fprintf(stderr,
                              "      assuming \"dev=%x\" from mount table\n",
                              sb.st_dev);
            }
        }
        /*
         * Allocate and fill a local mount structure.
         */
        if (!(mtp = (struct mounts *)malloc(sizeof(struct mounts))))
            goto no_space_for_mount;
        mtp->dir = dn;
        dn = (char *)NULL;

#if defined(HASPROCFS)
        if (strcmp(mb->f_fstypename, MOUNT_PROCFS) == 0) {

            /*
             * Save information on exactly one procfs file system.
             */
            if (procfs)
                Mtprocfs = (struct mounts *)NULL;
            else {
                procfs = 1;
                Mtprocfs = mtp;
            }
        }
#endif /* defined(HASPROCFS) */

        mtp->next = Lmi;
        mtp->dev = sb.st_dev;
        mtp->rdev = sb.st_rdev;
        mtp->inode = (INODETYPE)sb.st_ino;
        mtp->mode = sb.st_mode;
        /*
         * Interpolate a possible file system (mounted-on) device name link.
         */
        if (!(dn = mkstrcpy(mb->f_mntfromname, (MALLOC_S *)NULL)))
            goto no_space_for_mount;
        mtp->fsname = dn;
        ln = Readlink(ctx, dn);
        dn = (char *)NULL;
        /*
         * Stat() the file system (mounted-on) name and add file system
         * information to the local mount table entry.
         */
        if (!ln || statsafely(ctx, ln, &sb))
            sb.st_mode = 0;
        mtp->fsnmres = ln;
        mtp->fs_mode = sb.st_mode;
        Lmi = mtp;
    }
    /*
     * Clean up and return local mount info table address.
     */
    if (dn)
        (void)free((FREE_P *)dn);
    Lmist = 1;
    return (Lmi);
}

/*
 * readvfs() - read vfs structure
 */

struct l_vfs *readvfs(struct lsof_context *ctx, /* context */
                      KA_T vm) /* kernel mount address from vnode */
{
    struct mount m;
    struct l_vfs *vp;
    /*
     * Search for match on existing entry.
     */
    for (vp = Lvfs; vp; vp = vp->next) {
        if (vm == vp->addr)
            return (vp);
    }
    /*
     * Read the (new) mount structure, allocate a local entry, and fill it.
     */
    if (kread(ctx, vm, (char *)&m, sizeof(m)) != 0)
        return ((struct l_vfs *)NULL);
    if (!(vp = (struct l_vfs *)malloc(sizeof(struct l_vfs)))) {
        (void)fprintf(stderr, "%s: PID %d, no space for vfs\n", Pn, Lp->pid);
        Error(ctx);
    }
    if (!(vp->dir = mkstrcpy(m.m_stat.f_mntonname, (MALLOC_S *)NULL)) ||
        !(vp->fsname = mkstrcpy(m.m_stat.f_mntfromname, (MALLOC_S *)NULL))) {
        (void)fprintf(stderr, "%s: PID %d, no space for mount names\n", Pn,
                      Lp->pid);
        Error(ctx);
    }
    vp->addr = vm;

#if defined(HASSTATVFS)
    vp->fsid = m.m_stat.f_fsidx;
#else  /* !defined(HASSTATVFS) */
    vp->fsid = m.m_stat.f_fsid;
#endif /* defined(HASSTATVFS) */

    (void)snpf(vp->type, sizeof(vp->type), "%s", m.m_stat.f_fstypename);
    vp->next = Lvfs;
    Lvfs = vp;
    return (vp);
}
