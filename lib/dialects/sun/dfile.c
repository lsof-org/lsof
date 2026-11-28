/*
 * dfile.c - Solaris file processing functions for lsof
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

#include "common.h"

/*
 * Local structures
 */

/*
 * Local static variables
 */

/*
 * Local definitions
 */

#define SFCDHASH 1024 /* Sfile hash by clone device */
#define SFDIHASH                                                               \
    4094 /* Sfile hash by (device,inode) number                                \
          * pair bucket count (power of 2!) */
#define SFFSHASH                                                               \
    128 /* Sfile hash by file system device                                    \
         * number bucket count (power of 2!) */
#define SFHASHDEVINO(maj, min, ino, mod)                                       \
    ((int)(((int)((((int)(maj + 1)) * ((int)((min + 1)))) + ino) * 31415) &    \
           (mod - 1)))
/* hash for Sfile by major device,
 * minor device, and inode, modulo m
 * (m must be a power of 2) */
#define SFNMHASH                                                               \
    4096 /* Sfile hash by name bucket count                                    \
  (power of 2!) */
#define SFRDHASH                                                               \
    1024 /* Sfile hash by raw device number                                    \
          * bucket count (power of 2!) */
#define SFHASHRDEVI(maj, min, rmaj, rmin, ino, mod)                            \
    ((int)(((int)((((int)(maj + 1)) * ((int)((min + 1)))) +                    \
                  ((int)(rmaj + 1) * (int)(rmin + 1)) + ino) *                 \
            31415) &                                                           \
           (mod - 1)))
/* hash for Sfile by major device,
 * minor device, major raw device,
 * minor raw device, and inode, modulo
 * mod (mod must be a power of 2) */

/*
 * hashSfile() - hash Sfile entries for use in is_file_named() searches
 */

void hashSfile(struct lsof_context *ctx) {
    int cmaj, hvc, i;
    struct sfile *s;
    struct hsfile *sh, *sn;
    /*
     * Do nothing if there are no file search arguments cached or if the
     * hashes have already been constructed.
     */
    if (!Sfile || Hs)
        return;
    /*
     * Preset the clone major device for Solaris.
     */
    if (HaveCloneMaj) {
        cmaj = CloneMaj;
        hvc = 1;
    } else
        hvc = 0;
    /*
     * Allocate hash buckets by clone device, (device,inode), file system
     * device, and file name.
     */
    if (hvc) {
        if (!(HbyCd = (struct hsfile *)calloc((MALLOC_S)SFCDHASH,
                                              sizeof(struct hsfile)))) {
            (void)fprintf(
                stderr, "%s: can't allocate space for %d clone hash buckets\n",
                Pn, SFCDHASH);
            Error(ctx);
        }
    }
    if (!(HbyFdi = (struct hsfile *)calloc((MALLOC_S)SFDIHASH,
                                           sizeof(struct hsfile)))) {
        (void)fprintf(
            stderr, "%s: can't allocate space for %d (dev,ino) hash buckets\n",
            Pn, SFDIHASH);
        Error(ctx);
    }
    if (!(HbyFrd = (struct hsfile *)calloc((MALLOC_S)SFRDHASH,
                                           sizeof(struct hsfile)))) {
        (void)fprintf(stderr,
                      "%s: can't allocate space for %d rdev hash buckets\n", Pn,
                      SFRDHASH);
        Error(ctx);
    }
    if (!(HbyFsd = (struct hsfile *)calloc((MALLOC_S)SFFSHASH,
                                           sizeof(struct hsfile)))) {
        (void)fprintf(stderr,
                      "%s: can't allocate space for %d file sys hash buckets\n",
                      Pn, SFFSHASH);
        Error(ctx);
    }
    if (!(HbyNm = (struct hsfile *)calloc((MALLOC_S)SFNMHASH,
                                          sizeof(struct hsfile)))) {
        (void)fprintf(stderr,
                      "%s: can't allocate space for %d name hash buckets\n", Pn,
                      SFNMHASH);
        Error(ctx);
    }
    Hs++;
    /*
     * Scan the Sfile chain, building file, file system, and file name hash
     * bucket chains.
     */
    for (s = Sfile; s; s = s->next) {
        for (i = 0; i < 4; i++) {
            if (i == 0) {
                if (!s->aname)
                    continue;
                sh = &HbyNm[hashbyname(s->aname, SFNMHASH)];
                HbyNmCt++;
            } else if (i == 1) {
                if (s->type) {
                    sh = &HbyFdi[SFHASHDEVINO(GET_MAJ_DEV(s->dev),
                                              GET_MIN_DEV(s->dev), s->i,
                                              SFDIHASH)];
                    HbyFdiCt++;
                } else {
                    sh = &HbyFsd[SFHASHDEVINO(
                        GET_MAJ_DEV(s->dev), GET_MIN_DEV(s->dev), 0, SFFSHASH)];
                    HbyFsdCt++;
                }
            } else if (i == 2) {
                if (s->type && ((s->mode == S_IFCHR) || (s->mode == S_IFBLK))) {
                    sh = &HbyFrd[SFHASHRDEVI(
                        GET_MAJ_DEV(s->dev), GET_MIN_DEV(s->dev),
                        GET_MAJ_DEV(s->rdev), GET_MIN_DEV(s->rdev), s->i,
                        SFRDHASH)];
                    HbyFrdCt++;
                } else
                    continue;
            } else {
                if (!hvc || ((major_t)GET_MAJ_DEV(s->rdev) != (major_t)cmaj))
                    continue;
                sh = &HbyCd[SFHASHDEVINO(0, GET_MIN_DEV(s->rdev), 0, SFCDHASH)];
                HbyCdCt++;
            }
            if (!sh->s) {
                sh->s = s;
                sh->next = (struct hsfile *)NULL;
                continue;
            } else {
                if (!(sn = (struct hsfile *)malloc(
                          (MALLOC_S)sizeof(struct hsfile)))) {
                    (void)fprintf(stderr,
                                  "%s: can't allocate hsfile bucket for: %s\n",
                                  Pn, s->aname);
                    Error(ctx);
                }
                sn->s = s;
                sn->next = sh->next;
                sh->next = sn;
            }
        }
    }
}

