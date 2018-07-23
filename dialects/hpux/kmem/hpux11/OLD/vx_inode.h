/*
 * @(#) vx_inode.h $Date: 1998/12/15 09:26:35 $Revision: r11ros/cup_ros_ep1_pb/3 PATCH_11.00 (PHKL_17205)
 */
/*
 * @(#)vx_inode.h: $Revision: 1.10.106.4 $ $Date: 97/09/29 20:24:52 $
 * $Locker:  $
 */

/* @(#)src/kernel/vxfs/vx_inode.h	2.44 15 Oct 1996 16:31:55 -  */
/* #ident	"@(#)fshp:src/kernel/vxfs/vx_inode.h	2.44" */

/*
 *  (c) Copyright 1993-1997 VERITAS Software Corporation.  All
 *  rights reserved.
 *
 *               *** COMPANY CONFIDENTIAL ***
 *
 *  This media contains information which is proprietary
 *  and confidential to VERITAS Software Corporation.  It
 *  is not to be disclosed to anyone outside this organization.
 *
 *                      *** NOTE ***
 *
 *  If you are NOT an employee or contractor of Hewlett-Packard
 *  Company or HP's subsidiaries, the standard HP-UX source code
 *  license does NOT grant you a license to modify this software.
 *  If you want to modify this software, please contact HP for
 *  further details about a separate, royalty-bearing license.
 */

#ifndef	_FS_VXFS_VX_INODE_H
#define	_FS_VXFS_VX_INODE_H

/*
 * The disk structure of the link count table.
 */

union vx_lctdisk {
	vx_u32_t		lcd_entry;	/* link count table entry */
	struct vx_lctval	lcd_fields;	/* fields in entry */
};

#define	lcd_free	lcd_fields.lcf_highbit
#define	lcd_count	lcd_fields.lcf_lowbits

/*
 * The mlink structure links asynchronous updates to transactions.
 * The mlinkhd structure is used as a list header, it must match
 * the start of the mlink structure.
 */

struct vx_mlinkhd {
	struct vx_mlink	*ml_forw;	/* forw link of inode/map list */
	struct vx_mlink	*ml_back;	/* back link of inode/map list */
};

struct vx_mlink {
	struct vx_mlink	*ml_forw;	/* 0x00 forw link of inode/map list */
	struct vx_mlink	*ml_back;	/* 0x04 back link of inode/map list */
	struct vx_mlink *ml_fsforw;	/* 0x08 forw link on fs_mlink queue */
	struct vx_mlink *ml_fsback;	/* 0x0c back link on fs_mlink queue */
	struct vx_tran	*ml_tranp;	/* 0x10 pointer to tran for mlink */
	vx_um32_t	ml_flag;	/* 0x14 flags */
	struct vx_fs	*ml_fs;		/* 0x18 file system structure */
	union vx_mlspec {		/* 0x20 type specific info */
		struct vx_mli {		/* used for VX_MLINODE mlinks */
			struct vx_inode	*mi_ip;	    /* 0x20 pointer to inode */
		} ml_mli;
		struct vx_mlm {		/* used for VX_MLMAP mlinks */
			struct vx_map	*mm_map;    /* 0x20 pointer to map */
			struct vx_ctran	*mm_tcp;    /* 0x24 pointer to ctran */
			struct vx_emtran *mm_emp;   /* 0x28 pointer to emtran */
		} ml_mlm;
		struct vx_mlbh {	/* used for VX_MLBUFHEAD mlinks */
			struct vx_mlinkhd mh_link;  /* 0x20 mlinks for buf */
			struct vx_inode	*mh_ip;     /* 0x28 pointer to inode */
			struct vnode	*mh_iovp;   /* 0x3c ptr to io vnode */
			vx_off_t	mh_off;     /* 0x2c file offset */
			vx_um32_t	mh_len;     /* 0x34 length of buffer */
			daddr_t		mh_bno;     /* 0x38 bno of buffer */
		} ml_mlbh;
		struct vx_mlc {		/* used for VX_MLCUT mlinks */
			struct vx_fset	*mc_fset;   /* 0x20 pointer to fset */
			struct vx_mlink	*mc_link;   /* 0x24 mlinks for fset */
			struct vx_cut	*mc_cutp;   /* 0x28 pointer to cut */
			vx_hyper_t	mc_vers;    /* 0x2c version logged */
		} ml_mlc;
		struct vx_mll {		/* used for VX_MLLCT mlinks */
			struct vx_lct	*ml_lctp;   /* 0x20 pointer to lct */
		} ml_mll;
		struct vx_mldq {	/* used for VX_MLDQUOT mlinks */
			struct vx_dquot	*mdq_dquotp; /* 0x20 ptr to vx_dquot */
		} ml_mldq;
	} ml_ml;
					/* 0x38 is length */
};

#define	mli_ip		ml_ml.ml_mli.mi_ip
#define	mlm_map		ml_ml.ml_mlm.mm_map
#define	mlm_tcp		ml_ml.ml_mlm.mm_tcp
#define	mlm_emp		ml_ml.ml_mlm.mm_emp
#define	mlbh_link	ml_ml.ml_mlbh.mh_link
#define	mlbh_ip		ml_ml.ml_mlbh.mh_ip
#define	mlbh_bno	ml_ml.ml_mlbh.mh_bno
#define	mlbh_iovp	ml_ml.ml_mlbh.mh_iovp
#define	mlbh_off	ml_ml.ml_mlbh.mh_off
#define	mlbh_len	ml_ml.ml_mlbh.mh_len
#define	mlc_fset	ml_ml.ml_mlc.mc_fset
#define	mlc_link	ml_ml.ml_mlc.mc_link
#define	mlc_cutp	ml_ml.ml_mlc.mc_cutp
#define	mlc_vers	ml_ml.ml_mlc.mc_vers
#define	mll_lctp	ml_ml.ml_mll.ml_lctp
#define	mldq_dquotp	ml_ml.ml_mldq.mdq_dquotp

/*
 * values for ml_flag
 */

#define	VX_MLTYPEMASK	0x00ff		/* mask of mlink types */
#define	VX_MLMAP	0x0001		/* mlink is for a map */
#define	VX_MLINODE	0x0002		/* mlink is for an inode */
#define	VX_MLBUFFER	0x0004		/* mlink is for async buffer */
#define	VX_MLBUFHEAD	0x0008		/* mlink is header for async buffer */
#define	VX_MLCUT	0x0010		/* mlink is for a cut structure */
#define	VX_MLLCT	0x0020		/* mlink is for an lct structure */
#define	VX_MLDATA	0x0040		/* mlink is for a logged write */
#define	VX_MLDQUOT	0x0080		/* mlink is for a vx_dquot structure */
#define	VX_MLAFLUSHED	0x0100		/* async flush attempted on mlink */
#define	VX_MLSFLUSHED	0x0200		/* sync flush attempted on mlink */
#define	VX_MLBFLUSHED	0x0400		/* delbuf flush attempted on mlink */
#define	VX_MLDELAYED	0x1000		/* map changes delayed until log done */
#define	VX_MLFSLIST	0x2000		/* mlink is on fsq list */

/*
 * Add/remove an entry to/from an mlink ml_forw/back list
 */

#define	VX_MLINK_ADD(mlink, nlink) { \
	(nlink)->ml_forw = (mlink)->ml_forw; \
	(nlink)->ml_back = (mlink); \
	(mlink)->ml_forw = (nlink); \
	(nlink)->ml_forw->ml_back = (nlink); \
}

#define	VX_MLINK_REM(mlink) { \
	(mlink)->ml_forw->ml_back = (mlink)->ml_back; \
	(mlink)->ml_back->ml_forw = (mlink)->ml_forw; \
	(mlink)->ml_forw = (mlink); \
	(mlink)->ml_back = (mlink); \
}

#define	VX_MLINK_FSADD(mlink, nlink) { \
	(nlink)->ml_fsforw = (mlink)->ml_fsforw; \
	(nlink)->ml_fsback = (mlink); \
	(mlink)->ml_fsforw = (nlink); \
	(nlink)->ml_fsforw->ml_fsback = (nlink); \
	(nlink)->ml_flag |= VX_MLFSLIST; \
}

