
/* @(#)92	1.4  src/bos/kernel/j2/include/j2_lock.h, sysj2, bos500IA 6/5/00 09:16:53 */
/*
 * COMPONENT_NAME: (SYSJ2) JFS2 Physical File System
 *
 * FUNCTIONS: 
 *
 * ORIGINS: 27
 *
 * (C) COPYRIGHT International Business Machines Corp. 1996, 1999
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

#ifndef _H_J2_LOCK
#define _H_J2_LOCK
/*
 * FUNCTION: lock/event definitions
 */

#include <sys/lock_def.h>
#include <sys/lock_alloc.h>
#include <sys/lockname.h>
#include <sys/sleep.h>

/*
 *		lock
 */
/*
 *	spin lock: thread-interrupt
 */
#define	IOSPINLOCK_T	Simple_lock
#ifdef	_POWER_MP
#define IOSPINLOCK_ALLOC(Lock,Flag,Class,Occurrence)\
	lock_alloc(Lock, LOCK_ALLOC_PIN, Class, Occurrence)
#define IOSPINLOCK_FREE(Lock)			lock_free(Lock)
#define IOSPINLOCK_INIT(Lock)			simple_lock_init(Lock)
#define IOSPINLOCK_DISABLE_LOCK(Lock)		disable_lock(INTIODONE,Lock)
#define IO_DISABLE()				i_disable(INTIODONE)
#define IOSPINLOCK_LOCK(Lock)			simple_lock(Lock)
#define IOSPINLOCK_LOCK_TRY(Lock)		simple_lock_try(Lock)
#define IOSPINLOCK_UNLOCK_ENABLE(xipl,Lock)	unlock_enable(xipl,Lock)
#define IOSPINLOCK_UNLOCK(Lock)			simple_unlock(Lock)
#define IO_ENABLE(xipl)				i_enable(xipl)
#else	/* UP */
#define IOSPINLOCK_ALLOC(Lock,Flag,Class,Occurrence)
#define IOSPINLOCK_FREE(Lock)
#define IOSPINLOCK_INIT(Lock)
#define IOSPINLOCK_DISABLE_LOCK(Lock)		i_disable(INTIODONE)
#define IO_DISABLE()				i_disable(INTIODONE)
#define IOSPINLOCK_LOCK(Lock)			
#define IOSPINLOCK_LOCK_TRY(Lock)		1
#define IOSPINLOCK_UNLOCK_ENABLE(xipl,Lock)	i_enable(xipl)
#define IOSPINLOCK_UNLOCK(Lock)	
#define IO_ENABLE(xipl)				i_enable(xipl)
#endif	/* _POWER_MP */

/*
 *	mutex lock: thread-thread
 */
#define	MUTEXLOCK_T	Simple_lock
#define MUTEXLOCK_ALLOC(Lock,Flag,Class,Occurrence)\
	lock_alloc(Lock, LOCK_ALLOC_PAGED, Class, Occurrence)
#define MUTEXLOCK_FREE(Lock)		lock_free(Lock)
#define MUTEXLOCK_INIT(Lock)		simple_lock_init(Lock)
#define MUTEXLOCK_LOCK(Lock)		simple_lock(Lock)
#define MUTEXLOCK_LOCK_TRY(Lock)	simple_lock_try(Lock)
#define	MUTEXLOCK_UNLOCK(Lock)		simple_unlock(Lock)

/*
 *	readers/writer lock: thread-thread
 */
#define	RDWRLOCK_T	Complex_lock
#define RDWRLOCK_ALLOC(Lock,Flag,Class,Occurrence)\
	lock_alloc(Lock, LOCK_ALLOC_PAGED, Class, Occurrence)