/*
 * free_hashSfile() - free the Sfile hash buckets and chain entries
 */

void free_hashSfile(struct lsof_context *ctx) {
    int i;
    struct hsfile *sn;

    if (HbyCd) {
        for (i = 0; i < SFCDHASH; i++) {
            while ((sn = HbyCd[i].next)) {
                HbyCd[i].next = sn->next;
                CLEAN(sn);
            }
        }
        CLEAN(HbyCd);
    }
    HbyCdCt = 0;
    if (HbyFdi) {
        for (i = 0; i < SFDIHASH; i++) {
            while ((sn = HbyFdi[i].next)) {
                HbyFdi[i].next = sn->next;
                CLEAN(sn);
            }
        }
        CLEAN(HbyFdi);
    }
    HbyFdiCt = 0;
    if (HbyFrd) {
        for (i = 0; i < SFRDHASH; i++) {
            while ((sn = HbyFrd[i].next)) {
                HbyFrd[i].next = sn->next;
                CLEAN(sn);
            }
        }
        CLEAN(HbyFrd);
    }
    HbyFrdCt = 0;
    if (HbyFsd) {
        for (i = 0; i < SFFSHASH; i++) {
            while ((sn = HbyFsd[i].next)) {
                HbyFsd[i].next = sn->next;
                CLEAN(sn);
            }
        }
        CLEAN(HbyFsd);
    }
    HbyFsdCt = 0;
    if (HbyNm) {
        for (i = 0; i < SFNMHASH; i++) {
            while ((sn = HbyNm[i].next)) {
                HbyNm[i].next = sn->next;
                CLEAN(sn);
            }
        }
        CLEAN(HbyNm);
    }
    HbyNmCt = 0;
    Hs = 0;
}

/*
 * is_file_named() - is this file named?
 */

