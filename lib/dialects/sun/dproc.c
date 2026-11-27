/*
 * dproc.c - Solaris /proc process access
 *
 * This backend deliberately uses only procfs.  It does not open /dev/kmem
 * or use libkvm.
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


#define _STRUCTURED_PROC 1
#include <sys/procfs.h>

#include "common.h"

#include <limits.h>

#include <dirent.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <sys/sysmacros.h>
#include <sys/stat.h>


#define PROC_BASE "/proc"

static int read_psinfo(const char *path, psinfo_t *ps) {
    int fd, n;

    if ((fd = open(path, O_RDONLY)) < 0)
        return (1);
    n = (int)read(fd, ps, sizeof(*ps));
    (void)close(fd);
    return (n == (int)sizeof(*ps)) ? 0 : 1;
}

static int nm2id(char *nm, int *id) {
    char *ep;
    long v;

    if (!nm || !*nm)
        return (1);
    v = strtol(nm, &ep, 10);
    if (!ep || *ep || v < 0 || v > INT_MAX)
        return (1);
    *id = (int)v;
    return (0);
}

/*
 * /proc/<pid>/fdinfo/nn is a variable-length prfdinfo_t followed by
 * self-describing pr_misc entries.  The layout is from Solaris/illumos
 * procfs; it is userland ABI and does not require kernel memory.
 */
#define SUN_FDINFO_HDR 200
#define SUN_FDINFO_CORE 196
#define SUN_MISC_PATHNAME 0
#define SUN_MISC_SOCKETNAME 1
#define SUN_MISC_PEERSOCKNAME 2

struct sun_fdinfo {
    int pr_fd;
    mode_t pr_mode;
    uint64_t pr_ino;
    int64_t pr_size;
    int64_t pr_offset;
    uid_t pr_uid;
    gid_t pr_gid;
    uint32_t pr_major;
    uint32_t pr_minor;
    uint32_t pr_rmajor;
    uint32_t pr_rminor;
    int pr_fileflags;
    int pr_fdflags;
    short pr_locktype;
    pid_t pr_lockpid;
    int pr_locksysid;
    pid_t pr_peerpid;
    int pr_filler[25];
    char pr_peername[16];
};

static void enter_path(struct lsof_context *ctx, enum lsof_fd_type fdt,
                       int fd, const char *path, const struct stat *st);

struct sun_sockaddr_un {
    unsigned short sun_family;
    char sun_path[108];
};

static void set_sockaddr(struct lfile *lf, int which, const void *p, size_t n,
                         char *unixpath, size_t unixpathlen) {
    const struct sockaddr *sa = (const struct sockaddr *)p;

    if (n < sizeof(sa_family_t))
        return;
    switch (sa->sa_family) {
    case AF_INET: {
        const struct sockaddr_in *sin = (const struct sockaddr_in *)p;
        if (n < sizeof(*sin))
            return;
        lf->li[which].af = AF_INET;
        lf->li[which].p = ntohs(sin->sin_port);
        lf->li[which].ia.a4 = sin->sin_addr;
        break;
    }
#if defined(AF_INET6)
    case AF_INET6: {
        const struct sockaddr_in6 *sin6 = (const struct sockaddr_in6 *)p;
        if (n < sizeof(*sin6))
            return;
        lf->li[which].af = AF_INET6;
        lf->li[which].p = ntohs(sin6->sin6_port);
        lf->li[which].ia.a6 = sin6->sin6_addr;
        break;
    }
#endif
#if defined(AF_UNIX)
    case AF_UNIX: {
        const struct sun_sockaddr_un *unp = (const struct sun_sockaddr_un *)p;
        size_t l;

        lf->li[which].af = AF_UNIX;
        if (n <= offsetof(struct sun_sockaddr_un, sun_path))
            break;
        l = n - offsetof(struct sun_sockaddr_un, sun_path);
        if (l >= sizeof(unp->sun_path))
            l = sizeof(unp->sun_path) - 1;
        if (unixpath && unixpathlen && l && unp->sun_path[0]) {
            if (l >= unixpathlen)
                l = unixpathlen - 1;
            memcpy(unixpath, unp->sun_path, l);
            unixpath[l] = '\0';
        }
        break;
    }
#endif
    default:
        break;
    }
}