#define	VX_MLINK_FSREM(mlink) { \
	(mlink)->ml_fsforw->ml_fsback = (mlink)->ml_fsback; \
	(mlink)->ml_fsback->ml_fsforw = (mlink)->ml_fsforw; \
	(mlink)->ml_fsforw = (mlink); \
	(mlink)->ml_fsback = (mlink); \
	(mlink)->ml_flag &= ~VX_MLFSLIST; \
}

/*
 * Move an mlink chain from one mlinkhd to another.
 */

#define	VX_MLINK_MOVE(ohd, nhd) { \
	XTED_MLINK_MOVE((struct vx_mlink *)&(nhd)); \
	(nhd) = (ohd); \
	(nhd).ml_forw->ml_back = (struct vx_mlink *)&(nhd); \
	(nhd).ml_back->ml_forw = (struct vx_mlink *)&(nhd); \
	(ohd).ml_forw = (struct vx_mlink *)&(ohd); \
	(ohd).ml_back = (struct vx_mlink *)&(ohd); \
}

#define	VX_USRQUOTA	0	/* user quotas */
#define	VX_GRPQUOTA	1	/* group quotas */
#define	VX_MAXQUOTAS	2	/* currently we support user and group quotas */

#define	NDADDR_N	10		/* direct addresses in inode */
#define	NIMMED_N	96		/* immediate data in inode */
#define	NIADDR		3		/* indirect addresses in inode */
#define	VX_NTYP4	6		/* num of 4 word typed exts in inode */
#define	VX_NTYP8	3		/* num of 8 word typed exts in inode */

/*
 * flags for ex_eflags
 */

#define	VX_EXF_NEW		0x0001	/* extent is newly allocated */
#define	VX_EXF_EXTEND		0x0002	/* OK to extend previous extent */

struct vx_extent1 {
	vx_off_t	ex1_off;	/* offset of extent in file */
	vx_off_t	ex1_sz;		/* size of extent in bytes */
	vx_daddr_t	ex1_st;		/* start block of extent */
	vx_uq_t		ex1_flags;	/* flags */
	vx_blkcnt_t	ex1_elen;	/* component extent size if concat */
	vx_devid_t	ex1_dev;	/* devid of extent */
};

struct vx_extent2 {
	vx_uq_t		ex2_boff;	/* offset within buffer */
	vx_uq_t		ex2_bsz;	/* size of buffer */
	vx_daddr_t	ex2_bst;	/* start block of buffer */
	vx_off_t	ex2_lbno;	/* logical start block within file */
};

struct vx_extent3 {
	struct vnode	*ex3_vnode;	/* file vnode of extent */
	vx_uq_t		ex3_level;	/* Indirect level */
	vx_daddr_t	ex3_blkno;	/* blkno of indirect address extent */
	vx_uq_t		ex3_bsize;	/* size of iaddr extent */
	vx_uq_t		ex3_index;	/* index of entry in blkno */
};

struct vx_extent {
	struct vx_extent1	ex1;
	struct vx_extent2	ex2;
	struct vx_extent3	ex3;
};

#define	ex_off		ex1.ex1_off
#define	ex_sz		ex1.ex1_sz
#define	ex_st		ex1.ex1_st
#define	ex_eflags	ex1.ex1_flags
#define	ex_elen		ex1.ex1_elen
#define	ex_dev		ex1.ex1_dev
#define	ex_boff		ex2.ex2_boff
#define	ex_bsz		ex2.ex2_bsz
#define	ex_bst		ex2.ex2_bst
#define	ex_lbno		ex2.ex2_lbno
#define	ex_vnode	ex3.ex3_vnode
#define	ex_level	ex3.ex3_level
#define	ex_blkno	ex3.ex3_blkno
#define	ex_ebsize	ex3.ex3_bsize
#define	ex_index	ex3.ex3_index

/*
 * Typed Extent Descriptors
 */

#define	VX_TYPE_IADDR_4		1	/* indirect address extent */
#define	VX_TYPE_DATA_4		2	/* data extent */
#define	VX_TYPE_IADDR_DEV4	3	/* indirect addr extent with altdev */
#define	VX_TYPE_DATA_DEV4	4	/* data extent with altdev */
#define	VX_TYPE_PLACEHOLD	5	/* placeholder for empty indirects */

#define	VX_TYPE_HOLE		98	/* hole - not valid on disk */
#define	VX_TYPE_NULL		99	/* null entry - not valid on disk */

struct vx_typed_generic {
	vx_hyper_t	te_hdr;		/* 0x00 header with type and offset */
};

struct vx_typed_4 {
	vx_hyper_t	te_hdr;		/* 0x00 header with type and offset */
	vx_daddr32_t	te_start;	/* 0x08 starting block of extent */
	vx_blkcnt32_t	te_size;	/* 0x0c size of extent */
					/* 0x10 is size */
};

struct vx_typed_dev4 {
	vx_hyper_t	te_hdr;		/* 0x00 header with type and offset */
	vx_daddr32_t	te_start;	/* 0x08 starting block of extent */
	vx_blkcnt32_t	te_size;	/* 0x0c size of extent */
	vx_u32_t	te_devid;	/* 0x10 device id */
	char		te_padding[12];	/* 0x14 pad to power of 2 size */
					/* 0x20 is size */
};

#define	te_type		te_hdr.vh_highbyte

#if _VXFS_OFFSET_TYPE == _VXFS_BIG_OFFSETS
#define	te_offset	te_hdr.vh_low7bytes
#endif

#if _VXFS_OFFSET_TYPE == _VXFS_LITTLE_OFFSETS
#define	te_offset	te_hdr.vh_low
#endif

/*
 * This structure is used to keep counts of free and used inodes
 * for use by sar.
 */

struct vx_fshead {
	struct vx_inode	**f_iptrs;	/* list of inode pointers */
	vx_uq_t		f_nptrs;	/* number of pointers allocated */
	vx_uq_t		f_limit;	/* absolute maximum number of inodes */
	vx_uq_t		f_max;		/* desired maximum number of inodes */
	vx_uq_t		f_curr;		/* number of inodes allocated */
	vx_uq_t		f_inuse;	/* number of inodes being used	*/
	vx_uq_t		f_free;		/* number of inodes on freelist */
	vx_uq_t		f_fail;		/* number of failures allocating */
	size_t		f_isize;	/* size of an inode structure */
	vx_uq_t		f_incr;		/* incremental allocation size */
	vx_uq_t		f_slop;		/* diff between f_limit and f_max */
};

/*
 * The head of the various inode free/dirty lists.
 *
 * The offset of av_forw and av_back must match struct vx_inode.
 */

struct vx_ifreehead {			/* must match struct vx_inode */
	struct vx_inode *i_filler[4];	/* 0x00 filler for hash chains */
	struct vx_inode *av_forw;	/* 0x10 freelist chain */
	struct vx_inode *av_back;	/* 0x14 " */
	struct vx_inode *i_dxw_forw;	/* 0x18 delayed extending write list */
	struct vx_inode *i_dxw_back;	/* 0x1c " */
	vx_uq_t		i_count;	/* 0x20 inodes on list */
	vx_uq_t		i_gencount;	/* 0x24 list generation count */
	struct vx_fshead *i_fshead;	/* 0x28 fshead struct for freelists */
	vx_spin_t	i_freelist_lk;
	struct	vx_ifreehead *i_next;
};

/*
 * The inode lock control structure.
 */

struct vx_ilock {
	struct vx_ilock	*l_forw;	/* next lock in hash chain */
	struct vx_ilock	*l_back;	/* previous lock in hash chain */
	struct vx_ias	*l_ias;		/* lock belongs to this ias chain */
	ino_t		l_ino;		/* inode number */
	vx_um32_t	l_count;	/* reference count */
	vx_rwsleep_t	l_rwlock;	/* read/write lock */
	vx_rwsleep_t	l_glock;	/* getpage lock */
	vx_rwsleep_t	l_plock;	/* putpage lock */
};