int is_file_named(struct lsof_context *ctx, /* context */
                  char *p,       /* path name; NULL = search by device
                                  * and inode (from *Lf) */
                  int nt,        /* node type -- e.g., N_* */
                  enum vtype vt, /* vnode type */
                  int ps)        /* print status: 0 = don't copy name
                                  * to Namech */
{
    char *ep;
    int f = 0;
    struct sfile *s;
    struct hsfile *sh;
    size_t sz;
    /*
     * Check for a path name match, as requested.
     */
    if (p && HbyNmCt) {
        for (sh = &HbyNm[hashbyname(p, SFNMHASH)]; sh; sh = sh->next) {
            if ((s = sh->s) && strcmp(p, s->aname) == 0) {
                f = 2;
                break;
            }
        }
    }
    /*
     * Check for a Solaris clone file.
     */
    if (!f && HbyCdCt && nt == N_STREAM && Lf->dev_def && Lf->rdev_def &&
        (Lf->dev == DevDev)) {
        for (sh = &HbyCd[SFHASHDEVINO(0, GET_MAJ_DEV(Lf->rdev), 0, SFCDHASH)];
             sh; sh = sh->next) {
            if ((s = sh->s) &&
                (GET_MAJ_DEV(Lf->rdev) == GET_MIN_DEV(s->rdev))) {
                f = 1;
                break;
            }
        }
    }
    /*
     * Check for a regular file.
     */
    if (!f && HbyFdiCt && Lf->dev_def && (Lf->inp_ty == 1 || Lf->inp_ty == 3)) {
        for (sh = &HbyFdi[SFHASHDEVINO(GET_MAJ_DEV(Lf->dev),
                                       GET_MIN_DEV(Lf->dev), Lf->inode,
                                       SFDIHASH)];
             sh; sh = sh->next) {
            if ((s = sh->s) && (Lf->dev == s->dev) && (Lf->inode == s->i)) {
                f = 1;
                break;
            }
        }
    }
    /*
     * Check for a file system match.
     */
    if (!f && HbyFsdCt && Lf->dev_def) {
        for (sh = &HbyFsd[SFHASHDEVINO(GET_MAJ_DEV(Lf->dev),
                                       GET_MIN_DEV(Lf->dev), 0, SFFSHASH)];
             sh; sh = sh->next) {
            if ((s = sh->s) && Lf->dev == s->dev) {
                f = 1;
                break;
            }
        }
    }
    /*
     * Check for a character or block device match.
     */
    if (!f && HbyFrdCt && ((vt == VCHR) || (vt == VBLK)) && Lf->dev_def &&
        (Lf->dev == DevDev) && Lf->rdev_def &&
        (Lf->inp_ty == 1 || Lf->inp_ty == 3)) {
        for (sh = &HbyFrd[SFHASHRDEVI(
                 GET_MAJ_DEV(Lf->dev), GET_MIN_DEV(Lf->dev),
                 GET_MAJ_DEV(Lf->rdev), GET_MIN_DEV(Lf->rdev), Lf->inode,
                 SFRDHASH)];
             sh; sh = sh->next) {
            if ((s = sh->s) && (s->dev == Lf->dev) && (s->rdev == Lf->rdev) &&
                (s->i == Lf->inode)) {
                f = 1;
                break;
            }
        }
    }
    /*
     * Convert the name if a match occurred.
     */
    if (f) {
        if (f == 2) {
            if (ps)
                (void)snpf(Namech, Namechl, "%s", p);
        } else {
            if (ps && s->type) {

                /*
                 * If the search argument isn't a file system, propagate it
                 * to Namech[]; otherwise, let printname() compose the name.
                 */
                (void)snpf(Namech, Namechl, "%s", s->name);
                if (s->devnm) {
                    ep = endnm(ctx, &sz);
                    (void)snpf(ep, sz, " (%s)", s->devnm);
                }
            }
        }
        s->f = 1;
        return (1);
    }
    return (0);
}



#if defined(HASIPv6)
/*
 * gethostbyname2() -- an RFC2133-compatible get-host-by-name-two function
 *                     to get AF_INET and AF_INET6 addresses from host names,
 *                     using the RFC2553-compatible getipnodebyname() function
 */

extern struct hostent *gethostbyname2(nm, prot) const char *nm; /* host name */
int prot; /* protocol -- AF_INET or AF_INET6 */
{
    int err;
    static struct hostent *hep = (struct hostent *)NULL;

    if (hep)
        (void)freehostent(hep);
    return ((hep = getipnodebyname(nm, prot, 0, &err)));
}
#endif /* defined(HASIPv6) */
