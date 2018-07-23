/*
 * dproc.c -- Darwin process access functions for libproc-based lsof
 */


/*
 * Portions Copyright 2005 Apple Computer, Inc.  All rights reserved.
 *
 * Copyright 2005 Purdue Research Foundation, West Lafayette, Indiana
 * 47907.  All rights reserved.
 *
 * Written by Allan Nathanson, Apple Computer, Inc., and Victor A.
 * Abell, Purdue University.
 *
 * This software is not subject to any license of the American Telephone
 * and Telegraph Company or the Regents of the University of California.
 *
 * Permission is granted to anyone to use this software for any purpose on
 * any computer system, and to alter it and redistribute it freely, subject
 * to the following restrictions:
 *
 * 1. Neither the authors, nor Apple Computer, Inc. nor Purdue University
 *    are responsible for any consequences of the use of this software.
 *
 * 2. The origin of this software must not be misrepresented, either
 *    by explicit claim or by omission.  Credit to the authors, Apple
 *    Computer, Inc. and Purdue University must appear in documentation
 *    and sources.
 *
 * 3. Altered versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 4. This notice may not be removed or altered.
 */


#ifndef lint
static char copyright[] =
"@(#) Copyright 2005 Apple Computer, Inc. and Purdue Research Foundation.\nAll rights reserved.\n";
static char *rcsid = "$Id: dproc.c,v 1.4 2005/11/04 20:44:17 abe Exp abe $";
#endif

#include "lsof.h"


/*
 * Local definitions
 */

#define	PIDS_INCR	(sizeof(int) * 32)	/* PID space increment */
#define	FDS_INCR	(sizeof(struct proc_fdinfo) * 32)
						/* FD space increment */
#define	VIPS_INCR	16			/* Vips space increment */


/*
 * Local static variables
 */

static struct proc_fdinfo *Fds = (struct proc_fdinfo *)NULL;
						/* FD buffer */
static int NbPids = 0;				/* bytes allocated to Pids */
static int NbFds = 0;				/* bytes allocated to FDs */
static int *Pids = (int *)NULL;			/* PID buffer */


/*
 * Local structure definitions
 */

static struct vips_info {
	dev_t	dev;
	ino_t	ino;
} *Vips	= (struct vips_info *)NULL;		/* recorded vnodes */
static int NbVips = 0;				/* bytes allocated to Vips */
static int NVips = 0;				/* entries allocated to Vips */


/*
 * Local function prototypes
 */
_PROTOTYPE(static void enter_vn_text,(struct vnode_info_path *vip, int *n));
_PROTOTYPE(static void process_fds,(int pid));
_PROTOTYPE(static void process_text,(int pid));


/*
 * enter_vn_text() -- enter vnode information text reference
 */

static void
enter_vn_text(vip, n)
	struct vnode_info_path *vip;	/* vnode info */
	int *n;				/* number of vips[] entries in use */
{
	int i;
/*
 * Ignore the request if the vnode information has already been entered.
 */
	for (i = 0; i < *n; i++) {
	    if ((vip->vip_vi.vi_stat.st_dev == Vips[i].dev)
	    &&  (vip->vip_vi.vi_stat.st_ino == Vips[i].ino))
	    {
		return;
	    }
	}
/*
 * Save the text file information.
 */
	alloc_lfile(" txt", -1);
	Cfp = (struct file *)NULL;
	(void) enter_vnode_info(vip);
	if (Lf->sf)
	    link_lfile();
/*
 * Record the entry of the vnode information.
 */
	if (i >= NVips) {

	/*
	 * Allocate space for recording the vnode information.
	 */
	    NVips += VIPS_INCR;
	    NbVips += (int)(VIPS_INCR * sizeof(struct vips_info));
	    if (!Vips)
		Vips = (struct vips_info *)malloc((MALLOC_S)NbVips);
	    else
		Vips = (struct vips_info *)realloc((MALLOC_P *)Vips,
						   (MALLOC_S)NbVips);
	    if (!Vips) {
		(void) fprintf(stderr, "%s: PID %d: no text recording space\n",
		    Pn, Lp->pid);
		Exit(1);
	    }
	}
/*
 * Record the vnode information.
 */
	Vips[*n].dev = vip->vip_vi.vi_stat.st_dev;
	Vips[*n].ino = vip->vip_vi.vi_stat.st_ino;
	(*n)++;
}


