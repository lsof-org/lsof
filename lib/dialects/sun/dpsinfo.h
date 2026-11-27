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

/*
 * dpsinfo.h - Solaris/illumos psinfo reader for the procfs backend
 */

#if !defined(SOLARIS_DPSINFO_H)
#    define SOLARIS_DPSINFO_H 1

#    include <sys/types.h>

struct sun_psinfo {
    pid_t pid;
    pid_t ppid;
    pid_t pgid;
    uid_t uid;
    uid_t euid;
    char fname[256];
};

int read_sun_psinfo(const char *path, struct sun_psinfo *out);

#endif /* SOLARIS_DPSINFO_H */
