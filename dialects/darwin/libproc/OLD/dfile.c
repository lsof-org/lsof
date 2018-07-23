/*
 * dfile.c -- Darwin file processing functions for libproc-based lsof
 */


/*
 * Portions Copyright 2005-2007 Apple Inc.  All rights reserved.
 *
 * Copyright 2005 Purdue Research Foundation, West Lafayette, Indiana
 * 47907.  All rights reserved.
 *
 * Written by Allan Nathanson, Apple Inc., and Victor A. Abell, Purdue
 * University.
 *
 * This software is not subject to any license of the American Telephone
 * and Telegraph Company or the Regents of the University of California.
 *
 * Permission is granted to anyone to use this software for any purpose on
 * any computer system, and to alter it and redistribute it freely, subject
 * to the following restrictions:
 *
 * 1. Neither the authors, nor Apple Inc. nor Purdue University are
 *    responsible for any consequences of the use of this software.
 *
 * 2. The origin of this software must not be misrepresented, either
 *    by explicit claim or by omission.  Credit to the authors, Apple
 *    Inc. and Purdue University must appear in documentation and sources.
 *
 * 3. Altered versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 4. This notice may not be removed or altered.
 */


#ifndef lint
static char copyright[] =
"@(#) Copyright 2005-2007 Apple Inc. and Purdue Research Foundation.\nAll rights reserved.\n";
static char *rcsid = "$Id: dfile.c,v 1.4 2006/03/27 23:23:13 abe Exp abe $";
#endif


#include "lsof.h"


/*
 * enter_file_info() -- enter file information
 */

void
enter_file_info(pfi)
	struct proc_fileinfo *pfi;	/* pointer to process file info */
{
	int f;
/*
 * Construct access code
 */
	f = pfi->fi_openflags & (FREAD | FWRITE);
	if (f == FREAD)
	    Lf->access = 'r';
	else if (f == FWRITE)
	    Lf->access = 'w';
	else if (f == (FREAD | FWRITE))
	    Lf->access = 'u';
/*
 * Save the offset / size
 */
	Lf->off = (SZOFFTYPE)pfi->fi_offset;
	if (Foffset)
	    Lf->off_def = 1;
/*
 * Save file structure information as requested.
 */
	if (Fsv & FSV_FG) {
	    Lf->ffg = (long)pfi->fi_openflags;
	    Lf->fsv |= FSV_FG;
	}
}


/*
 * enter_vnode_info() -- enter vnode information
 */