/*
 * gather_proc_info() -- gather process information
 */

void
gather_proc_info()
{
	int ef, i, nb, np, pid;
	short pss, sf;
	struct proc_taskallinfo ti;
	struct proc_vnodepathinfo vpi;
/*
 * Determine how many bytes are needed to contain the PIDs on the system;
 * make sure sufficient buffer space is allocated to hold them (and a few
 * extra); then read the list of PIDs.
 */
	if ((nb = proc_listpids(PROC_ALL_PIDS, 0, NULL, 0)) <= 0) {
	    (void) fprintf(stderr, "%s: can't get PID byte count: %s\n",
		Pn, strerror(errno));
	    Exit(1);
	}
	if (nb > NbPids) {
	    while (nb > NbPids) {
		NbPids += PIDS_INCR;
	    }
	    if (!Pids)
		Pids = (int *)malloc((MALLOC_S)NbPids);
	    else
		Pids = (int *)realloc((MALLOC_P *)Pids, (MALLOC_S)NbPids);
	    if (!Pids) {
		(void) fprintf(stderr,
		    "%s: can't allocate space for %d PIDs\n", Pn,
		    (int)(NbPids / sizeof(int *)));
		Exit(1);
	    }
	}
/*
 * Get the list of PIDs.
 */
	for (ef = 0; !ef;) {
	    if ((nb = proc_listpids(PROC_ALL_PIDS, 0, Pids, NbPids)) <= 0) {
		(void) fprintf(stderr, "%s: can't get list of PIDs: %s\n",
		    Pn, strerror(errno));
		Exit(1);
	    }

	    if ((nb + sizeof(int)) < NbPids) {

	    /*
	     * There is room in the buffer for at least one more PID.
	     */
		np = nb / sizeof(int);
		ef = 1;
	    } else {

	    /*
	     * The PID buffer must be enlarged.
	     */
		NbPids += PIDS_INCR;
		Pids = (int *)realloc((MALLOC_P *)Pids, (MALLOC_S)NbPids);
		if (!Pids) {
		    (void) fprintf(stderr,
			"%s: can't allocate space for %d PIDs\n", Pn,
			(int)(NbPids / sizeof(int *)));
		    Exit(1);
		}
	    }
	}
/*
 * Loop through the identified processes.
 */
	for (i = 0; i < np; i++) {
	    if (!(pid = Pids[i]))
		continue;
	    nb = proc_pidinfo(pid, PROC_PIDTASKALLINFO, 0, &ti, sizeof(ti));
	    if (nb <= 0) {
		if (errno == ESRCH)
		    continue;
		if (!Fwarn) {
		    (void) fprintf(stderr, "%s: PID %d information error: %s\n",
			Pn, pid, strerror(errno));
		}
		continue;
	    } else if (nb < sizeof(ti)) {
		(void) fprintf(stderr,
		    "%s: PID %d: proc_pidinfo(PROC_PIDTASKALLINFO);\n",
		    Pn, pid);
		(void) fprintf(stderr,
		    "      too few bytes; expected %ld, got %d\n",
		    sizeof(ti), nb);
		Exit(1);
	    }
	/*
	 * Check for process or command exclusion.
	 */
	    if (is_proc_excl((int)pid, (int)ti.pbsd.pbi_rgid,
			     (UID_ARG)ti.pbsd.pbi_uid, &pss, &sf))
	    {
		continue;
	    }
	    ti.pbsd.pbi_comm[sizeof(ti.pbsd.pbi_comm) - 1] = '\0';
	    if (is_cmd_excl(ti.pbsd.pbi_comm, &pss, &sf))
		continue;
	    if (ti.pbsd.pbi_name[0]) {
		ti.pbsd.pbi_name[sizeof(ti.pbsd.pbi_name) - 1] = '\0';
		if (is_cmd_excl(ti.pbsd.pbi_name, &pss, &sf))
		    continue;
	    }
	/*
	 * Get root and current directory information.
	 */
	    nb = proc_pidinfo(pid, PROC_PIDVNODEPATHINFO, 0, &vpi, sizeof(vpi));
	    if (nb <= 0) {
		if (errno == ESRCH)
		    continue;
		if (!Fwarn) {
		    (void) fprintf(stderr,
			"%s: PID %d root|current directory error: %s\n",
			Pn, pid, strerror(errno));
		}
		continue;
	    } else if (nb < sizeof(vpi)) {
		(void) fprintf(stderr,
		    "%s: PID %d: proc_pidinfo(PROC_PIDVNODEPATHINFO);\n",
		    Pn, pid);
		(void) fprintf(stderr,
		    "      too few bytes; expected %ld, got %d\n",
		    sizeof(vpi), nb);
		Exit(1);
	    }
	/*
	 * Allocate local process space.
	 */
	    alloc_lproc((int)pid, (int)ti.pbsd.pbi_rgid, (int)ti.pbsd.pbi_ppid,
		(UID_ARG)ti.pbsd.pbi_uid,
		(ti.pbsd.pbi_name[0] != '\0') ? ti.pbsd.pbi_name
					      : ti.pbsd.pbi_comm,
		(int)pss, (int)sf);
	    Plf = (struct lfile *)NULL;
	/*
	 * Save current working directory information.
	 */
	    if (vpi.pvi_cdir.vip_path[0]) {
		alloc_lfile(CWD, -1);
		Cfp = (struct file *)NULL;
		(void) enter_vnode_info(&vpi.pvi_cdir);
		if (Lf->sf)
		    link_lfile();
	    }
	/*
	 * Save root directory information.
	 */
	    if (vpi.pvi_rdir.vip_path[0]) {
		alloc_lfile(RTD, -1);
		Cfp = (struct file *)NULL;
		(void) enter_vnode_info(&vpi.pvi_rdir);
		if (Lf->sf)
		    link_lfile();
	    }
	/*
	 * Print text file information.
	 */
	    (void) process_text(pid);
	/*
	 * Loop through the file descriptors.
	 */
	    (void) process_fds(pid);
	/*
	 * Examine results.
	 */
	    if (examine_lproc())
		return;
	}
}