/*
 * The inode lock hash chain headers.
 */

struct vx_hilock {
	struct vx_ilock	*l_forw;	/* next lock in hash chain */
	struct vx_ilock	*l_back;	/* previous lock in hash chain */
	vx_spin_t	*l_lhash_lkp;
};

/*
 * The inode hash chain headers.
 */

struct vx_hinode {		/* must match struct vx_inode */
	struct vx_inode	*i_forw;
	struct vx_inode	*i_back;
	struct vx_inode	*i_bforw;
	struct vx_inode	*i_bback;
	struct vx_ifreehead *i_identlock;
	vx_spin_t *i_bhash_lkp;
};

struct vx_inode {
	struct vx_inode	*i_forw;   /* 0x00 inode hash chain */
	struct vx_inode *i_back;   /* 0x04 " */
	struct vx_inode *i_bforw;  /* 0x08 inode block hash chain */
	struct vx_inode *i_bback;  /* 0x0c " */
	struct vx_inode *av_forw;  /* 0x10 freelist chain */
	struct vx_inode *av_back;  /* 0x14 " */
	struct vx_inode *i_dxw_forw; /* 0x18 delayed extending write list */
	struct vx_inode *i_dxw_back; /* 0x1c " */
	struct vx_mlinkhd i_blink; /* 0x20 head of buffer mlinks */
	struct vx_mlinkhd i_dlink; /* 0x28 head of logged write mlinks */
	struct vx_mlinkhd i_mlink; /* 0x30 head of inode modification mlinks */
	struct vx_mlinkhd i_alink; /* 0x38 async iupdate mlinks */
	struct vx_inode	*i_aupd;   /* 0x40 async iupdate linked list */
	vx_um32_t	i_flag;		/* 0x44 flags - see below */
	vx_um32_t	i_sflag;	/* 0x48 flags protected by ispin lock */
	vx_um32_t	i_intrflag;	/* 0x4c flags protected by iintr lock */
	vx_off_t	i_nsize;	/* 0x50 IDELXWRI "new" bytes in file */
	vx_off_t	i_wsize;	/* 0x58 size of file if write suceeds */
	vx_off_t	i_dsize;	/* 0x60 max file inode size on disk */
	vx_off_t	i_errsize;	/* 0x68 lowest delxwri i/o error */
	vx_off_t	i_savesize;	/* 0x70 size at last delxwri flush */

#if	!defined(HASONLINEJFS)
	struct vx_fset	*i_fset;	/* 0x78 fileset of inode */
	struct vx_fs	*i_fs;		/* 0x7c aggregate of inode */
 	struct vx_ias	*i_ias;		/* 0x80 inode address space pointer */
#endif	/* !defined(HASONLINEJFS) */

	dev_t		i_dev;		/* 0x84 device where inode resides */
	ino_t		i_number;	/* 0x88 inode number */
	vx_um32_t	i_advise;	/* 0x8c advisories */
	vx_off_t	i_ndblocks;	/* 0x90 num blocks in direct extents */
	vx_off_t	i_diroff;	/* 0x98 offset last dir entry found */
	ino_t		i_lastino;	/* 0xa0 last inode alloced into dir */
	struct vx_extent1 i_ext;	/* 0xa8 last extent for bmap cache */
	vx_tranid_t	i_tranid;	/* 0xc8 tran id of last sync write */ 
	vx_tranid_t	i_max_lwrid;	/* 0xcc log id of last logged write */
	vx_off_t	i_nextr;	/* 0xd0 expected offset of next read */
	vx_off_t	i_nextio;	/* 0xd8 next read ahead I/O point */
	vx_off_t	i_raend;	/* 0xe0 where read ahead area ends */
	vx_off_t	i_nextw;	/* 0xe8 expected offset of next write */
	vx_off_t	i_wflush;	/* 0xf0 previous write flush point */
	vx_off_t	i_wseqstart;	/* 0xf8 start of sequential writes */
	vx_off_t	i_lastflushoff;	/* 0x100 last completed flush off */
	vx_off_t	i_wlen;		/* 0x108 len of writes */
	vx_um32_t	i_ralen;	/* 0x110 length of read ahead area */
	vx_um16_t	i_nodealloc_fsq;/* 0x118 don't dealloc; fsq lock */
	vx_um16_t	i_nodealloc_idxw; /* 0x11c don't dealloc; delxwri lck */
	vx_um16_t	i_nodealloc_ilist; /* 0x120 don't dealloc; ilist lock */
	vx_um8_t	i_inreuse;	/* 0x124 block vx_itryhold references */
	vx_um8_t	i_vinactive;	/* 0x154 inode is delayed inactive */
	vx_um16_t	i_nomapcnt;	/* 0x130 block creation of mappings */
	vx_um32_t	i_mapcnt;	/* 0x12c num of process file mappings */
	vx_um32_t	i_diocount;	/* 0x134 num procs using direct I/O */
	vx_um32_t	i_readcnt;	/* 0x138 num procs reading the file */
	caddr_t		i_swapmap;	/* 0x128 swap file info */
	caddr_t		i_attr;		/* 0x13c pointer to attribute area */
	size_t		i_attrlen;	/* 0x140 length of attribute area */
	size_t		i_afree;	/* 0x144 free space in attribute area */
	daddr_t		i_bno;		/* 0x134 ilist block - in sectors */
	vx_um32_t	i_boff;		/* 0x138 inode offset in in ilist buf */
	vx_um32_t	i_blen;		/* 0x13c length of ilist buffer */
	struct vx_ifreehead *i_freelist; /* 0x140 inode freelist pointer */
	struct vx_fset	*i_qfset;	/* 0x144 fset to charge for quotas */
	vx_um32_t	i_pageflushes;	/* 0x148 num of ongoing write flushes */
	vx_sec_t	i_ftime;	/* 0x14c time inode put on freelist */
	vx_um32_t	i_trangen;	/* 0x150 generation count of tran */
	struct vx_dquot *i_dquot[VX_MAXQUOTAS]; /* 0x158 quota information */
	struct vnode	*i_vnode;	/* 0x160 vnode for inode */
	struct vx_inode	*i_push;	/* 0x164 inode push chain */
	struct vx_inode	*i_overlay;	/* 0x168 inode overlay chain */
	vx_um32_t	i_holdcount;	/* 0x16c inode hold count */
	struct vx_bmapops *i_bops;	/* 0x170 bmap ops */
	vx_um32_t	i_rmtcnt;	/* 0x174 remote access count */
	struct vx_extent3 i_ext3;	/* 0x178 last extent3 for bmap cache */
	vx_um32_t	i_dxwgencnt;	/* 0x18c saved gen count for dxw list */

#if	defined(HASONLINEJFS)
	vx_um32_t OnlineJFS_d1[15];	/* OnlineJFS dummies */
#endif	/* defined(HASONLINEJFS) */

