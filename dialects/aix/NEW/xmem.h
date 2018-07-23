/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* bos53H src/bos/kernel/sys/xmem.h 1.50                                  */
/*                                                                        */
/* Licensed Materials - Property of IBM                                   */
/*                                                                        */
/* (C) COPYRIGHT International Business Machines Corp. 1988,2006          */
/* All Rights Reserved                                                    */
/*                                                                        */
/* US Government Users Restricted Rights - Use, duplication or            */
/* disclosure restricted by GSA ADP Schedule Contract with IBM Corp.      */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
/* @(#)61     1.50  src/bos/kernel/sys/xmem.h, sysvmm, bos53H, h2006_05C2 1/29/06 17:14:46 */
/*
 * COMPONENT_NAME: (SYSVMM) Virtual Memory Manager
 *
 * FUNCTIONS:
 *
 * ORIGINS: 27
 *
 * (C) COPYRIGHT International Business Machines Corp. 1988, 1997
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

#ifndef _H_XMEM
#define _H_XMEM

#include <sys/types.h>
#include <sys/vm_types.h>

/*
 *	Memory Address Space Definitions
 * These values define the address space for kernel services requiring
 * address space information (usually via the segflag parameter). 
 */
#define USER_ADSPACE	0		/* user address space		*/
#define SYS_ADSPACE	1		/* system address space		*/
#define USERI_ADSPACE	2		/* user instruction address space */

/* value of 3 should be reserved for use by UIO_XMEM in uio.h */

/* LW_XMATTACH can only be used as a segflag value when calling xmattach(),
 * not in uio code.  An xmattach() with LW_XMATTACH implies
 * USER_ADSPACE; calling xmattach() with LW_XMATTACH to attach from system
 * address space is illegal.
 */
#define LW_XMATTACH	4		/* lw attach in user address space */

/*
 *	Return Codes
 */

#define XMEM_SUCC	(0)		/* successful completion	*/
#define XMEM_FAIL	(-1)		/* unsuccessful completion	*/
 
/*
 *	Cross Memory Address Space Definitions
 */
#define XMEM_PROC	( 0)		/* process address space	*/
#define XMEM_GLOBAL	(-1)		/* global address space		*/
#define XMEM_INVAL	(-2)		/* access removed		*/
#ifndef __64BIT_KERNEL	/* These aren't used in 64-bit kernel */
#define XMEM_PROC2	(-3)		/* process aspace, split portion */
#define XMEM_XLATE	(-4)		/* Special pre-translated	*/
#endif /* ! __64BIT_KERNEL */
#define XMEM_REMIO	(-5)		/* remote i/o address space	*/

/*
 * Structure for managing pre-translated memory buffers
 * within the xmem descriptor
 */

#ifdef __64BIT_KERNEL
struct xmem_phys {
	rpn_t		total;
	rpn_t		used;
	vpn_t		s_vpn;
	rpn_t 		*rpn;
};
#else
struct xmem_phys {
	int		total;
	int		used;
	unsigned int	s_vpn;
	rpn_t 		*rpn;
};
#endif

/* 
 *	Pretranslation Definitions
 */
#define XLATE_PERSISTENT	0x1	/* prex data structures should	*/
					/* be persistent across calls	*/
#define XLATE_PRESENT		0x2	/* pre-translated buffer	*/
#define XLATE_ALLOC		0x4	/* only allocate prex data	*/
					/* structures ....xlate later	*/
#define XLATE_SPARSE		0x8	/* sparse translations		*/
#define XLATE_DR_SAFE		0x10	/* dynamic reconfig safe prex data */
#define XLATE_TRANSIENT		0x20	/* transient pretranslation	*/
#define XLATE_FAST		0x40	/* fast-path pre-xlation used	*/
/* NOTE: Avoid growing prexflags beyond 8 bits if possible to allow
 * room for xm_amr growth.
 */

#ifdef __64BIT_KERNEL
/*
 *	xmemflags definitions.
 */
#define XMEM_AUTHORITY		0x0001	/* unprivileged or priviledged	*/
#define XMEM_TEMP_ATT		0x0002	/* xmem from a temp attach	*/
#define XMEM_LW			0x0004	/* xmem from a lightweight attach */
#define XMEM_SEG_XLATE		0x0008	/* segments are already pre-xlated */
#define XMEM_SUB_PTR		0x0010	/* use subspace_ptr not _id	*/
#define XMEM_SEG_1T		0x0020	/* segments are 1T, not 256M	*/
#define XMEM_AMR_SAFE           0x0040  /* xm_amr field known to be ok*/
/* NOTE: Avoid growing xmemflags beyond 8 bits if possible to allow
 * room for xm_amr growth.
 */

/*
 *	xm_amr definitions
 */
