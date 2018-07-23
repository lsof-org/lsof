/*
 * "@(#)prdata.h	Revision: 3.1	93/02/10 21:54:22"
 */

/*	Copyright (c) 1984, 1986, 1987, 1988, 1989, 1990 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifndef _PROC_PRDATA_H
#define _PROC_PRDATA_H

#ident	"@(#)genfs:proc/prdata.h	1.1"
#define	round(r)	(((r)+sizeof(int)-1)&(~(sizeof(int)-1)))

#define	PNSIZ	5			/* size of /proc name entries */

/*
 * Macros for mapping between i-numbers and pids.
 */
#define	PRBIAS	64
#define	itop(n)	((int)((n)-PRBIAS))	/* i-number to pid */
#define	ptoi(n)	((int)((n)+PRBIAS))	/* pid to i-number */

typedef struct prnode {
	struct prnode	*pr_free;	/* freelist pointer */
	struct vnode	*pr_vnext;	/* linked list of invalid vnodes */
	struct vnode	pr_vnode;	/* associated vnode */
	struct proc	*pr_proc;	/* process being traced */
	short		pr_mode;	/* file mode bits */
	short		pr_opens;	/* count of opens */
	short		pr_writers;	/* count of opens for writing */
	short		pr_flags;	/* private flags */
} prnode_t;

/*
 * Conversion macros.
 */
#define	VTOP(vp)	((struct prnode *)(vp)->v_data)
#define	PTOV(pnp)	((struct vnode *)&(pnp)->pr_vnode)

/*
 * Flags for pr_flags.
 */
#define	PREXCL		0x01	/* Exclusive-use (disallow opens for write) */
#define	PRINVAL		0x02	/* vnode is invalid (security provision) */

/*
 * Flags to prlock().
 */
#define	ZNO	0	/* Fail on encountering a zombie process. */
#define	ZYES	1	/* Allow zombies. */

/*
 * Assign one set to another (possible different sizes).
 *
 * Assigning to a smaller set causes members to be lost.
 * Assigning to a larger set causes extra members to be cleared.
 */
#define	prassignset(ap, sp)					\
{								\
	register int _i_ = sizeof(*(ap))/sizeof(u_long);	\
	while (--_i_ >= 0)					\
		((u_long*)(ap))[_i_] =				\
		  (_i_ >= sizeof(*(sp))/sizeof(u_long)) ?	\
		  0L : ((u_long*)(sp))[_i_];			\
}

/*
 * Determine whether or not a set (of arbitrary size) is empty.
 */
#define prisempty(sp) setisempty((u_long *)(sp), sizeof(*(sp))/sizeof(u_long))

#ifdef _KERNEL

#include "sys/types.h"
#include "sys/proc.h"
#include "sys/procfs.h"
#include "sys/uio.h"
#include "sys/reg.h"
#include "vm/seg.h"

#if defined(__STDC__)

int		prusrio(proc_t *, enum uio_rw, struct uio *);
int		prisreadable(proc_t *, cred_t *);
int		prlock(struct prnode *, int, int);
void		prunlock(struct prnode *);
void		prgetstatus(proc_t *, struct prstatus *);
void		prgetaction(proc_t *, user_t *, u_int, struct sigaction *);
int		prnsegs(proc_t *);
void		prgetmap(proc_t *, struct prmap *);
vnode_t		*prvnode(proc_t *, struct seg *, caddr_t);
void		prgetpsinfo(proc_t *, struct prpsinfo *);
int		setisempty(u_long *, unsigned);

#else

int		prusrio();
int		prisreadable();
int		prlock();
void		prunlock();
void		prgetstatus();
void		prgetaction();
int		prnsegs();
void		prgetmap();
vnode_t		*prvnode();
void		prgetpsinfo();
int		setisempty();

#endif

/*
 * Machine-dependent routines (defined in prmachdep.c).
 */

#if defined(__STDC__)

user_t		*prumap(proc_t *);
void		prunmap(proc_t *);
struct seg	*prgrowstack(proc_t *, caddr_t);
void		prgetregs(user_t *, gregset_t);
void		prsetregs(user_t *, gregset_t);
greg_t		prgetpc(gregset_t);
int		prhasfp(void);
void		prgetfpregs(proc_t *, fpregset_t *);
void		prsetfpregs(proc_t *, fpregset_t *);
caddr_t		prgetpsaddr(proc_t *);
void		prstep(proc_t *, user_t *);
void		prsvaddr(proc_t *, user_t *, caddr_t);
caddr_t		prmapin(proc_t *, caddr_t, int);
void		prmapout(proc_t *, caddr_t, caddr_t, int);
caddr_t		prfastmapin(proc_t *, caddr_t, int);
void		prfastmapout(proc_t *, caddr_t, caddr_t, int);

#else

user_t		*prumap();
void		prunmap();
struct seg	*prgrowstack();
void		prgetregs();
void		prsetregs();
greg_t		prgetpc();
int		prhasfp();
void		prgetfpregs();
void		prsetfpregs();
caddr_t		prgetpsaddr();
void		prstep();
void		prsvaddr();
caddr_t		prmapin();
void		prmapout();
caddr_t		prfastmapin();
void		prfastmapout();

#endif

extern int procfstype;
extern int prmounted;		/* Set to 1 if /proc is mounted. */
extern struct vfs *procvfs;	/* Points to /proc vfs entry. */
extern dev_t procdev;
extern struct vnodeops prvnodeops;
extern struct vfsops prvfsops;
extern struct prnode prrootnode;

/*
 * Prototypes for other external functions; these should appear elsewhere
 * but don't.
 */
#if defined(__STDC__)

int	donice(proc_t *, cred_t *, int, int *);
int	fsig(proc_t *);
int	getudev(void);
void	setrun(proc_t *);	
int	suser(cred_t *);

#else

int	donice();
int	fsig();
int	getudev();
void	setrun();
int	stop();
int	suser();

#endif

#endif	/* _KERNEL */

#endif	/* _PROC_PRDATA_H */