	struct	vx_icommon {		/* 0x190 disk inode */
		vx_u32_t	ic_mode;	/* 0x00: mode and type */
		vx_u32_t	ic_nlink;	/* 0x04: number of links */
		vx_u32_t	ic_uid;		/* 0x08: owner's user id */
		vx_u32_t	ic_gid;		/* 0x0c: owner's group id */
		vx_hyper_t	ic_size;	/* 0x10: "disk" num of bytes */
		vx_timeval32_t	ic_atime;	/* 0x18: time last accessed */
		vx_timeval32_t	ic_mtime;	/* 0x20: time last modified */
		vx_timeval32_t	ic_ctime;	/* 0x28: time ino last change */
		vx_u8_t		ic_aflags;	/* 0x30: allocation flags */
		vx_u8_t		ic_orgtype;	/* 0x31: org type */
		vx_u16_t	ic_eopflags;	/* 0x32: extended operations */
		vx_u32_t	ic_eopdata;	/* 0x34: extended operations */
		union vx_ftarea {		/* 0x38: type specific data */
		    struct vx_ftarea_dev {	    /* device files */
			vx_u32_t	ic_rdev;    /* 0x38: device number */
		    } ic_ftarea_dev;
		    struct vx_ftarea_dir {	    /* directories */
			vx_ino32_t	ic_dotdot;  /* 0x38: parent directory */
		    } ic_ftarea_dir;
		    struct vx_ftarea_reg {	    /* regular files */
			vx_blkcnt32_t	ic_reserve; /* 0x38: prealloc space */
			vx_blkcnt32_t	ic_fixextsize; /* 0x3c: fixed ext size*/
		    } ic_ftarea_reg;
		    struct vx_ftarea_vxspec {	    /* vxfs private files */
			vx_ino32_t	ic_matchino; /* 0x38: matching inode */
			vx_u32_t	ic_fsetindex; /* 0x3c: fileset index */
		    } ic_ftarea_vxspec;
		} ic_ftarea;
		vx_blkcnt32_t	ic_blocks;	/* 0x40: blocks actually held */
		vx_u32_t	ic_gen;		/* 0x44: generation number */
		vx_hyper_t ic_vversion;		/* 0x48: inode serial number */
		union vx_org {			/* 0x50: */
		    struct vx_immed {
			char		ic_immed[NIMMED_N]; /* 0x50 immediate */
		    } ic_vx_immed;
		    struct vx_ext4 {
			vx_blkcnt32_t	ic_ies;		/* 0x50: ind ext sz */
			vx_daddr32_t	ic_ie[NIADDR];	/* 0x54: indir exts */
			struct vx_dext {		/* 0x60: dir exts */
			    vx_daddr32_t	ic_de;	/* dir ext */
			    vx_blkcnt32_t	ic_des;	/* dir ext size */
			} ic_dext[NDADDR_N];
		    } ic_vx_e4;
		    struct vx_typed_4    ic_td4[VX_NTYP4]; /* typed exts */
		    struct vx_typed_dev4 ic_td8[VX_NTYP8]; /* typed exts */
		} ic_org;
		vx_ino32_t	ic_iattrino; /* 0xb0: indirect attribute ino */
		vx_u32_t	ic_noverlay; /* 0xb4: overlay count */
	} i_ic;
	vx_spin_t	i_intrlock;	/* 0x248 interrupt lock */
	vx_spin_t	i_spinlock;	/* 0x250 inode spin lock */
	vx_sleep_t	*i_ilock;	/* 0x258 inode lock */
	vx_rwsleep_t	*i_bmaplock;	/* 0x25c bmap lock */
	struct vx_ilock	*i_slocks;	/* 0x260 shared inode locks */
	struct kdm_vnode *i_hsm_kvnode;	/* 0x268 HSM_ */
	caddr_t		i_hsm_attr;	/* 0x26c HSM_ */
	vx_uq_t		i_hsm_attrlen;	/* 0x270 HSM_ */
	vx_u32_t	i_sgen;		/* 0x274 saved ic_gen */
	vx_hyper_t	i_volid;	/* 0x278 volume ID */
					/* 0x280 is length */
	struct	vx_ifreehead	*i_ilist;
	struct	vx_hinode	*i_bhash;
	struct	vx_hilock	*i_lhash;
};

#define	VX_KDM_ITOV(ip)	((struct kdm_vnode *)(ip)->i_hsm_kvnode)
#define	VX_KDM_VTOI(kp)	((struct vx_inode *)(kp)->kdv_data)

struct vx_dinode {
	struct vx_icommon	di_ic;
};

#define OLDINOSIZE	256		/* version 1 inode size */
#define OLDINODESHIFT	8		/* log base 2 of OLDINOSIZE */

#define	i_mode		i_ic.ic_mode
#define	i_nlink		i_ic.ic_nlink
#define	i_uid		i_ic.ic_uid
#define	i_gid		i_ic.ic_gid

#if _VXFS_OFFSET_TYPE == _VXFS_BIG_OFFSETS
#define i_size		i_ic.ic_size.vh_value
#endif
#if _VXFS_OFFSET_TYPE == _VXFS_LITTLE_OFFSETS
#define	i_size		i_ic.ic_size.vh_low
#endif

#define	i_atime		i_ic.ic_atime
#define	i_mtime		i_ic.ic_mtime
#define	i_ctime		i_ic.ic_ctime

#define	i_aflags	i_ic.ic_aflags
#define	i_orgtype	i_ic.ic_orgtype
#define	i_eopflags	i_ic.ic_eopflags
#define	i_eopdata	i_ic.ic_eopdata

#define	i_rdev		i_ic.ic_ftarea.ic_ftarea_dev.ic_rdev
#define	i_dotdot	i_ic.ic_ftarea.ic_ftarea_dir.ic_dotdot
#define	i_fixextsize	i_ic.ic_ftarea.ic_ftarea_reg.ic_fixextsize
#define	i_reserve	i_ic.ic_ftarea.ic_ftarea_reg.ic_reserve
#define	i_matchino	i_ic.ic_ftarea.ic_ftarea_vxspec.ic_matchino
#define	i_fsetindex	i_ic.ic_ftarea.ic_ftarea_vxspec.ic_fsetindex

#define	i_blocks	i_ic.ic_blocks
#define	i_gen		i_ic.ic_gen
#define	i_lversion	i_ic.ic_vversion.vh_low
#define	i_hversion	i_ic.ic_vversion.vh_high
#define	i_vversion	i_ic.ic_vversion

#define	i_immed		i_ic.ic_org.ic_vx_immed.ic_immed

#define	i_ies		i_ic.ic_org.ic_vx_e4.ic_ies
#define	i_ie		i_ic.ic_org.ic_vx_e4.ic_ie
#define	i_dext		i_ic.ic_org.ic_vx_e4.ic_dext

#define	i_td4		i_ic.ic_org.ic_td4
#define	i_td8		i_ic.ic_org.ic_td8

#define	i_iattrino	i_ic.ic_iattrino

#define	i_noverlay	i_ic.ic_noverlay


#define	di_mode		di_ic.ic_mode
#define	di_nlink	di_ic.ic_nlink
#define	di_uid		di_ic.ic_uid
#define	di_gid		di_ic.ic_gid
#define	di_eopflags	di_ic.ic_eopflags

#if _VXFS_OFFSET_TYPE == _VXFS_BIG_OFFSETS
#define di_size		di_ic.ic_size.vh_value
#endif
#if _VXFS_OFFSET_TYPE == _VXFS_LITTLE_OFFSETS
#define	di_size		di_ic.ic_size.vh_low
#endif
#define	di_size2	di_ic.ic_size.vh_high

#define	di_atime	di_ic.ic_atime
#define	di_mtime	di_ic.ic_mtime
#define	di_ctime	di_ic.ic_ctime

#define	di_aflags	di_ic.ic_aflags
#define	di_orgtype	di_ic.ic_orgtype
#define	di_eopflags	di_ic.ic_eopflags
#define	di_eopdata	di_ic.ic_eopdata

#define	di_rdev		di_ic.ic_ftarea.ic_ftarea_dev.ic_rdev
#define	di_dotdot	di_ic.ic_ftarea.ic_ftarea_dir.ic_dotdot
#define	di_fixextsize	di_ic.ic_ftarea.ic_ftarea_reg.ic_fixextsize
#define	di_reserve	di_ic.ic_ftarea.ic_ftarea_reg.ic_reserve
#define	di_matchino	di_ic.ic_ftarea.ic_ftarea_vxspec.ic_matchino
#define	di_fsetindex di_ic.ic_ftarea.ic_ftarea_vxspec.ic_fsetindex

#define	di_blocks	di_ic.ic_blocks
#define	di_gen		di_ic.ic_gen
#define	di_lversion	di_ic.ic_vversion.vh_low
#define	di_hversion	di_ic.ic_vversion.vh_high
#define	di_vversion	di_ic.ic_vversion

#define	di_immed	di_ic.ic_org.ic_vx_immed.ic_immed