static int read_fdinfo(const char *p, struct sun_fdinfo *fi, char *path,
                       size_t pathlen, unsigned char *sn, size_t *snl,
                       unsigned char *pn, size_t *pnl) {
    unsigned char buf[8192];
    int fd, n;
    size_t off;

    *snl = *pnl = 0;
    path[0] = '\0';
    if ((fd = open(p, O_RDONLY)) < 0)
        return (1);
    n = (int)read(fd, buf, sizeof(buf));
    (void)close(fd);
    if (n < SUN_FDINFO_HDR)
        return (1);
    memcpy(fi, buf, sizeof(*fi));
    /*
     * The fixed core is 196 bytes.  Some Solaris fdinfo records have four
     * bytes of alignment before the misc array; detect which by validating
     * the first misc header.
     */
    off = SUN_FDINFO_CORE;
    if (off + 8 <= (size_t)n) {
        uint32_t sz, ty;
        memcpy(&sz, &buf[off], sizeof(sz));
        memcpy(&ty, &buf[off + 4], sizeof(ty));
        if (sz < 8 || ty > SUN_MISC_PEERSOCKNAME)
            off = SUN_FDINFO_HDR;
    }
    while (off + 8 <= (size_t)n) {
        uint32_t sz, ty;
        unsigned char *data;
        size_t dlen, adv;

        memcpy(&sz, &buf[off], sizeof(sz));
        memcpy(&ty, &buf[off + 4], sizeof(ty));
        if (sz < 8 || off + sz > (size_t)n)
            break;
        data = &buf[off + 8];
        dlen = sz - 8;
        switch (ty) {
        case SUN_MISC_PATHNAME:
            if (dlen && pathlen) {
                size_t l = dlen < pathlen - 1 ? dlen : pathlen - 1;
                memcpy(path, data, l);
                path[l] = '\0';
            }
            break;
        case SUN_MISC_SOCKETNAME:
            if (dlen <= 256) {
                memcpy(sn, data, dlen);
                *snl = dlen;
            }
            break;
        case SUN_MISC_PEERSOCKNAME:
            if (dlen <= 256) {
                memcpy(pn, data, dlen);
                *pnl = dlen;
            }
            break;
        default:
            break;
        }
        adv = (sz + 3) & ~3U;
        if (adv < sz || off + adv > (size_t)n)
            break;
        off += adv;
    }
    return (0);
}

static void set_lock_from_proc(const char *path, struct lfile *lf,
                             int locktype, SZOFFTYPE size) {
#if defined(F_GETLK)
    struct flock fl;
    int fd;

    if (locktype == F_UNLCK)
        return;
    if (locktype == F_RDLCK)
        lf->lock = LSOF_LOCK_READ_FULL;
    else
        lf->lock = LSOF_LOCK_WRITE_FULL;
    if ((fd = open(path, O_RDONLY)) < 0)
        return;
    (void)memset(&fl, 0, sizeof(fl));
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;
    if (fcntl(fd, F_GETLK, &fl) == 0 && fl.l_type != F_UNLCK) {
        int full = (fl.l_start == 0 &&
                    (fl.l_len == 0 || fl.l_len >= (off_t)size));
        if (fl.l_type == F_RDLCK)
            lf->lock = full ? LSOF_LOCK_READ_FULL : LSOF_LOCK_READ_PARTIAL;
        else if (fl.l_type == F_WRLCK)
            lf->lock = full ? LSOF_LOCK_WRITE_FULL : LSOF_LOCK_WRITE_PARTIAL;
    }
    (void)close(fd);
#else
    (void)path;
    (void)lf;
    (void)locktype;
    (void)size;
#endif
}

