/* pstat_constants.h : shared between pm and lwp support. */

/*   @(#)B11.23_LR   */
#ifndef _SYS_PSTAT_PSTAT_CONSTANTS_INCLUDED
#define _SYS_PSTAT_PSTAT_CONSTANTS_INCLUDED

/*
 * Process states for pst_stat
 */
#define PS_SLEEP	1	/* Sleeping on pst_wchan value */
#define PS_RUN		2	/* Running/waiting for CPU */
#define PS_STOP		3	/* Stopped for ptrace() */
#define PS_ZOMBIE	4	/* Dead, waiting for parent to wait() */
#define PS_IDLE		6       /* Being created */
#define PS_OTHER	5	/* Misc. state (forking, exiting, etc.) */

/*
 * Scheduling policies for pst_schedpolicy
 */
#define PS_TIMESHARE	0x1	/* HP-UX scheduling (Tshare/FSS) */
#define PS_RTPRIO	0x2	/* HP-UX rtprio(2) scheduling */
#define PS_FIFO		0x4	/* P1003.4 FIFO scheduling */
#define PS_RR		0x8	/* P1003.4 Round Robin scheduling */
#define PS_RR2		0x10	/* Round Robin with variable timeslice */
#define PS_NOAGE	0x20	/* HP-UX TIMESHARE without usage decay */

#define PS_HPUX	PS_TIMESHARE	/* A.K.A. */

#define PST_MAXCLASSES 32	        /* Max. number of page classes */

/*
 * Constants used in certain pstat calls which return largepage information.
 * The arrays which are sized by PST_N_PG_SIZES are obsoleted and have been 
 * replaced by arrays which are sized by PST_N_PG_SIZES_FIRST.
 */
#define PST_N_PG_SIZES 16                /* obsolete */
#define PST_N_PG_SIZES_FIRST 32

#endif /* _SYS_PSTAT_PSTAT_CONSTANTS_INCLUDED */