#define	di_ies		di_ic.ic_org.ic_vx_e4.ic_ies
#define	di_ie		di_ic.ic_org.ic_vx_e4.ic_ie
#define	di_dext		di_ic.ic_org.ic_vx_e4.ic_dext

#define	di_td4		di_ic.ic_org.ic_td4
#define	di_td8		di_ic.ic_org.ic_td8

#define di_iattrino	di_ic.ic_iattrino
#define	di_noverlay	di_ic.ic_noverlay

/*
 * flags in i_flag
 */

#define	IUPD		0x00000001	/* file has been modified */
#define	IACC		0x00000002	/* inode access time to be updated */
#define	IMOD		0x00000004	/* inode has been modified */
#define	ICHG		0x00000008	/* inode has been changed */
#define	IATIMEMOD	0x00000010	/* atime modified */
#define	IMTIMEMOD	0x00000020	/* mtime modified */
#define	ITRANLAZYMOD	0x00000040	/* transaction lazy mod */
#define	ICLOSED		0x00000100	/* delxwri file was closed */
#define	IFLUSHED	0x00000200	/* delxwri file was flushed */
#define	ISHORTENED	0x00000400	/* file has been shortened */
#define	ISYNCWRITES	0x00000800	/* file has had O_SYNC writes */
#define	IINTRANS	0x00001000	/* inode is in a transaction */
#define	IDELBUF		0x00002000	/* inode has pending delayed buffers */
#define	IGHOST		0x00004000	/* inode is a ghost attribute */
#define	IINACTIVE	0x00008000	/* inode is inactive */
#define	IINACTIVATING	0x00010000	/* inode is geting inactivated */
#define ISHRDATA	0x00020000	/* don't delete data extents */
#define	INOTIMEMOD	0x00100000	/* no time update on invis i/o */
#define IBUSY		0x00200000	/* busy inode */
#define ISTALE		0x00400000	/* stale inode */
#define IGONE		0x00800000	/* deleted inode */

/*
 * flags in i_intrflag
 */

#define	IDELXWRI	0x0001		/* inode has delxwri data */
#define	IDELXWRIERR	0x0002		/* delxwri write has failed */
#define	ILOGWRITE	0x0004		/* inode has pending logged writes */
#define	IDELAYEDERR	0x0008		/* delayed update error occurred */
#define	IPUTERROR	0x0010		/* putpage error occurred */
#define	ILOGWRIFLUSH	0x0020		/* logged write flush occurred */
#define	IASYNCUPD	0x0040		/* async inode write in progress */

/*
 * flags in i_sflag
 */

#define	IADDRVALID	0x00000001	/* bmap has validated address map */
#define	IBAD		0x00000002	/* inode is bad */
#define	IUEREAD		0x00000004	/* getpage did an unexpected read */
#define	INOBMAPCACHE	0x00000008	/* bmap cache is disabled */
#define	IBADUPD		0x00000010	/* must be marked bad to disk */
#define	IPUTTIME	0x00000020	/* B_FORCE putpage mtime update */
#define	IATTRREM	0x00000040	/* remove attribute on inactive */
#define	INOBMAPCLUSTER	0x00000080	/* prevent bmap from clustering */
#define	IDEVACC		0x00000100	/* update access time on dev/fifo */
#define	IDEVUPDCHG	0x00000200	/* update mod/chg time on dev/fifo */
#define	IXWRITE		0x00000400	/* file has been written flag */
#define	IWRITEMAP	0x00000800	/* file has writable mappings */
#define	IPLOCK		0x00001000	/* ilock the push inode */

/*
 * Misc clone flags
 */

#define	VX_EOVERLAY	0x0001		/* Setup the next overlay inode */
#define VX_EPUSH	0x0002		/* Setup the push inode */

/*
 * Iget flags
 */

#define VX_IG_AFLAG	0x0001		/* Set full fsck flag indication */
#define VX_IG_IHOLD	0x0002		/* Place a private hold on inode */

/*
 * File modes.  File types above 0xf000 are vxfs internal only, they should
 * not be passed back to higher levels of the system.  vxfs file types must
 * never have one of the regular file type bits set.
 */

#define	VX_SIFMT	0x0000f000	/* system file types */
#define	VX_VXIFMT	0xff000000	/* vxfs internal file types */
#define	IFMT		0xff00f000	/* type of file */

#define	IFIFO		0x00001000	/* 0010000 named pipe (fifo) */
#define	IFCHR		0x00002000	/* 0020000 character special */
#define	IFDIR		0x00004000	/* 0040000 directory */
#define	IFNAM		0x00005000	/* 0050000 xenix special file */
#define	IFBLK		0x00006000	/* 0060000 block special */
#define	IFREG		0x00008000	/* 0100000 regular */
#define	IFLNK		0x0000a000	/* 0120000 symbolic link */
#define	IFSOC		0x0000c000	/* 0140000 socket */

#define	IFFSH		0x10000000	/* file set header */
#define	IFILT		0x20000000	/* inode list */
#define	IFIAU		0x30000000	/* inode allocation unit */
#define	IFCUT		0x40000000	/* current usage table */
#define	IFATT		0x50000000	/* attribute inode */
#define	IFLCT		0x60000000	/* lint count table */
#define	IFIAT		0x70000000	/* indirect attribute file */
#define	IFEMR		0x80000000	/* extent map reorg file */
#define	IFQUO		0x90000000	/* bsd quota file */

#define	IFLAB		0x11000000	/* device label file */
#define	IFOLT		0x12000000	/* object location table file */
#define	IFLOG		0x13000000	/* log file */
#define	IFEMP		0x14000000	/* extent map file */
#define	IFEAU		0x15000000	/* extent allocation unit file */
#define	IFAUS		0x16000000	/* extent au summary file */
#define	IFDEV		0x17000000	/* device configuration file */

#define	ISUID		0x0800		/* 04000 set user id on execution */
#define	ISGID		0x0400		/* 02000 set group id on execution */
#define	ISVTX		0x0200		/* 01000 sticky bit */
#define	IREAD		0x0100		/* 0400 read permission */
#define	IWRITE		0x0080		/* 0200 write permission */
#define	IEXEC		0x0040		/* 0100 execute/search permission */

#define	VX_GEMODE	(ISGID | (IEXEC >> 3))	/* setgid + group exec mode */
#define	VX_EXECMODES	(IEXEC | (IEXEC >> 3) | (IEXEC >> 6))

/*
 * File types for which BSD quotas may be enforced.
 */

#define VX_BSD_QUOT	(IFIFO | IFCHR | IFBLK | IFDIR | IFREG | IFLNK | \
			 IFSOC | IFATT | IFIAT)

/*
 * org types
 */

#define	IORG_NONE	0		/* inode has no format */
#define	IORG_EXT4	1		/* inode has 4 byte data block addrs */
#define	IORG_IMMED	2		/* data stored in inode */
#define	IORG_TYPED	3		/* inode has typed extent descriptors */

/*
 * allocation flags (char)
 */

#define	VX_AF_MASK	0x7f		/* allocation flags mask */
#define	VX_AF_IBAD	0x01		/* inode is bad */
#define	VX_AF_NOEXTEND	0x02		/* file can't be auto extended */
#define	VX_AF_ALIGN	0x04		/* all extents must be aligned */
#define	VX_AF_NOGROW	0x08		/* file can't grow */
#define	VX_AF_ALTDIR	0x10		/* directory contains altdirects */
#define	VX_AF_COMPRESS	0x20		/* file is compressed */
#define	VX_AF_NOCLONE	0x40		/* no clone shared data */

/*
 * extop flags
 */

#define	VX_IEMASK	0x03ff		/* extended op masks */
#define	VX_IEREMOVE	0x0001		/* deferred inode removal */
#define	VX_IETRUNC	0x0002		/* extended truncation */
#define	VX_IERTRUNC	0x0004		/* trim blocks down to i_reserve */
#define	VX_IESHORTEN	0x0008		/* shorten file to i_size */
#define	VX_IEZEROEXT	0x0010		/* zeroing an extent */
#define	VX_IETRIM	0x0020		/* trim reservation to i_size */
#define	VX_IEZLC	0x0040		/* zero link count file */
#define	VX_IEHOLETRUNC	0x0080		/* "punch hole" truncation */
#define	VX_IEREORG	0x0100		/* inode undergoing reorg */
#define VX_IEPTTRUNC    0x0200          /* extended "pass-thru" truncation */