void
enter_vnode_info(vip)
	struct vnode_info_path *vip;	/* pointer to vnode info with path */
{
	char buf[32], *cp;
	dev_t dev = 0;
	int devs = 0;
	struct mounts *mp;
/*
 * Derive file type.
 */
	switch ((int)(vip->vip_vi.vi_stat.vst_mode & S_IFMT)) {
	case S_IFIFO:
	    cp = "FIFO";
	    Ntype = N_FIFO;
	    break;
	case S_IFCHR:
	    cp = "CHR";
	    Ntype = N_CHR;
	    break;
	case S_IFDIR:
	    cp = "DIR";
	    Ntype = N_REGLR;
	    break;
	case S_IFBLK:
	    cp = "BLK";
	    Ntype = N_BLK;
	    break;
	case S_IFREG:
	    cp = "REG";
	    Ntype = N_REGLR;
	    break;
	default:
	    (void) snpf(buf, sizeof(buf), "%04o",
		(((vip->vip_vi.vi_stat.vst_mode & S_IFMT) >> 12) & 0xfff));
	    cp = buf;
	    Ntype = N_REGLR;
	}
	if (!Lf->type[0])
	    (void) snpf(Lf->type, sizeof(Lf->type), "%s", cp);
	Lf->ntype = Ntype;
/*
 * Save device number and path
 */
	switch (Ntype) {
	case N_FIFO:
	    break;
	case N_CHR:
	case N_BLK:
	    Lf->rdev = vip->vip_vi.vi_stat.vst_rdev;
	    Lf->rdev_def = 1;
	    /* fall through */
	default:
	    Lf->dev = dev = vip->vip_vi.vi_stat.vst_dev;
	    Lf->dev_def = devs = 1;
	}
/*
 * Save path name.
 */
	vip->vip_path[sizeof(vip->vip_path) - 1] = '\0';
	if (vip->vip_path[0] != '\0') {
	    Lf->V_path = mkstrcpy(vip->vip_path, (MALLOC_S *)NULL);
	}
/*
 * Save node number.
 */
	Lf->inode = (INODETYPE)vip->vip_vi.vi_stat.vst_ino;
	Lf->inp_ty = 1;
/*
 * Save link count, as requested.
 */
	if (Fnlink) {
	    Lf->nlink = vip->vip_vi.vi_stat.vst_nlink;
	    Lf->nlink_def = 1;
	    if (Nlink && (Lf->nlink < Nlink))
		Lf->sf |= SELNLINK;
	}
/*
 * If a device number is defined, locate file system and save its identity.
 */
	if (devs) {
	    for (mp = readmnt(); mp; mp = mp->next) {
		if (dev == mp->dev) {
		    Lf->fsdir = mp->dir;
		    Lf->fsdev = mp->fsname;
		    break;
		}
	    }
	}
/*
 * Save the file size.
 */
	switch (Ntype) {
	case N_CHR:
	case N_FIFO:
	    Lf->off_def = 1;
	    break;
	default:
	    Lf->sz = (SZOFFTYPE)vip->vip_vi.vi_stat.vst_size;
	    Lf->sz_def = 1;
	}
/*
 * Test for specified file.
 */
	if (Sfile && is_file_named(NULL,
				   ((Ntype == N_CHR) || (Ntype == N_BLK) ? 1
									 : 0)))
	{
	    Lf->sf |= SELNM;
	}
/*
 * Enter name characters.
 */
	if (!Lf->nm && Namech[0])
	    enter_nm(Namech);
}


/*
 * err2nm() -- convert errno to a message in Namech
 */

void
err2nm(pfx)
	char *pfx;			/* Namech message prefix */
{
	char *sfx;

	switch (errno) {
	case EBADF:

	/*
	 * The file descriptor is no longer available.
	 */
	    sfx = "FD unavailable";
	    break;
	case ESRCH:

	/*
	 * The process is no longer available.
	 */
	    sfx = "process unavailable";
	    break;
	default:

	/*
	 * All other errors are reported with strerror() information.
	 */
	    sfx = strerror(errno);
	}
	(void) snpf(Namech, Namechl, "%s: %s", pfx, sfx);
	enter_nm(Namech);
}


/*
 * print_v_path() -- print vnode's path
 */

int
print_v_path(lf)
	struct lfile *lf;
{
	if (lf->V_path) {
	    safestrprt(lf->V_path, stdout, 0);
	    return(1);
	}
	return(0);
}


/*
 * process_atalk() -- process an Apple Talk file
 */

void
process_atalk(pid, fd)
	int pid;			/* PID */
	int32_t fd;			/* FD */
{
	(void) snpf(Lf->type, sizeof(Lf->type), "ATALK");
	return;
}


/*
 * process_fsevents() -- process a file system events file
 */

void
process_fsevents(pid, fd)
	int pid;			/* PID */
	int32_t fd;			/* FD */
{
	(void) snpf(Lf->type, sizeof(Lf->type), "FSEVENTS");
}


/*
 * process_kqueue() -- process a kernel queue file
 */