/*
 * initialize() -- perform all initialization
 */

void
initialize()
{
}


/*
 * process_fds() -- process file descriptors
 */

static void
process_fds(pid)
	int pid;			/* PID of interest */
{
	int ef, i, nb, nf;
	struct proc_fdinfo *fdp;
/*
 * Make sure an FD buffer has been allocated.
 */
	if (!Fds) {
	    NbFds = FDS_INCR;
	    if (!(Fds = (struct proc_fdinfo *)malloc((MALLOC_S)NbFds))) {
		(void) fprintf(stderr,
		    "%s: can't allocate %d FD entries\n", Pn,
		    (int)(NbFds / sizeof(struct proc_fdinfo)));
		Exit(1);
	    }
	}
/*
 * Get FD information for the process.
 */
	for (ef = 0; !ef;) {
	    nb = proc_pidinfo(pid, PROC_PIDLISTFDS, 0, Fds, NbFds);
	    if (nb <= 0) {
		if (errno == ESRCH)
		    continue;
		if (!Fwarn) {
		    (void) fprintf(stderr, "%s: PID %d FD info error: %s\n",
			Pn, pid, strerror(errno));
		}
		continue;
	    }
	    if ((nb + sizeof(struct proc_fdinfo)) < NbFds) {

	    /*
	     * There is room in the FD buffer for one more proc_fdinfo
	     * structure.
	     */
		nf = (int)(nb / sizeof(struct proc_fdinfo));
		ef = 1;
	    } else {

	    /*
	     * More proc_fdinfo space is required.  Allocate it.
	     */
		NbFds += FDS_INCR;
		Fds = (struct proc_fdinfo *)realloc((MALLOC_P *)Fds,
						    (MALLOC_S)NbFds);
		if (!Fds) {
		    (void) fprintf(stderr,
			"%s: PID %d: can't allocate space for %d FDs\n",
			Pn, pid, (int)(NbFds / sizeof(struct proc_fdinfo)));
		    Exit(1);
		}
	    }
	}
/*
 * Loop through the file descriptors.
 */
	for (i = 0; i < nf; i++) {
	    fdp = &Fds[i];
	    alloc_lfile(NULL, (int)fdp->proc_fd);
	/*
	 * Process the file by its type.
	 */
	    switch (fdp->proc_fdtype) {
	    case PROX_FDTYPE_ATALK:
		(void) process_atalk(pid, fdp->proc_fd);
		break;
	    case PROX_FDTYPE_FSEVENTS:
		(void) process_fsevents(pid, fdp->proc_fd);
		break;
	    case PROX_FDTYPE_KQUEUE:
		(void) process_kqueue(pid, fdp->proc_fd);
		break;
	    case PROX_FDTYPE_PIPE:
		(void) process_pipe(pid, fdp->proc_fd);
		break;
	    case PROX_FDTYPE_PSEM:
		(void) process_psem(pid, fdp->proc_fd);
		break;
	    case PROX_FDTYPE_SOCKET:
		(void) process_socket(pid, fdp->proc_fd);
		break;
	    case PROX_FDTYPE_PSHM:
		(void) process_pshm(pid, fdp->proc_fd);
		break;
	    case PROX_FDTYPE_VNODE:
		(void) process_vnode(pid, fdp->proc_fd);
		break;
	    default:
		(void) snpf(Namech, Namechl - 1, "unknown file type: %d",
		    fdp->proc_fdtype);
		Namech[Namechl - 1] = '\0';
		(void) enter_nm(Namech);
		break;
	    }
	    if (Lf->sf)
		link_lfile();
	}
}