#define XM_ALL_HKEYS 0                  /* access to all h/w storage keys */

#ifdef _KERNSYS
/* Last value in vmhandle array of xmem descriptor is really a
 * timestamp. 
 */
#define LW_TIME(_dp)	((_dp)->subspace_ptr[(_dp)->num_sids])
#endif /* _KERNSYS */
#endif /* __64BIT_KERNEL */


/*
 *	Cross Memory Descriptor.
 *	Describes what portions of memory are accessable.
 */

#ifdef __64BIT_KERNEL 
#if defined(__64BIT__) ||  defined(_LONG_LONG)

/* 
 * This is the 64-bit kernel's definition of a cross memory descriptor
 * There is no limit to the number of segment crossings allowed.
 */
struct xmem { 
	int		aspace_id;	/* describe address space	*/
	int		num_sids;	/* number of segment IDs	*/

	/* If !(xmemflags & XMEM_SUB_PTR), this field is the Segment ID.
	 *
	 * If (xmemflags & XMEM_SUB_PTR), this field is a pointer to a
	 * list of sids, supporting multi-segment crossings.
	 */
	union {
		vmhandle_t	_subspace_id;        
		vmhandle_t	*_subspace_ptr;
	} _u;

#define subspace_id	_u._subspace_id
#define subspace_ptr	_u._subspace_ptr 

	char		*vaddr;		/* Attached start vaddr 	*/
	ushort		xmemflags;	/* xmem flags			*/
	ushort		prexflags;	/* pre-translation flags	*/
        ushort      	xm_amr;         /* partial AMR, 8 keys (0-7)    */
	pshift_t	l2psize;	/* log2 of region's page size	*/

	struct xmem_phys xp;

	/* XMEM_REMIO overloaded fields:
	 * vaddr	- remote liobn
	 * subspace_id	- remote i/o address
	 * xm_len	- length in bytes (overloaded on 32bk)
	 */
#define rem_liobn	vaddr
#define rem_addr        _u._subspace_id
#define rem_size        num_sids
}; 

#endif /* defined(__64BIT__) ||  defined(_LONG_LONG) */
#else /* __64BIT_KERNEL */

struct xmem {
	union {
		struct {
			short	version;
			short	flag;	/* address space of buffer	*/
		} s;
		int	_aspace_id;
	} _u;
#define aspace_id	_u._aspace_id
#define xm_flag		_u.s.flag
#define xm_version	_u.s.version

	union {
	    vmhandle_t	_subspace_id;	/* portion of aspace for this	*/
	    uint	_prexflags;	/* pre-translation flag word	*/
	} u1;
#define subspace_id	u1._subspace_id
#define prexflags	u1._prexflags

	union {
	    vmhandle_t	_subspace_id2;	/* 2nd portion of aspace	*/
	    struct xmem *_orig_xmem;	/* ptr to real original xmem	*/
	} u2;
#define subspace_id2	u2._subspace_id2
#define orig_xmem	u2._orig_xmem

	char	*uaddr;		/* user buffer address			*/

	/* pre-translation overloaded fields:
	 * uaddr	- pointer to the xmem_phys struct describing the
	 *		  pre-xlated information
	 */
#define rlist		uaddr

	/* XMEM_REMIO overloaded fields:
	 * uaddr	- remote liobn
	 * subspace_id	- remote i/o address
	 * subspace_id2	- remote length in bytes
	 */
#define rem_liobn	uaddr
#define rem_addr	u1._subspace_id
#define rem_size	u2._subspace_id2	
};

#endif /* __64BIT_KERNEL */

/* This is the maximum size of a remote TCE table. Bytes per TCE is defined
 * as TCE_MAP_SIZE in <sys/dma.h>
 */
#define MAX_TCE_MAP_SIZE  0x100000000ULL

/* 
 * For VIO support
 */
#define IS_REMOTE(xp)	(xp->aspace_id == XMEM_REMIO)
#define GET_LIOBN(xp)	((liobn_t)xp->rem_liobn)

extern struct xmem *xmem_global;


/*
 *	Macros for cross memory attach/detach operations.
 */
#define xmemat(vaddr,count,dp) xmattach(vaddr,count,dp,SYS_ADSPACE)
#define xmemdt(dp) xmdetach(dp)
 
/*
 *	Entry points for cross memory operations.
 */


/*cross memory atomic operations*/
int xmem_compare_and_swap_32(int *location,int *original,int newval,
			     struct xmem *dp);
#ifdef _LONG_LONG /*ensure that compiler knows about long long*/
int xmem_compare_and_swap_64(long long *location,long long *original,
			     long long newval,struct xmem *dp);
#endif /*_LONG_LONG*/

