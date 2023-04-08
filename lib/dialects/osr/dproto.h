/*
 * dproto.h - SCO OpenServer function prototypes for lsof
 *
 * The _PROTOTYPE macro is defined in the common proto.h.
 */

/*
 * Copyright 1995 Purdue Research Foundation, West Lafayette, Indiana
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

/*
 * $Id: dproto.h,v 1.5 99/06/22 08:22:28 abe Exp $
 */

extern int is_file_named(char *p, int cd);
extern void process_socket(struct inode *i);
extern int get_max_fd(void);

#if OSRV < 500
extern int endservent(void);
extern int setservent(int);

#    if defined(HASSTATLSTAT)
extern int statlstat(const char *, struct stat *);
#    endif /* defined(HASTSTATLSTAT) */

extern int strcasecmp(char *, char *);
extern int strncasecmp(char *, char *, unsigned int);
extern pid_t wait();
#endif /* OSRV<500 */

extern int sysi86();
extern int sysfs();
extern void udp_tm(time_t tm);

#if !defined(N_UNIX)
extern char *get_nlist_path(int pd);
#endif /* !defined(N_UNIX) */