/*
 * vxfs specific errno values
 */

#define	VX_ERETRY	0xf001	/* iget must be retried */
#define	VX_ENOINODE	0xf002	/* out of inodes */
#define	VX_ENOENT	0xf003	/* ENOENT ret from vx_dirscan or vx_attr_rm */
#define	VX_EAGMOUNT	0xf004	/* aggregate mount was successful */
#define	VX_EMAXTRUNC	0xf005	/* truncation must be retried */
#define VX_EMAXALLOC	0xf006	/* too many allocations in transaction */
#define	VX_ERWLOCK	0xf007	/* need a RWLOCK in transaction */
#define VX_EMAXHOLD	0xf008	/* too many held inodes in the transaction */
#define VX_EQUOTRETRY	0xf009	/* block allocation needed in quota file */
#define VX_ETRANRES	0xf00a	/* transaction reservation commit retry */

/*
 * Convert between inode pointers and vnode pointers
 */

#define VX_VTOI(vp)	((struct vx_inode *)(vp)->v_data)
#define	VX_ITOV(ip)	((ip)->i_vnode)

#define	VX_HOLE		((vx_daddr_t)-1)	/* value used when no block allocated */
#define	VX_OVERLAY	((vx_daddr_t)-2)	/* value used with overlay descriptor */
#define	VX_BLOCK0	((vx_daddr_t)-3)	/* value for block 0 on disk */

/*
 * Lock and unlock inodes.
 */

#define	VX_IRWLOCK(ip, mode)		vx_irwlock(ip, mode)
#define	VX_IRWLOCK_TRY(ip, mode)	vx_irwlock_try(ip, mode)
#define	VX_IRWLOCK_AVAIL(ip, mode)	vx_irwlock_avail(ip, mode)
#define	VX_IRWUNLOCK(ip)		vx_irwunlock(ip)

#define	VX_IGLOCK(ip, mode)		vx_iglock(ip, mode)
#define VX_IGLOCK_TRY(ip, mode)		vx_iglock_try(ip, mode)
#define	VX_IGLOCK_AVAIL(ip, mode)	vx_iglock_avail(ip, mode)
#define	VX_IGUNLOCK(ip)			vx_igunlock(ip)

#define	VX_IPLOCK(ip, mode)		vx_iplock(ip, mode)
#define VX_IPLOCK_TRY(ip, mode)		vx_iplock_try(ip, mode)
#define	VX_IPLOCK_AVAIL(ip, mode)	vx_iplock_avail(ip, mode)
#define	VX_IPUNLOCK(ip)			vx_ipunlock(ip)

#define	VX_ILOCK(ip)			vx_ilock(ip)
#define	VX_ILOCK_TRY(ip)		vx_ilock_try(ip)
#define	VX_ILOCK_AVAIL(ip)		vx_ilock_avail(ip)
#define	VX_IUNLOCK(ip)			vx_iunlock(ip)
#define	VX_IUNLOCK_NOFLUSH(ip)		vx_iunlock_noflush(ip)

#define	VX_IBMAPLOCK(ip, mode)		vx_ibmaplock(ip, mode)
#define	VX_IBMAPLOCK_TRY(ip, mode)	vx_ibmaplock_try(ip, mode)
#define	VX_IBMAPLOCK_AVAIL(ip, mode)	vx_ibmaplock_avail(ip, mode)
#define	VX_IBMAPUNLOCK(ip)		vx_ibmapunlock(ip)

#define	VX_ISPIN_LOCK(ip, ipl) {				\
	XTED_ISPIN_LOCK1(ip);					\
	VX_SPIN_LOCK(&(ip)->i_spinlock, VX_SPLBASE, &ipl);	\
	XTED_ISPIN_LOCK2(ip);					\
}

#define	VX_ISPIN_UNLOCK(ip, ipl) {				\
	XTED_ISPIN_UNLOCK(ip, ipl);				\
	VX_SPIN_UNLOCK(&(ip)->i_spinlock, ipl);			\
}

#define	VX_IINTR_LOCK(ip, ipl) {				\
	VX_SPIN_LOCK(&(ip)->i_intrlock, VX_SPLHI, &ipl);	\
	XTED_IINTR_LOCK(ip);					\
}

#define	VX_IINTR_UNLOCK(ip, ipl) {				\
	XTED_IINTR_UNLOCK(ip);					\
	VX_SPIN_UNLOCK(&(ip)->i_intrlock, ipl);			\
}

#define	VX_MISC_ILIST_LOCK(ipl) {				\
	VX_SPIN_LOCK(&vx_misc_ilist_lk, VX_SPLBASE, &ipl);	\
	XTED_ILIST_LOCK();					\
}

#define	VX_MISC_ILIST_UNLOCK(ipl) {				\
	XTED_ILIST_UNLOCK(ipl);					\
	VX_SPIN_UNLOCK(&vx_misc_ilist_lk, ipl);			\
}

#define VX_ILIST_LOCK(fl,ipl) {					\
	VX_SPIN_LOCK(&(fl)->i_freelist_lk, VX_SPLHI, &ipl);	\
}

#define VX_ILIST_UNLOCK(fl, ipl) {				\
	VX_SPIN_UNLOCK(&(fl)->i_freelist_lk, (ipl));		\
}

#define VX_ILHASH_LOCK(lip, ipl) {				\
	VX_SPIN_LOCK((lip)->l_lhash_lkp, VX_SPLHI, &ipl);	\
}

#define VX_ILHASH_UNLOCK(lip, ipl) {				\
	VX_SPIN_UNLOCK((lip)->l_lhash_lkp, (ipl));		\
}

#define VX_IBHASH_LOCK(hip, ipl)   {				\
	VX_SPIN_LOCK((hip)->i_bhash_lkp, VX_SPLHI, &ipl);	\
}

#define VX_IBHASH_UNLOCK(hip, ipl)  {				\
	VX_SPIN_UNLOCK((hip)->i_bhash_lkp, (ipl));		\
}

/*
 * The inode pointers lock protects the array of pointers to vxfs inodes.
 * While held exclusively, memory for inodes can be allocated or freed.
 * While held shared, the number of incore vxfs inodes and array of
 * pointers to them is stable.
 */

#define	VX_IPTRS_LOCK(mode) {					\
	XTED_IPTRS_LOCK(mode);					\
	VX_RWSLEEP_LOCK(&vx_iptrs_slk, mode);			\
}

#define VX_IPTRS_LOCK_TRY(mode)         vx_iptrs_lock_try(mode)

#define	VX_IPTRS_UNLOCK() {					\
	XTED_IPTRS_UNLOCK();					\
	VX_RWSLEEP_UNLOCK(&vx_iptrs_slk);			\
}

/*
 * Force any delayed writes on the inode to complete.
 */

#define	VX_DELBUF_FLUSH(ip) {			\
	if ((ip)->i_flag & IDELBUF) {		\
		vx_delbuf_flush(ip);		\
	}					\
}

#define	VX_LOGWRI_FLUSH(ip) {			\
	if ((ip)->i_intrflag & ILOGWRITE) {	\
		vx_logwri_flush(ip);		\
	}					\
}

#define	VX_DELXWRI_FLUSH(ip, mode) {					\
	if ((ip)->i_intrflag & IDELXWRI) {				\
		vx_idelxwri_flush((ip), (mode));			\
	}								\
}

/*
 * Find advisories in effect for current process
 */

#define	VX_ADVISEGET(ip)	((ip)->i_advise)

/*
 * Clean up advisories on close
 *
 * When this macro is invoked in vx_inactive(), the reference count
 * should be 1 (invoked by vx_inactive() as the last reference is
 * being released) or 0 (invoked by vx_idrop()).
 */