#if defined(__64BIT_KERNEL) || defined(__FULL_PROTO)
int xmattach(caddr_t, int32long64_t, struct xmem *, int); 
#else
int xmattach();				/* setup for cross memory */
#endif
/*	char	*uaddr;	*/		/* user buffer address	  */
/*	int32long64_t     count;*/	/* user buffer length	  */
/*	struct xmem *dp;	*/	/* cross memory descriptor*/
/*	int     segflag;	*/	/* address space of buffer */

#ifndef __64BIT_KERNEL
/* 64bit prototype
 */
#if defined(_LONG_LONG)
int xmattach64(unsigned long long vaddr, int count, struct xmem *dp,
int segflag);
#endif
#endif

#if defined(__64BIT_KERNEL) || defined(__FULL_PROTO)
long xmattach_remio(liobn_t, ulong, ulong, struct xmem*);
#else
long xmattach_remio();
#endif
/*        liobn_t       rliobn; */	/* remote LIOBN 	   */
/*        ulong         raddr;  */	/* remote starting address */
/*        ulong         count;  */	/* count in bytes	   */
/*        struct xmem   *dp;    */	/* remote xmem (XMEM_REMIO)*/


/* cross mem copy in */
#if defined(__64BIT_KERNEL) || defined(__FULL_PROTO)
int xmemin(caddr_t, caddr_t, int32long64_t, struct xmem *); 
#else
int xmemin();				/* cross memory copy in */
#endif
/*	char	*uaddr;	*/		/* address in other address space */
/*	char	*kaddr;	*/		/* address in global memory	*/ 
/*	int32long64_t 	count;	*/	/* amount of data to copy	*/
/*	struct xmem *dp;	*/	/* cross memory descriptor	*/

 
#if defined(__64BIT_KERNEL) || defined(__FULL_PROTO)
int xmemout(caddr_t, caddr_t, int32long64_t, struct xmem *); /* xmem copy out */
#else
int xmemout();				/* cross memory copy out */
#endif
/*	char	*kaddr;	*/		/* address in global memory 	*/
/*	char	*uaddr;	*/		/* address in other address space*/
/*	int32long64_t count;	*/	/* amount of data to copy	*/
/*	struct xmem *dp;	*/	/* cross memory descriptor	*/
 
#if defined(__64BIT_KERNEL) || defined(__FULL_PROTO)
int xmdetach(struct xmem *);		/* cross memory detach */
#else
int xmdetach();				/* cross memory detach */
#endif
/*	struct xmem *dp;	*/	/* cross memory descriptor */


/*
 *	Entry points for kernel only cross memory operations.
 */

#define	XMEM_HIDE	0x00000000	/* hide the page */
#define	XMEM_UNHIDE	0x00000001	/* unhide the page */
#define	XMEM_ACC_CHK	0x00000002	/* perform access checking */
#define	XMEM_WRITE_ONLY	0x00000004	/* intended transfer is outbound only*/
#define XMEM_DR_SAFE    0x00000008      /* caller is DR safe/aware */

 
/* cross memory DMA setup */
#ifndef __64BIT_KERNEL 
#if defined(__FULL_PROTO)
int xmemdma(struct xmem *, caddr_t, int);
int xmemacc(struct xmem *, caddr_t, int);
#else
int xmemdma();				/* cross memory DMA setup */
/*	struct xmem *dp;	*/	/* cross memory descriptor */
/*	caddr_t	uaddr;	*/		/* address in other address space */
/*	int	flags;	*/		/* operation flags */
                                        /* (page can be read-only) */
/*	returns the real address */
int xmemacc();				/* cross memory access */
/*	struct xmem *dp;	*/	/* cross memory descriptor */
/*	caddr_t	raddr;	*/		/* real address */
/*      int modify;     */              /* non-zero if page modified */
#endif /* __FULL_PROTO */ 
#endif /* ! __64BIT_KERNEL */

#define XMEM_RDONLY	0	/* readonly access allowed	*/
#define XMEM_RDWR	1	/* read/write access allowed	*/
#define XMEM_NOACC	2	/* no access allowed		*/

#ifdef _KERNSYS

int xlate_create(struct xmem *, caddr_t baddr,  int32long64_t, uint); 
int xlate_remove(struct xmem *);
int xlate_pin(struct xmem *, caddr_t baddr, int32long64_t, int);
int xlate_unpin(struct xmem *, caddr_t, int32long64_t);
int xmemdma64_list(struct xmem *, caddr_t, int32long64_t, void *, int, int);
int xmemzero(struct xmem *, caddr_t, long);
#endif /* _KERNSYS */