void
process_kqueue(pid, fd)
	int pid;			/* PID */
	int32_t fd;			/* FD */
{
	struct kqueue_fdinfo kq;
	int nb;
/*
 * Get the kernel queue file information.
 */
	(void) snpf(Lf->type, sizeof(Lf->type), "KQUEUE");
	nb = proc_pidfdinfo(pid, fd, PROC_PIDFDKQUEUEINFO, &kq, sizeof(kq));
	if (nb <= 0) {
	    (void) err2nm("kqueue");
	    return;
	} else if (nb < sizeof(kq)) {
	    (void) fprintf(stderr,
		"%s: PID %d, FD %d; proc_pidfdinfo(PROC_PIDFDKQUEUEINFO);\n",
		Pn, pid, fd);
	    (void) fprintf(stderr,
		"      too few bytes; expected %ld, got %d\n",
		sizeof(kq), nb);
	    Exit(1);
	}
/*
 * Enter the kernel queue file information.
 */
	enter_file_info(&kq.pfi);
/*
 * Enter queue counts and the O_EVTONLY flag as NAME column information.
 */
	(void) snpf(Namech, Namechl,
	    "count=%" SZOFFPSPEC "u, state=%#x%s",
	    (SZOFFTYPE)kq.kqueueinfo.kq_stat.vst_size,
	    kq.kqueueinfo.kq_state,
	    (kq.pfi.fi_openflags & O_EVTONLY) ? ", " FF_EVTONLY : "");
	enter_nm(Namech);
}


/*
 * process_pipe() -- process pipe file
 */

void
process_pipe(pid, fd)
	int pid;			/* PID */
	int32_t fd;			/* FD */
{
	char dev_ch[32], *ep;
	int nb;
	struct pipe_fdinfo pi;
        size_t sz;
/*
 * Get pipe file information.
 */
	(void) snpf(Lf->type, sizeof(Lf->type), "PIPE");
	nb = proc_pidfdinfo(pid, fd, PROC_PIDFDPIPEINFO, &pi, sizeof(pi));
	if (nb <= 0) {
	    (void) err2nm("pipe");
	    return;
	} else if (nb < sizeof(pi)) {
	    (void) fprintf(stderr,
		"%s: PID %d, FD %d; proc_pidfdinfo(PROC_PIDFDPIPEINFO);\n",
		Pn, pid, fd);
	    (void) fprintf(stderr,
		"      too few bytes; expected %ld, got %d\n",
	       sizeof(pi), nb);
	    Exit(1);
	}
/*
 * Enter the pipe handle as the device.
 */
	(void) snpf(dev_ch, sizeof(dev_ch), "%s",
	    print_kptr((KA_T)pi.pipeinfo.pipe_handle, (char *)NULL, 0));
	enter_dev_ch(dev_ch);
/*
 * Enable offset or size reporting.
 */
	if (Foffset)
	    Lf->off_def = 1;
	else {
	    Lf->sz = (SZOFFTYPE)pi.pipeinfo.pipe_stat.vst_blksize;
	    Lf->sz_def = 1;
	}
/*
 * If there is a peer handle, enter it in as NAME column information.
 */
	if (pi.pipeinfo.pipe_peerhandle)
	    (void) snpf(Namech, Namechl, "->%s",
		print_kptr((KA_T)pi.pipeinfo.pipe_peerhandle, (char *)NULL, 0));
	else
	    Namech[0] = '\0';
/*
 * If the pipe has a count, add it to the NAME column.
 */
	if (pi.pipeinfo.pipe_stat.vst_size) {
	    ep = endnm(&sz);
	    (void) snpf(ep, sz, ", cnt=%" SZOFFPSPEC "u",
		(SZOFFTYPE)pi.pipeinfo.pipe_stat.vst_size);
	}
}


/*
 * process_psem() -- process a POSIX semaphore file
 */

void
process_psem(pid, fd)
	int pid;			/* PID */
	int32_t fd;			/* FD */
{
	int nb;
	struct psem_fdinfo ps;
/*
 * Get the sempaphore file information.
 */
	(void) snpf(Lf->type, sizeof(Lf->type), "PSXSEM");
	nb = proc_pidfdinfo(pid, fd, PROC_PIDFDPSEMINFO, &ps, sizeof(ps));
	if (nb <= 0) {
	    (void) err2nm("semaphore");
	    return;
	} else if (nb < sizeof(ps)) {
	    (void) fprintf(stderr,
		"%s: PID %d, FD %d; proc_pidfdinfo(PROC_PIDFDPSEMINFO);\n",
		Pn, pid, fd);
	    (void) fprintf(stderr,
		"      too few bytes; expected %ld, got %d\n",
		sizeof(ps), nb);
	    Exit(1);
	}
/*
 * Enter the semaphore file information.
 */
	enter_file_info(&ps.pfi);
/*
 * If there is a semaphore file name, enter it.
 */
	if (ps.pseminfo.psem_name[0]) {
	    ps.pseminfo.psem_name[sizeof(ps.pseminfo.psem_name) - 1] = '\0';
	    (void) snpf(Namech, Namechl, "%s", ps.pseminfo.psem_name);
	    enter_nm(Namech);
	}
/*
 * Unless file size has been specifically requested, enable the printing of
 * file offset.
 */
	if (!Fsize)
	    Lf->off_def = 1;
}