#define RDWRLOCK_FREE(Lock)		lock_free(Lock)
#define RDWRLOCK_INIT(Lock)		lock_init(Lock, 0)
#define READ_LOCK(Lock)			lock_read(Lock)
#define	READ_UNLOCK(Lock)		lock_done(Lock)
#define WRITE_LOCK(Lock)		lock_write(Lock)
#define WRITE_LOCK_TRY(Lock)	lock_try_write(Lock)
#define	WRITE_UNLOCK(Lock)		lock_done(Lock)

/*
 *		event
 *
 * int32 e_sleep_thread(int32 *Event, void *Lock, int32 Flag):
 * . thread is inserted on the event list anchored by Event;
 * . if Lock is not NULL, its lock type is specified by Flag, and
 *   it is released before blocked and reacquired when resumed
 *   (there's no T_NORELOCK option);
 *
 * void e_wakeup_w_result(int32 *Event, int32 Result):
 * . wake up ALL threads sleeping on the Event: set Event to EVENT_NULL;
 * . Result is returned as return value of e_sleep_thread();
 */
typedef	long		event_t;
#define event_init(event)	*(event) = EVENT_NULL

/* lock type flag for e_sleep_thread() */
#define	T_IOSPINLOCK	LOCK_HANDLER
#define	T_MUTEXLOCK		LOCK_SIMPLE
#define	T_READLOCK		LOCK_READ
#define	T_WRITELOCK		LOCK_WRITE

#define	EVENT_SLEEP(Event,Lock,Flag)	e_sleep_thread(Event,Lock,Flag)
#define	EVENT_SLEEP_RC(Event,Lock,Flag,rc)\
	rc = e_sleep_thread(Event,Lock,Flag)
#define	EVENT_WAKEUP(Event)		e_wakeup(Event)
#define	EVENT_WAKEUP_RC(Event,rc)	e_wakeup_w_result(Event,rc)


/*
 *	file system global lock
 *	-----------------------
 */
extern MUTEXLOCK_T	j2Lock;
#define J2_LOCK()		MUTEXLOCK_LOCK(&j2Lock)
#define J2_UNLOCK()		MUTEXLOCK_UNLOCK(&j2Lock)

/*
 *	inode read/write lock (per inode)
 */
#define IREAD_LOCK(ip)		READ_LOCK(&((ip)->i_rdwrlock))
#define IREAD_UNLOCK(ip)	READ_UNLOCK(&((ip)->i_rdwrlock))

#define IWRITE_LOCK(ip)	\
{\
	WRITE_LOCK(&((ip)->i_rdwrlock));\
	if (ip->i_fsxlock & FSXLOCK)\
	{\
		do {\
			ip->i_fsxlock |= FSXWANT;\
			EVENT_SLEEP(&(ip)->i_fsevent, &(ip)->i_rdwrlock, T_WRITELOCK);\
		} while (ip->i_fsxlock & FSXLOCK);\
	}\
}

#define IWRITE_LOCK_TRY(ip)	WRITE_LOCK_TRY(&((ip)->i_rdwrlock))
#define IWRITE_UNLOCK(ip)	WRITE_UNLOCK(&((ip)->i_rdwrlock))
#define IWRITE_LOCK_LIST	iWriteLockList

/*
 *	inode lock (per inode)
 */
#define INODE_LOCK(ip)		MUTEXLOCK_LOCK(&((ip)->i_nodelock))
#define INODE_LOCK_TRY(ip)	MUTEXLOCK_LOCK_TRY(&((ip)->i_nodelock))
#define INODE_UNLOCK(ip)	MUTEXLOCK_UNLOCK(&((ip)->i_nodelock))


/*
 *	rename lock (per file system)
 */
#define RENAME_LOCK(ipmnt)      MUTEXLOCK_LOCK(&((ipmnt)->i_renamelock))
#define RENAME_UNLOCK(ipmnt)    MUTEXLOCK_UNLOCK(&((ipmnt)->i_renamelock))

/*
 *	i/o cache lock
 */
extern IOSPINLOCK_T	j2IOCacheLock;

#endif /* _H_J2_LOCK */