#if defined(__64BIT_KERNEL)
void *xmemdma64(struct xmem *, caddr_t, int);	/* cross memory DMA setup */
#else
#if defined(_LONG_LONG)
#if defined(__FULL_PROTO)
unsigned long long xmemdma64(struct xmem *, caddr_t, int);	
#else
unsigned long long xmemdma64();		/* cross memory DMA setup */
#endif /* __FULL_PROTO */
#endif /* _LONG_LONG */
#endif /* __64BIT_KERNEL */
/*	struct xmem *dp;	*/	/* cross memory descriptor */
/*	char	*uaddr;	*/		/* address in other address space */
/*	int	flags;	*/		/* operation flags, same as */
					/* xmemdma() above */
                                        /* (page can be read-only) */
/*	returns the 64-bit real address */
 

#ifdef __64BIT_KERNEL
extern int xmem_getlocation(struct xmem *, caddr_t, size_t, int);
long long xmempsize(struct xmem *, void *, size_t);
#endif /* __64BIT_KERNEL */


/* RPN allocator structures and prototypes */

#ifdef _KERNEL

#define RPN_INVAL       -1
#define RPN_DR_INVAL    -2 

#endif /* _KERNEL */

#ifdef _KERNSYS
#include <sys/processor.h>
#include <sys/lock_def.h>
#include <sys/syspest.h>
#define CACHELINESIZE 128

struct xmfree_s {
	struct xmfree_s *next;
	char   mem_type;        /* type of memory */
};

struct rpn_header {
	struct rpn_list *__rpn_next;	/* next allocated list */
	struct rpn_list *__rpn_prev;	/* previous allocated list */
	int		__rpn_length;	/* length of array */
	cpu_t 		__rpn_cpu;
	char            __pad;
	char            __l2psize;      /* log2 of rpn-list psize */
} rpn_header_t;

typedef struct rpn_lock {
	Simple_lock	lock;
	/* pad to next cache line */
	char		fill[CACHELINESIZE - (sizeof(Simple_lock))];
} rpn_lock_t;

typedef struct rpn_list {
	struct rpn_header __rpn_header;
	struct xmfree_s   xmflist;      /* used when free'ing at interrupt level */
	rpn_t		  rpn_trans[1];	/* pre-translation of first page */
} rpn_list_t;

typedef struct rpn_anchor {
	struct rpn_list list;
	/* pad to next cache line */
	char		fill[CACHELINESIZE - (sizeof(struct rpn_list))];
} rpn_anchor_t;

#define rpn_next     __rpn_header.__rpn_next
#define rpn_prev     __rpn_header.__rpn_prev
#define rpn_length   __rpn_header.__rpn_length
#define rpn_cpu      __rpn_header.__rpn_cpu

/* Given a pointer to the start of a field in a rpn_list_t, determine the
 * address of the start of the rpn_list_t containing the field.
 */
#define RPNFLD_TO_LISTP(_ptr, _fld)  \
      ((rpn_list_t *)(((char *)(_ptr)) - ((char *)&(((rpn_list_t *)0)->_fld))))

#define RPN_CPU_LOCK(_cpunum)   simple_lock(&rpn_list_locks[_cpunum].lock)
#define RPN_CPU_UNLOCK(_cpunum) simple_unlock(&rpn_list_locks[_cpunum].lock)

#define RPN_REG(_rpnl, _rpnl_length, _cpunum)  {		\
  rpn_list_t *_rpnl_h = RPNFLD_TO_LISTP(_rpnl, rpn_trans);      \
  ASSERT(lock_mine(&rpn_list_locks[_cpunum].lock));		\
  if ((_rpnl_h->rpn_next = rpn_anchor[_cpunum].list.rpn_next) != NULL)	\
    _rpnl_h->rpn_next->rpn_prev = _rpnl_h;			\
  rpn_anchor[_cpunum].list.rpn_next = _rpnl_h;			\
  _rpnl_h->rpn_prev = &rpn_anchor[_cpunum].list;		\
  _rpnl_h->rpn_length = _rpnl_length;				\
  _rpnl_h->rpn_cpu = _cpunum;					\
}

#define RPN_UNREG(_rpnl)  {					\
  rpn_list_t *_rpnl_h = RPNFLD_TO_LISTP(_rpnl, rpn_trans);      \
  cpu_t _cpunum = _rpnl_h->rpn_cpu;				\
  ASSERT(lock_mine(&rpn_list_locks[_cpunum].lock));		\
  if ((_rpnl_h->rpn_prev->rpn_next = _rpnl_h->rpn_next) != NULL)	\
    _rpnl_h->rpn_next->rpn_prev = _rpnl_h->rpn_prev;		\
}

extern rpn_anchor_t *rpn_anchor;
extern rpn_lock_t *rpn_list_locks;
void rpn_init(void);
void rpn_inval(rpn_t, rpn_t);
rpn_t *rpn_alloc(int, ushort);
void rpn_free(rpn_t *);
#endif /* _KERNSYS */

#endif /* _H_XMEM */