static void process_fdinfo(struct lsof_context *ctx, int pid, int fd,
                           const char *fdpath, const char *fipath,
                           const char *pathpath) {
    char nm[MAXPATHLEN + 1], path[MAXPATHLEN + 1];
    char unixpath[MAXPATHLEN + 1];
    unsigned char sock[256], peer[256];
    size_t sl, pl;
    struct sun_fdinfo fi;
    struct stat st;
    ssize_t r;
    int has_sock, has_peer;

    if (read_fdinfo(fipath, &fi, path, sizeof(path), sock, &sl, peer, &pl))
        return;
    has_sock = has_peer = 0;
    unixpath[0] = '\0';

    if (stat(fdpath, &st) != 0) {
        memset(&st, 0, sizeof(st));
        st.st_mode = fi.pr_mode;
        st.st_ino = (ino_t)fi.pr_ino;
        st.st_size = (off_t)fi.pr_size;
        st.st_nlink = 1;
        st.st_dev = (dev_t)(((uint64_t)fi.pr_major << 18) | fi.pr_minor);
        st.st_rdev = (dev_t)(((uint64_t)fi.pr_rmajor << 18) | fi.pr_rminor);
    }

    if (path[0])
        (void)snprintf(nm, sizeof(nm), "%s", path);
    else {
        r = readlink(pathpath, nm, sizeof(nm) - 1);
        if (r < 0)
            nm[0] = '\0';
        else
            nm[r] = '\0';
    }
    enter_path(ctx, LSOF_FD_NUMERIC, fd, nm[0] ? nm : fdpath, &st);
    if (!Lf)
        return;

    if (fi.pr_offset >= 0) {
        Lf->off = (SZOFFTYPE)fi.pr_offset;
        Lf->off_def = 1;
    }
    switch (fi.pr_fileflags & O_ACCMODE) {
    case O_RDONLY:
        Lf->access = LSOF_FILE_ACCESS_READ;
        break;
    case O_WRONLY:
        Lf->access = LSOF_FILE_ACCESS_WRITE;
        break;
    default:
        Lf->access = LSOF_FILE_ACCESS_READ_WRITE;
        break;
    }

    if (fi.pr_locktype != F_UNLCK)
        set_lock_from_proc(nm[0] ? nm : fdpath, Lf, fi.pr_locktype,
                           (SZOFFTYPE)st.st_size);


    if ((fi.pr_mode & S_IFMT) == S_IFSOCK) {
        if (sl) {
            (void)set_sockaddr(Lf, 0, sock, sl, unixpath, sizeof(unixpath));
            has_sock = 1;
        }
        if (pl) {
            (void)set_sockaddr(Lf, 1, peer, pl, (char *)NULL, 0);
            has_peer = 1;
        }
        if (unixpath[0])
            enter_nm(ctx, unixpath);
        else if (Lf->nm) {
            free(Lf->nm);
            Lf->nm = (char *)NULL;
        }
        Lf->ntype = Ntype = N_REGLR;
        if (has_sock || has_peer) {
            switch (Lf->li[0].af) {
            case AF_INET:
                Lf->type = LSOF_FILE_IPV4;
                break;
#if defined(AF_INET6)
            case AF_INET6:
                Lf->type = LSOF_FILE_IPV6;
                break;
#endif
#if defined(AF_UNIX)
            case AF_UNIX:
                Lf->type = LSOF_FILE_UNIX;
                break;
#endif
            default:
                Lf->type = LSOF_FILE_SOCKET;
                break;
            }
            Lf->lts.type = 1;
            if (Selflags & SELNET)
                Lf->sf |= SELNET;
            if (Selflags & SELUNX)
                Lf->sf |= SELUNX;
#if defined(SELNA)
            if ((Selflags & SELNA) && Nwad) {
                unsigned char *la = (unsigned char *)NULL;
                unsigned char *fa = (unsigned char *)NULL;
                if (Lf->li[0].af == AF_INET)
                    la = (unsigned char *)&Lf->li[0].ia.a4;
#if defined(AF_INET6)
                else if (Lf->li[0].af == AF_INET6)
                    la = (unsigned char *)&Lf->li[0].ia.a6;
#endif
                if (Lf->li[1].af == AF_INET)
                    fa = (unsigned char *)&Lf->li[1].ia.a4;
#if defined(AF_INET6)
                else if (Lf->li[1].af == AF_INET6)
                    fa = (unsigned char *)&Lf->li[1].ia.a6;
#endif
                ent_inaddr(ctx, la, Lf->li[0].p, fa, Lf->li[1].p,
                           Lf->li[0].af ? Lf->li[0].af : Lf->li[1].af);
            }
#endif
        }
    }
    if (Lf && Lf->sf)
        link_lfile(ctx);
}


static void enter_path(struct lsof_context *ctx, enum lsof_fd_type fdt,
                       int fd, const char *path, const struct stat *st) {
    enum lsof_file_type ft;
    enum vtype vt;
    int ntype;

    alloc_lfile(ctx, fdt, fd);
    Lf->dev = st->st_dev;
    Lf->dev_def = 1;
    Lf->inode = (INODETYPE)st->st_ino;
    Lf->inp_ty = 1;
    Lf->nlink = (long)st->st_nlink;
    Lf->nlink_def = 1;
    Lf->sz = (SZOFFTYPE)st->st_size;
    Lf->sz_def = 1;
    Lf->access = LSOF_FILE_ACCESS_READ_WRITE;

    switch (st->st_mode & S_IFMT) {
    case S_IFREG:
        ft = LSOF_FILE_REGULAR;
        vt = VREG;
        ntype = N_REGLR;
        break;
    case S_IFDIR:
        ft = LSOF_FILE_DIR;
        vt = VDIR;
        ntype = N_REGLR;
        break;
    case S_IFCHR:
        ft = LSOF_FILE_CHAR;
        vt = VCHR;
        ntype = N_REGLR;
        Lf->rdev = st->st_rdev;
        Lf->rdev_def = 1;
        break;
    case S_IFBLK:
        ft = LSOF_FILE_BLOCK;
        vt = VBLK;
        ntype = N_REGLR;
        Lf->rdev = st->st_rdev;
        Lf->rdev_def = 1;
        break;
    case S_IFIFO:
        ft = LSOF_FILE_FIFO;
        vt = VFIFO;
        ntype = N_REGLR;
        break;
    case S_IFSOCK:
        ft = LSOF_FILE_SOCKET;
        vt = VSOCK;
        ntype = N_REGLR;
        break;
    default:
        ft = LSOF_FILE_UNKNOWN_STAT;
        vt = VNON;
        ntype = N_REGLR;
        break;
    }

    Lf->type = ft;
    Lf->ntype = Ntype = ntype;
    if (path && *path) {
        if (Sfile && is_file_named(ctx, (char *)path, Ntype, vt, 1))
            Lf->sf |= SELNM;
        enter_nm(ctx, (char *)path);
    } else
        enter_nm(ctx, "(unknown)");
}