/*
 * process_pshm() -- process POSIX shared memory file
 */

void
process_pshm(pid, fd)
	int pid;			/* PID */
	int32_t fd;			/* FD */
{
	int nb;
	struct pshm_fdinfo ps;
/*
 * Get the POSIX shared memory file information.
 */
	(void) snpf(Lf->type, sizeof(Lf->type), "PSXSHM");
	nb = proc_pidfdinfo(pid, fd, PROC_PIDFDPSHMINFO, &ps, sizeof(ps));
	if (nb <= 0) {
	    (void) err2nm("POSIX shared memory");
	    return;
	} else if (nb < sizeof(ps)) {
	    (void) fprintf(stderr,
		"%s: PID %d, FD %d; proc_pidfdinfo(PROC_PIDFDPSHMINFO);\n",
		Pn, pid, fd);
	    (void) fprintf(stderr,
		"      too few bytes; expected %ld, got %d\n",
		sizeof(ps), nb);
	    Exit(1);
	}
/*
 * Enter the POSIX shared memory file information.
 */
	enter_file_info(&ps.pfi);
/*
 * If the POSIX shared memory file has a path name, enter it; otherwise, if it
 * has a mapping address, enter that.
 */
	if (ps.pshminfo.pshm_name[0]) {
	    ps.pshminfo.pshm_name[sizeof(ps.pshminfo.pshm_name) - 1] = '\0';
	    (void) snpf(Namech, Namechl, "%s", ps.pshminfo.pshm_name);
	    enter_nm(Namech);
	} else if (ps.pshminfo.pshm_mappaddr) {
	    (void) snpf(Namech, Namechl, "obj=%s",
		print_kptr((KA_T)ps.pshminfo.pshm_mappaddr, (char *)NULL, 0));
	    enter_nm(Namech);
	}
/*
 * Enable offset or size reporting.
 */
	if (Foffset)
	    Lf->off_def = 1;
	else {
	    Lf->sz = (SZOFFTYPE)ps.pshminfo.pshm_stat.vst_size;
	    Lf->sz_def = 1;
	}
}


/*
 * process_vnode() -- process a vnode file
 */

void
process_vnode(pid, fd)
	int pid;			/* PID */
	int32_t fd;			/* FD */
{
	int nb;
	struct vnode_fdinfowithpath vi;

	nb = proc_pidfdinfo(pid, fd, PROC_PIDFDVNODEPATHINFO, &vi, sizeof(vi));
	if (nb <= 0) {
	    if (errno == ENOENT) {

	    /*
	     * The file descriptor's vnode may have been revoked.  This is a
	     * bit of a hack, since an ENOENT error might not always mean the
	     * descriptor's vnode has been revoked.  As the libproc API
	     * matures, this code may need to be revisited.
	     */
		enter_nm("(revoked)");
	    } else
		(void) err2nm("vnode");
	    return;
	} else if (nb < sizeof(vi)) {
	    (void) fprintf(stderr,
		"%s: PID %d, Fd %d: proc_pidfdinfo(PROC_PIDFDVNODEPATHINFO);\n",
		Pn, pid, fd);
	    (void) fprintf(stderr,
		"      too few bytes; expected %ld, got %d\n",
		sizeof(vi), nb);
	    Exit(1);
	}
/*
 * Enter the file and vnode information.
 */
	enter_file_info(&vi.pfi);
	enter_vnode_info(&vi.pvip);
}