#define	VX_ADVISECLOSE(ip) {						\
	XTED_ADVISECLOSE(ip);						\
	(ip)->i_advise = 0;						\
}

/*
 * Increment and decrement the swap lock
 */

#define	VX_SWLOCKINC()
#define	VX_SWLOCKDEC()

/*
 * Swap lock and unlock ourselves if this is a swap vnode
 */

#define	VX_SWLOCK_VP(vp)
#define	VX_SWUNLOCK_VP(vp)

/*
 * System time interface.
 */

#define	vx_lbolt		time.tv_sec

/*
 * Put an inode on the end of the free list.
 */

#define	VX_IPFREE(ip) {						\
	XTED_IPFREE(ip);					\
	(ip)->i_freelist->av_back->av_forw = (ip);		\
	(ip)->av_forw = (struct vx_inode *)(ip)->i_freelist;	\
	(ip)->av_back = (ip)->i_freelist->av_back;		\
	(ip)->i_freelist->av_back = (ip);			\
	(ip)->i_freelist->i_fshead->f_inuse--;			\
	(ip)->i_freelist->i_fshead->f_free++;			\
	(ip)->i_freelist->i_count++;				\
	VX_MPCNTR(activeinodes, -1);				\
}

/*
 * Put an inode on the front of the free list.
 */

#define	VX_IPFFREE(ip) {					\
	XTED_IPFREE(ip);					\
	(ip)->i_freelist->av_forw->av_back = (ip);		\
	(ip)->av_back = (struct vx_inode *)(ip)->i_freelist;	\
	(ip)->av_forw = (ip)->i_freelist->av_forw;		\
	(ip)->i_freelist->av_forw = (ip);			\
	(ip)->i_freelist->i_fshead->f_inuse--;			\
	(ip)->i_freelist->i_fshead->f_free++;			\
	(ip)->i_freelist->i_count++;				\
	VX_MPCNTR(activeinodes, -1);				\
}

/*
 * Remove an inode from the freelist.
 */

#define	VX_IUNFREE(ip) {					\
	(ip)->av_back->av_forw = (ip)->av_forw;			\
	(ip)->av_forw->av_back = (ip)->av_back;			\
	(ip)->av_forw = 0;					\
	(ip)->av_back = 0;					\
	(ip)->i_freelist->i_fshead->f_inuse++;			\
	(ip)->i_freelist->i_fshead->f_free--;			\
	(ip)->i_freelist->i_count--;				\
	(ip)->i_freelist = 0;					\
	VX_MPCNTR(activeinodes, 1);				\
}



/*
 * Put an inode on a hash list.
 */

#define	VX_IPHASH(hip, ip) {					\
	(hip)->i_forw->i_back = (ip);				\
	(ip)->i_forw = (hip)->i_forw;				\
	(hip)->i_forw = (ip);					\
	(ip)->i_back = (struct vx_inode *)(hip);		\
}

/*
 * Remove an inode from its hash list.  To allow multiple calls,
 * the forward and back hash pointers point back to the inode.
 */

#define	VX_IUNHASH(ip) {					\
	(ip)->i_back->i_forw = (ip)->i_forw;			\
	(ip)->i_forw->i_back = (ip)->i_back;			\
	(ip)->i_forw = (ip);					\
	(ip)->i_back = (ip);					\
}

/*
 * Put an inode on a block hash list.
 */

#define	VX_IPBHASH(hip, ip) {					\
	(hip)->i_bforw->i_bback = (ip);				\
	(ip)->i_bforw = (hip)->i_bforw;				\
	(hip)->i_bforw = (ip);					\
	(ip)->i_bback = (struct vx_inode *)(hip);		\
}

/*
 * Remove an inode from its block hash list.  To allow multiple calls,
 * the forward and back hash pointers point back to the inode.
 */

#define	VX_IUNBHASH(ip) {					\
	(ip)->i_bback->i_bforw = (ip)->i_bforw;			\
	(ip)->i_bforw->i_bback = (ip)->i_bback;			\
	(ip)->i_bforw = (ip);					\
	(ip)->i_bback = (ip);					\
}

/*
 * Put an inode lock on a hash list.
 */

#define	VX_LPHASH(hlp, lp) {					\
	(hlp)->l_forw->l_back = (lp);				\
	(lp)->l_forw = (hlp)->l_forw;				\
	(hlp)->l_forw = (lp);					\
	(lp)->l_back = (struct vx_ilock *)(hlp);		\
}

/*
 * Remove an inode lock from its hash list.  
 */

#define	VX_LRHASH(lp) {						\
	(lp)->l_back->l_forw = (lp)->l_forw;			\
	(lp)->l_forw->l_back = (lp)->l_back;			\
	(lp)->l_forw = (lp);					\
	(lp)->l_back = (lp);					\
}

/*
 * Hash an inode based on device, inode list, and inode number or
 * device and inode list block number.
 */

#define	VX_IHASH(idx, iltype, dev, ino)	\
		(vx_hinode + ((((int)(dev) << 4) + (int)(dev) + \
			       ((int)(idx) << 3) - (int)(idx) + \
				(int)(iltype) + (int)(ino)) & (vx_nhino - 1)))

#define	VX_IBHASH(dev, blkno)	\
		(vx_hinode + ((((int)(dev) << 4) + (int)(dev) + \
				(int)(blkno)) & (vx_nhino - 1)))

#define	VX_LHASH(idx, iltype, dev, ino)	\
		(vx_hilock + ((((int)(dev) << 4) + (int)(dev) + \
			      ((int)(idx) << 3) - (int)(idx) + \
			       (int)(iltype) + (int)(ino)) & (vx_nhino - 1)))

/*
 * Hold a vnode that may have a delayed inactive pending.  If the vnode
 * is a delayed inactive, then take that hold and clear the inactive.
 */

#define	VX_VNHOLD(ip, vp) {						\
	u_long	context;						\
	lock_t	*sv_lock;						\
	MP_H_SPINLOCK_USAV(vn_h_sl_pool, (vp), &sv_lock, &context);	\
	if ((ip)->i_vinactive) {					\
		(ip)->i_vinactive = 0;					\
		(ip)->i_ftime = 0;					\
	} else {							\
		(vp)->v_count++;					\
	}								\
	MP_SPINUNLOCK_USAV(sv_lock, context);				\
}



union vx_ipargs {
	struct vx_ipchkver {
		vx_hyper_t		version;
	} ipchkser;
	struct vx_ipquotacheck {
		struct vx_fset		*fset;
		vx_uq_t			type;
		uid_t			*newidlist;
		vx_uq_t			newidcount;
		vx_uq_t			newidlimit;
		vx_uq_t			dqheld;
	} ipquotacheck;
};

/*
 * This overlays struct fid (see h/vfs.h).
 */

struct vx_vfid {
	uint16_t	vfid_len;
	vx_ino32_t	vfid_ino;
	vx_u32_t	vfid_gen;
};

#define MAKEIMODE(x, y) ((vx_vttoif(x)) | ((y) & (IFMT | 0xfff)))

/*
 * Structure used by resize to keep track of changes to structural inodes.
 */

struct vx_rsi {
	struct vx_inode	*rsi_ip;
	vx_off_t	rsi_osize;
	vx_off_t	rsi_nsize;
};

/*
 * The attribute code that modifies attributes, particular the attribute
 * inheritance code, requires a number of inode locks.  Some must be
 * appropriately cleaned up/reacquired when a transaction commits and
 * all must be released when the operation completes (an operation may
 * require multiple transactions).
 *
 * See vx_attr.c for a description of attribute locking.  The fields
 * are arranged in the order that locks are acquired.  All non_NULL
 * fields represent inodes that are held and locked.  The locks are:
 *
 * Field	Lock	Explanation
 *
 * vt_dirinip	rwlock	indirect attribute inode for "parent" inode
 * vt_preinip	rwlock	pre-existing indirect attribute inode for target
 * vt_preaip	rwlock	pre-existing attribute inode for target
 * vt_newaip	ilock	newly allocated (or modified) attribute inode
 *			for target
 * vt_newinip	ilock	newly allocated (or modified) indirect attribute
 *			inode for target
 * vt_oldaip	none	an old attribute inode removed in the transaction
 *
 * vt_preinip and vt_newinip may refer to the same inode, if it was
 * pre-existing and modified in the transaction.  vt_preaip and
 * vt_newaip may also refer to the same inode, typically when the
 * inode rquires a multi-transaction allocation.
 */

