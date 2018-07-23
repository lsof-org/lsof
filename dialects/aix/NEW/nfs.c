#include <sys/cred.h>
#include <sys/vmuser.h>
#include <sys/vattr.h>
#include <sys/vnode.h>

typedef struct mutex {
#if	defined(_LP64)
	void    *_opaque[1];
#else	/* !defined(_LP64) */
	void    *_opaque[2];
#endif	/* defined(__LP64) */
} kmutex_t;

typedef struct _krwlock {
	void    *_opaque[1];
} krwlock_t;

typedef struct rnode {
	struct vmm_client_info vci;	/* VMM client info struct    */
	struct rnode	*r_freef;	/* free list forward pointer */
	struct rnode	*r_freeb;	/* free list back pointer */
	struct rnode	*r_hash;	/* rnode hash chain */
	vnode_t		r_vnode;	/* vnode for remote file */
	struct gnode	r_gnode;	/* gnode for remote file */
	int		r_vers;		/* which nfs rnode this is. 2/3 or 4 */
	krwlock_t	r_rwlock;	/* serializes write/setattr requests */
	kmutex_t	r_statelock;	/* protects (most of) rnode contents */
	kmutex_t	r_reflock;	/* protects v_count in vnode */
	u_short		r_flags;	/* flags, see below */
	short		r_error;	/* async write error */
	cred_t		*r_cred;	/* current credentials */
	cred_t		*r_altcred;	/* saved credentials */
	cred_t		*r_unlcred;	/* unlinked credentials */
	char		*r_unlname;	/* unlinked file name */
	vnode_t		*r_unldvp;	/* parent dir of unlinked file */
        offset_t        r_size;         /* client's view of file size */
	struct vattr	r_attr;		/* cached vnode attributes */
} rnode_t;

main() {
	rnode_t t;

	return(0);
}