static void process_path(struct lsof_context *ctx, int pid,
                         enum lsof_fd_type fdt, int fd, const char *procpath,
                         const char *pathpath) {
    char nm[MAXPATHLEN + 1];
    struct stat st;
    ssize_t n;

    n = readlink(pathpath, nm, sizeof(nm) - 1);
    if (n < 0)
        nm[0] = '\0';
    else
        nm[n] = '\0';

    /*
     * stat() on /proc/<pid>/fd/<n> returns the underlying file, including
     * deleted ZFS files.  For cwd/root/exe, stat the procfs symlink itself.
     */
    if (stat(procpath, &st) != 0) {
        if (!nm[0])
            return;
        if (stat(nm, &st) != 0)
            return;
    }
    enter_path(ctx, fdt, fd, nm[0] ? nm : procpath, &st);
    if (Lf && Lf->sf)
        link_lfile(ctx);
}

void gather_proc_info(struct lsof_context *ctx) {
    DIR *pd, *fdd;
    struct dirent *dp, *fdp;
    psinfo_t ps;
    char path[MAXPATHLEN + 1];
    char fdpath[MAXPATHLEN + 64];
    char pspath[MAXPATHLEN + 1];
    int pid;

    if (!(pd = opendir(PROC_BASE))) {
        (void)fprintf(stderr, "%s: can't open %s: %s\n", Pn, PROC_BASE,
                      strerror(errno));
        Error(ctx);
    }

    while ((dp = readdir(pd))) {
        short pss, sf;

        if (nm2id(dp->d_name, &pid))
            continue;
        (void)snpf(pspath, sizeof(pspath), "%s/%d/psinfo", PROC_BASE, pid);
        if (read_psinfo(pspath, &ps))
            continue;
        if (is_proc_excl(ctx, pid, (int)ps.pr_pgid, (UID_ARG)ps.pr_uid, &pss,
                         &sf) ||
            is_cmd_excl(ctx, ps.pr_fname, &pss, &sf))
            continue;

        alloc_lproc(ctx, pid, (int)ps.pr_pgid, (int)ps.pr_ppid,
                    (UID_ARG)ps.pr_euid, ps.pr_fname, (int)pss, (int)sf);
        Plf = (struct lfile *)NULL;

        (void)snpf(path, sizeof(path), "%s/%d/cwd", PROC_BASE, pid);
        (void)snpf(pspath, sizeof(pspath), "%s/%d/path/cwd", PROC_BASE, pid);
        process_path(ctx, pid, LSOF_FD_CWD, -1, path, pspath);

        (void)snpf(path, sizeof(path), "%s/%d/root", PROC_BASE, pid);
        (void)snpf(pspath, sizeof(pspath), "%s/%d/path/root", PROC_BASE, pid);
        process_path(ctx, pid, LSOF_FD_ROOT_DIR, -1, path, pspath);

        (void)snpf(path, sizeof(path), "%s/%d/path/a.out", PROC_BASE, pid);
        process_path(ctx, pid, LSOF_FD_PROGRAM_TEXT, -1, path, path);

        (void)snpf(path, sizeof(path), "%s/%d/fd", PROC_BASE, pid);
        if (!(fdd = opendir(path)))
            continue;
        while ((fdp = readdir(fdd))) {
            int fd;

            if (nm2id(fdp->d_name, &fd))
                continue;
            (void)snpf(fdpath, sizeof(fdpath), "%s/%s", path, fdp->d_name);
            (void)snpf(pspath, sizeof(pspath), "%s/%d/fdinfo/%d", PROC_BASE,
                       pid, fd);
            {
                char rpath[MAXPATHLEN + 1];
                (void)snpf(rpath, sizeof(rpath), "%s/%d/path/%d", PROC_BASE,
                           pid, fd);
                process_fdinfo(ctx, pid, fd, fdpath, pspath, rpath);
            }
        }
        (void)closedir(fdd);
    }
    (void)closedir(pd);
}