struct vx_attrlocks {
	struct vx_inode *vt_dirinip;	/* directory (parent) indirect inode */
	struct vx_inode *vt_preinip;	/* pre-existing indirect inode */
	struct vx_inode	*vt_preaip;	/* pre-existing attribute inode */
	struct vx_inode	*vt_newaip;	/* new attribute inode */
	struct vx_inode	*vt_newinip;	/* new indirect attribute inode */
	struct vx_inode *vt_oldaip;	/* old (removed) attribute inode */
};

/*
 * Parameters passed through dircreate for inheritance.
 */

struct vx_inherit {
	struct vx_attrlocks	vh_alocks;	/* 0x0 attribute locks */
	struct vx_inode		*vh_linklist[5]; /* 0x18 attr inodes linked */
	vx_uq_t			vh_nlink;	/* 0x2c num of entries */
	struct vx_tran		*vh_tranp;	/* 0x30 current transaction */
	vx_uq_t			vh_onetran;	/* 0x34 fast inheritance */
	uid_t			vh_uid;		/* 0x38 uid to be allocated */
	gid_t			vh_gid;		/* 0x3c gid to be allocated */
	vx_uq_t			vh_tranbytes;	/* 0x40 tranbytes needed */
};

/*
 * Structure for inode attribute area (on disk)
 */

struct vx_iattr {
	vx_u32_t	a_format;	/* format of attribute entry */
	vx_u32_t	a_length;	/* length of attribute entry */
	char		a_data[4];	/* attribute data (expandable) */
};

#define	VX_ATTROVERHEAD		(sizeof (struct vx_iattr) - 4)

/*
 * Structure for immediate attributes (on disk)
 */

struct vx_attr_immed {
	vx_u32_t	aim_class;	/* class of attribute */
	vx_u32_t	aim_subclass;	/* subclass of attribute */
	char		aim_data[4];	/* immediate data (expandable) */
};

#define	VX_ATTR_IMMEDOVER	(sizeof (struct vx_attr_immed) - 4)

/*
 * Structure for directly indexed attributes (on disk)
 */

struct vx_attr_direct {
	vx_u32_t	ad_class;	/* class of attribute */
	vx_u32_t	ad_subclass;	/* subclass of attribute */
	vx_hyper_t	ad_len;		/* length of attribute */
	vx_ino32_t	ad_ino;		/* inode containing attribute data */
};

#define	ad_length	ad_len.vh_low

/*
 * Structure for listing attributes.
 */

struct vx_attrlist {
	vx_uq_t		al_class;	/* class of attribute */
	vx_uq_t		al_subclass;	/* subclass of attribute */
	vx_off_t	al_length;	/* length of attribute */
};

/*
 * Arguments to VX_ATTRLIST ioctl.
 */

struct vx_albuf {
	size_t		len;		/* length of buffer */
	caddr_t		bufp;		/* buffer for attribute list */
};

/*
 * Arguments to VX_ATTRGET ioctl.
 */

struct vx_agbuf {
	vx_uq_t		class;		/* class of attribute */
	vx_uq_t		subclass;	/* subclass of attribute */
	vx_off_t	len;		/* length of buffer */
	caddr_t		bufp;		/* buffer for attribute */
};

/*
 * Format for buffers passed to VX_ATTRSET ioctl.
 */

struct vx_asent {
	vx_uq_t		function;	/* function */
	vx_sq_t		class;		/* class of attribute */
	vx_sq_t		subclass;	/* subclass of attribute */
	vx_off_t	len;		/* length of data */
	caddr_t		bufp;		/* attribute data */
};

/*
 * Arguments for the VX_CLONE ioctl.
 */

struct vx_clbuf {
	caddr_t		src;
	caddr_t	targ;
};

/*
 * Values for the set attribute functions.
 */

#define	VX_AREMOVE	0x1	/* Remove an attribute */
#define	VX_ASET		0x2	/* Set an attribute */
#define	VX_ARESTORE	0x3	/* Restore an attribute */

#define	VX_ALIST	0x4	/* Listing attributes */
#define	VX_AREAD	0x5	/* Reading attributes */

/*
 * Attribute area formats
 */

#define	VX_ATTR_IMMED		1	/* stored immediately in inode */
#define	VX_ATTR_DIRECT		2	/* attribute inum stored in inode */
#define	VX_ATTR_EXTIMDATA	3	/* extension of file immed data */

#define	VX_ATTR_INDIRECT	100	/* direct attribute stored in an
					   indirect attribute inode */

#define	VX_ATTR_ALL	(vx_uq_t)-1	/* wildcard class and subclass value */

#define	VX_FREECLASS	(vx_uq_t)-2	/* class value for free slot */

/*
 * Class and subclass fields are of type 32 bit unsigned. Class ids
 * in range of (vx_uq_t)-999 to 999 are now reserved. The unsignedness 
 * of class/subclass invalidates the check for negative ids, however,
 * there are set of attribute tests that specify negative class ids 
 * and expect failure. Until we change the attribute test suite not 
 * to test with negative class ids, we'll reject any 'reserved' class 
 * ids in range of (vx_uq_t)-999 to (vx_uq_t)-1 (used by fsqa test
 * suite) as invalid negative classes.
 */

#define VX_PUBLIC_CLASS_MIN	(vx_uq_t)-999	/* smallest resevered class id*/
#define VX_PUBLIC_CLASS_MAX	(vx_uq_t)999	/* largest resevered class id */
#define VX_MAX_CLASS		(vx_uq_t)-999	/* maximum class id */
#define VX_INVALID_CLASS(class)	((class) >= VX_MAX_CLASS)

#define	VX_ATTR_IGNORE	1	/* Don't allow operation to see attribute */
#define	VX_ATTR_KEEP	2	/* Perform operation as requested */
#define	VX_ATTR_NEW	3	/* Substitute new attribute data */
#define	VX_ATTR_LINK	4	/* Link to parent attribute */

struct vx_attrop {
	int	(*aop_listop)();	/* Listing/Reading attributes */
	int	(*aop_changeop)();	/* Setting/Removing attributes */
	int	(*aop_creatop)();	/* Inheriting attributes */
	int	(*aop_newop)();		/* New attributes on file create */
};

struct vx_attrreg {
	struct vx_attrreg	*ar_next;	/* next registered class */
	vx_uq_t			ar_class;	/* registered class */
	struct vx_attrop	ar_aops;	/* attribute operations */
	struct vx_attrreg	*ar_newop_next;	/* next class with newop */
};

#define	VX_NREGHASH	64	/* num of hash chains of registered classes */

#define VX_ATTROP_LOCK(mode) {					\
	VX_RWSLEEP_LOCK(&vx_attrop_lock, (mode));		\
	XTED_ATTROP_LOCK(mode);					\
}

#define VX_ATTROP_UNLOCK() {					\
	XTED_ATTROP_UNLOCK();					\
	VX_RWSLEEP_UNLOCK(&vx_attrop_lock);			\
}

/*
 * Known Attributes
 */

#define VX_ACL_CLASS		1
#define VX_ACL_SVR4_SUBCLASS	1

#define	VX_SECATTR_CLASS	2
#define	VX_SECATTR_SUBCLASS	1

#define	VX_NXFS_CLASS		3
#define	VX_DOS_ATTR		1
#define	VX_MAC_ATTR		2
#define	VX_OS2_ATTR		3

#define	VX_DCE_CLASS		4
#define	VX_DCE_ATTR		1

#endif	/* _FS_VXFS_VX_INODE_H */