/*
 * process_text() -- process text information
 */

static void
process_text(pid)
	int pid;			/* PID */
{
	uint64_t a;
	int i, n, nb;
	struct proc_regionwithpathinfo rwpi;

	for (a = (uint64_t)0, i = n = 0; i < 10000;) {
	    nb = proc_pidinfo(pid, PROC_PIDREGIONPATHINFO, a, &rwpi,
			      sizeof(rwpi));
	    if (nb <= 0) {
		if ((errno == ESRCH) || (errno == EINVAL)) {

		/*
		 * Quit if no more text information is available for the
		 * process.
		 */
		    return;
		}
	    /*
	     * Warn about all other errors.
	     */

		if (!Fwarn) {
		    (void) fprintf(stderr, "%s: PID %d path info error: %s\n",
			Pn, pid, strerror(errno));
		}
		return;
	    } else if (nb < sizeof(rwpi)) {
		(void) fprintf(stderr,
		    "%s: PID %d: proc_pidinfo(PROC_PIDREGIONPATHINFO);\n",
		    Pn, pid);
		(void) fprintf(stderr,
		    "      too few bytes; expected %ld, got %d\n",
		    sizeof(rwpi), nb);
		Exit(1);
	    }
	    if (rwpi.prp_vip.vip_path[0])
		enter_vn_text(&rwpi.prp_vip, &n);
	    a = rwpi.prp_prinfo.pri_address + rwpi.prp_prinfo.pri_size;
	}
}
